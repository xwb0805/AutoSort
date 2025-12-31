#include "filesearchdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressBar>
#include <QGroupBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QRegularExpression>
#include <QApplication>

FileSearchDialog::FileSearchDialog(QWidget *parent)
    : QDialog(parent)
    , m_isSearching(false)
    , m_stopRequested(false)
{
    setupUI();
}

FileSearchDialog::~FileSearchDialog()
{
}

void FileSearchDialog::setupUI()
{
    setWindowTitle("文件搜索");
    setMinimumSize(900, 700);

    m_mainLayout = new QVBoxLayout(this);

    // 路径选择
    QGroupBox *pathGroup = new QGroupBox("搜索路径", this);
    m_pathLayout = new QHBoxLayout(pathGroup);

    m_pathLayout->addWidget(new QLabel("路径:", this));
    m_pathEdit = new QLineEdit(this);
    m_pathEdit->setText(QDir::homePath());
    m_pathLayout->addWidget(m_pathEdit);

    m_browseButton = new QPushButton("浏览...", this);
    m_pathLayout->addWidget(m_browseButton);

    m_mainLayout->addWidget(pathGroup);

    // 搜索条件
    QGroupBox *searchGroup = new QGroupBox("搜索条件", this);
    QGridLayout *searchLayout = new QGridLayout(searchGroup);

    searchLayout->addWidget(new QLabel("文件名:", this), 0, 0);
    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("支持通配符 * 和 ?");
    searchLayout->addWidget(m_nameEdit, 0, 1);

    searchLayout->addWidget(new QLabel("扩展名:", this), 1, 0);
    m_extensionEdit = new QLineEdit(this);
    m_extensionEdit->setPlaceholderText("例如: txt, pdf, jpg");
    searchLayout->addWidget(m_extensionEdit, 1, 1);

    searchLayout->addWidget(new QLabel("文件大小:", this), 2, 0);
    m_sizeOperatorCombo = new QComboBox(this);
    m_sizeOperatorCombo->addItem("大于", 0);
    m_sizeOperatorCombo->addItem("小于", 1);
    m_sizeOperatorCombo->addItem("等于", 2);
    m_sizeOperatorCombo->addItem("介于", 3);
    searchLayout->addWidget(m_sizeOperatorCombo, 2, 1);

    m_sizeEdit = new QLineEdit(this);
    m_sizeEdit->setPlaceholderText("大小");
    searchLayout->addWidget(m_sizeEdit, 2, 2);

    m_sizeUnitCombo = new QComboBox(this);
    m_sizeUnitCombo->addItem("KB", 1024);
    m_sizeUnitCombo->addItem("MB", 1024 * 1024);
    m_sizeUnitCombo->addItem("GB", 1024 * 1024 * 1024);
    m_sizeUnitCombo->setCurrentIndex(1);
    searchLayout->addWidget(m_sizeUnitCombo, 2, 3);

    m_includeSubdirsCheckBox = new QCheckBox("包含子文件夹", this);
    m_includeSubdirsCheckBox->setChecked(true);
    searchLayout->addWidget(m_includeSubdirsCheckBox, 3, 0, 1, 2);

    m_caseSensitiveCheckBox = new QCheckBox("区分大小写", this);
    searchLayout->addWidget(m_caseSensitiveCheckBox, 3, 2, 1, 2);

    m_mainLayout->addWidget(searchGroup);

    // 搜索按钮
    QHBoxLayout *searchButtonLayout = new QHBoxLayout();

    m_searchButton = new QPushButton("开始搜索", this);
    m_searchButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    searchButtonLayout->addWidget(m_searchButton);

    m_stopButton = new QPushButton("停止", this);
    m_stopButton->setEnabled(false);
    m_stopButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; padding: 8px; }");
    searchButtonLayout->addWidget(m_stopButton);

    searchButtonLayout->addStretch();

    m_mainLayout->addLayout(searchButtonLayout);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_mainLayout->addWidget(m_progressBar);

    m_statusLabel = new QLabel("就绪", this);
    m_mainLayout->addWidget(m_statusLabel);

    // 搜索结果
    m_resultsTable = new QTableWidget(this);
    m_resultsTable->setColumnCount(5);
    m_resultsTable->setHorizontalHeaderLabels(QStringList() << "文件名" << "路径" << "大小" << "类型" << "修改时间");
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultsTable->setSortingEnabled(true);
    m_mainLayout->addWidget(m_resultsTable);

    // 操作按钮
    m_buttonLayout = new QHBoxLayout();

    m_openButton = new QPushButton("打开文件", this);
    m_buttonLayout->addWidget(m_openButton);

    m_infoButton = new QPushButton("详细信息", this);
    m_buttonLayout->addWidget(m_infoButton);

    m_buttonLayout->addStretch();

    m_closeButton = new QPushButton("关闭", this);
    m_buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_buttonLayout);

    // 连接信号
    connect(m_browseButton, &QPushButton::clicked, this, &FileSearchDialog::browsePath);
    connect(m_searchButton, &QPushButton::clicked, this, &FileSearchDialog::startSearch);
    connect(m_stopButton, &QPushButton::clicked, this, &FileSearchDialog::stopSearch);
    connect(m_openButton, &QPushButton::clicked, this, &FileSearchDialog::openSelectedFile);
    connect(m_infoButton, &QPushButton::clicked, this, &FileSearchDialog::showSelectedFileInfo);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void FileSearchDialog::browsePath()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择搜索路径", m_pathEdit->text());
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void FileSearchDialog::startSearch()
{
    if (m_isSearching) {
        return;
    }

    QString path = m_pathEdit->text();
    if (path.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入搜索路径");
        return;
    }

    QDir dir(path);
    if (!dir.exists()) {
        QMessageBox::warning(this, "错误", "路径不存在");
        return;
    }

    m_isSearching = true;
    m_stopRequested = false;
    m_searchButton->setEnabled(false);
    m_stopButton->setEnabled(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0); // 不确定进度
    m_statusLabel->setText("正在搜索...");

    m_resultsTable->setRowCount(0);

    QString namePattern = m_nameEdit->text();
    QString extension = m_extensionEdit->text().toLower();

    qint64 minSize = 0;
    qint64 maxSize = Q_INT64_C(-1);

    if (!m_sizeEdit->text().isEmpty()) {
        qint64 size = m_sizeEdit->text().toLongLong();
        qint64 unit = m_sizeUnitCombo->currentData().toLongLong();
        qint64 bytes = size * unit;

        int operatorIndex = m_sizeOperatorCombo->currentIndex();
        if (operatorIndex == 0) { // 大于
            minSize = bytes;
        } else if (operatorIndex == 1) { // 小于
            maxSize = bytes;
        } else if (operatorIndex == 2) { // 等于
            minSize = bytes;
            maxSize = bytes;
        }
        // 介于情况需要两个输入框，这里简化处理
    }

    bool includeSubdirs = m_includeSubdirsCheckBox->isChecked();
    bool caseSensitive = m_caseSensitiveCheckBox->isChecked();

    searchFiles(path, namePattern, extension, minSize, maxSize, includeSubdirs);

    m_isSearching = false;
    m_searchButton->setEnabled(true);
    m_stopButton->setEnabled(false);
    m_progressBar->setVisible(false);
    m_statusLabel->setText(QString("搜索完成，找到 %1 个文件").arg(m_resultsTable->rowCount()));
}

void FileSearchDialog::stopSearch()
{
    if (m_isSearching) {
        m_stopRequested = true;
        m_statusLabel->setText("正在停止...");
    }
}

void FileSearchDialog::searchFiles(const QString &path, const QString &namePattern, const QString &extension,
                                   qint64 minSize, qint64 maxSize, bool includeSubdirs)
{
    QDir dir(path);
    QFileInfoList fileList;

    if (includeSubdirs) {
        fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    } else {
        fileList = dir.entryInfoList(QDir::Files, QDir::Name);
    }

    for (const QFileInfo &fileInfo : fileList) {
        if (m_stopRequested) {
            break;
        }

        // 检查文件名
        if (!namePattern.isEmpty()) {
            QString fileName = m_caseSensitiveCheckBox->isChecked() ? fileInfo.fileName() : fileInfo.fileName().toLower();
            QString pattern = m_caseSensitiveCheckBox->isChecked() ? namePattern : namePattern.toLower();

            // 将通配符转换为正则表达式
            QString regexPattern = QRegularExpression::wildcardToRegularExpression(pattern);
            QRegularExpression regex(regexPattern, m_caseSensitiveCheckBox->isChecked() ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption);
            if (!regex.match(fileName).hasMatch()) {
                continue;
            }
        }

        // 检查扩展名
        if (!extension.isEmpty()) {
            QStringList extList = extension.split(',', Qt::SkipEmptyParts);
            QString fileExt = fileInfo.suffix().toLower();
            bool match = false;
            for (const QString &ext : extList) {
                if (fileExt == ext.trimmed().toLower()) {
                    match = true;
                    break;
                }
            }
            if (!match) {
                continue;
            }
        }

        // 检查文件大小
        qint64 fileSize = fileInfo.size();
        if (minSize > 0 && fileSize < minSize) {
            continue;
        }
        if (maxSize >= 0 && fileSize > maxSize) {
            continue;
        }

        addSearchResult(fileInfo);

        QApplication::processEvents();
    }

    // 递归搜索子文件夹
    if (includeSubdirs && !m_stopRequested) {
        QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &dirInfo : dirList) {
            if (m_stopRequested) {
                break;
            }
            searchFiles(dirInfo.absoluteFilePath(), namePattern, extension, minSize, maxSize, includeSubdirs);
        }
    }
}

void FileSearchDialog::addSearchResult(const QFileInfo &fileInfo)
{
    int row = m_resultsTable->rowCount();
    m_resultsTable->insertRow(row);

    m_resultsTable->setItem(row, 0, new QTableWidgetItem(fileInfo.fileName()));
    m_resultsTable->setItem(row, 1, new QTableWidgetItem(fileInfo.absolutePath()));
    m_resultsTable->setItem(row, 2, new QTableWidgetItem(formatFileSize(fileInfo.size())));
    m_resultsTable->setItem(row, 3, new QTableWidgetItem(fileInfo.suffix().toUpper()));
    m_resultsTable->setItem(row, 4, new QTableWidgetItem(fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")));
}

QString FileSearchDialog::formatFileSize(qint64 size) const
{
    const qint64 KB = 1024;
    const qint64 MB = 1024 * KB;
    const qint64 GB = 1024 * MB;

    if (size < KB) {
        return QString("%1 B").arg(size);
    } else if (size < MB) {
        return QString("%1 KB").arg(size / (double)KB, 0, 'f', 2);
    } else if (size < GB) {
        return QString("%1 MB").arg(size / (double)MB, 0, 'f', 2);
    } else {
        return QString("%1 GB").arg(size / (double)GB, 0, 'f', 2);
    }
}

void FileSearchDialog::updateProgress(int current, int total)
{
    if (total > 0) {
        m_progressBar->setRange(0, total);
        m_progressBar->setValue(current);
    }
}

void FileSearchDialog::openSelectedFile()
{
    int currentRow = m_resultsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "提示", "请选择一个文件");
        return;
    }

    QString filePath = m_resultsTable->item(currentRow, 0)->data(Qt::UserRole).toString();
    if (filePath.isEmpty()) {
        filePath = m_resultsTable->item(currentRow, 1)->text() + "/" + m_resultsTable->item(currentRow, 0)->text();
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void FileSearchDialog::showSelectedFileInfo()
{
    int currentRow = m_resultsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "提示", "请选择一个文件");
        return;
    }

    QString fileName = m_resultsTable->item(currentRow, 0)->text();
    QString path = m_resultsTable->item(currentRow, 1)->text();
    QString size = m_resultsTable->item(currentRow, 2)->text();
    QString type = m_resultsTable->item(currentRow, 3)->text();
    QString modified = m_resultsTable->item(currentRow, 4)->text();

    QString info = QString("文件名: %1\n路径: %2\n大小: %3\n类型: %4\n修改时间: %5")
        .arg(fileName).arg(path).arg(size).arg(type).arg(modified);

    QMessageBox::information(this, "文件信息", info);
}