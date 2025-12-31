#include "batchrenamedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

BatchRenameDialog::BatchRenameDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

BatchRenameDialog::~BatchRenameDialog()
{
}

void BatchRenameDialog::setupUI()
{
    setWindowTitle("批量重命名");
    setMinimumSize(900, 700);

    m_mainLayout = new QVBoxLayout(this);

    // 路径选择
    QGroupBox *pathGroup = new QGroupBox("文件夹路径", this);
    QHBoxLayout *pathLayout = new QHBoxLayout(pathGroup);

    m_pathEdit = new QLineEdit(this);
    pathLayout->addWidget(m_pathEdit);

    m_browseButton = new QPushButton("浏览...", this);
    pathLayout->addWidget(m_browseButton);

    m_loadButton = new QPushButton("加载文件", this);
    pathLayout->addWidget(m_loadButton);

    m_mainLayout->addWidget(pathGroup);

    // 文件列表
    m_filesTable = new QTableWidget(this);
    m_filesTable->setColumnCount(4);
    m_filesTable->setHorizontalHeaderLabels(QStringList() << "原文件名" << "新文件名" << "大小" << "类型");
    m_filesTable->horizontalHeader()->setStretchLastSection(true);
    m_filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_mainLayout->addWidget(m_filesTable);

    // 重命名选项
    m_renameGroup = new QGroupBox("重命名规则", this);
    m_renameLayout = new QGridLayout(m_renameGroup);

    m_renameLayout->addWidget(new QLabel("重命名模式:", this), 0, 0);
    m_renameModeCombo = new QComboBox(this);
    m_renameModeCombo->addItem("前缀 + 序号", 0);
    m_renameModeCombo->addItem("后缀 + 序号", 1);
    m_renameModeCombo->addItem("替换文本", 2);
    m_renameModeCombo->addItem("自定义模板", 3);
    m_renameLayout->addWidget(m_renameModeCombo, 0, 1);

    m_renameLayout->addWidget(new QLabel("前缀:", this), 1, 0);
    m_prefixEdit = new QLineEdit(this);
    m_prefixEdit->setPlaceholderText("文件名前缀");
    m_renameLayout->addWidget(m_prefixEdit, 1, 1);

    m_renameLayout->addWidget(new QLabel("后缀:", this), 2, 0);
    m_suffixEdit = new QLineEdit(this);
    m_suffixEdit->setPlaceholderText("文件名后缀");
    m_renameLayout->addWidget(m_suffixEdit, 2, 1);

    m_renameLayout->addWidget(new QLabel("序号起始:", this), 3, 0);
    m_counterStartSpinBox = new QSpinBox(this);
    m_counterStartSpinBox->setMinimum(0);
    m_counterStartSpinBox->setMaximum(99999);
    m_counterStartSpinBox->setValue(1);
    m_renameLayout->addWidget(m_counterStartSpinBox, 3, 1);

    m_renameLayout->addWidget(new QLabel("查找:", this), 4, 0);
    m_replaceOldEdit = new QLineEdit(this);
    m_replaceOldEdit->setPlaceholderText("要替换的文本");
    m_renameLayout->addWidget(m_replaceOldEdit, 4, 1);

    m_renameLayout->addWidget(new QLabel("替换为:", this), 5, 0);
    m_replaceNewEdit = new QLineEdit(this);
    m_replaceNewEdit->setPlaceholderText("新文本");
    m_renameLayout->addWidget(m_replaceNewEdit, 5, 1);

    m_caseSensitiveCheckBox = new QCheckBox("区分大小写", this);
    m_renameLayout->addWidget(m_caseSensitiveCheckBox, 6, 0, 1, 2);

    m_mainLayout->addWidget(m_renameGroup);

    // 操作按钮
    m_actionLayout = new QHBoxLayout();

    m_addButton = new QPushButton("添加文件", this);
    m_actionLayout->addWidget(m_addButton);

    m_removeButton = new QPushButton("移除选中", this);
    m_actionLayout->addWidget(m_removeButton);

    m_clearButton = new QPushButton("清空列表", this);
    m_actionLayout->addWidget(m_clearButton);

    m_actionLayout->addStretch();

    m_previewButton = new QPushButton("预览", this);
    m_actionLayout->addWidget(m_previewButton);

    m_executeButton = new QPushButton("执行重命名", this);
    m_executeButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; }");
    m_actionLayout->addWidget(m_executeButton);

    m_mainLayout->addLayout(m_actionLayout);

    // 关闭按钮
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    m_closeButton = new QPushButton("关闭", this);
    closeLayout->addWidget(m_closeButton);
    m_mainLayout->addLayout(closeLayout);

    // 连接信号
    connect(m_browseButton, &QPushButton::clicked, this, &BatchRenameDialog::browsePath);
    connect(m_loadButton, &QPushButton::clicked, this, &BatchRenameDialog::loadFiles);
    connect(m_addButton, &QPushButton::clicked, this, &BatchRenameDialog::addFiles);
    connect(m_removeButton, &QPushButton::clicked, this, &BatchRenameDialog::removeFiles);
    connect(m_clearButton, &QPushButton::clicked, this, &BatchRenameDialog::clearFiles);
    connect(m_previewButton, &QPushButton::clicked, this, &BatchRenameDialog::previewRename);
    connect(m_executeButton, &QPushButton::clicked, this, &BatchRenameDialog::executeRename);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);

    connect(m_renameModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &BatchRenameDialog::onPatternChanged);
    connect(m_counterStartSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &BatchRenameDialog::onCounterStartChanged);
}

void BatchRenameDialog::browsePath()
{
    QString path = QFileDialog::getExistingDirectory(this, "选择文件夹", m_pathEdit->text());
    if (!path.isEmpty()) {
        m_pathEdit->setText(path);
    }
}

void BatchRenameDialog::loadFiles()
{
    QString path = m_pathEdit->text();
    if (path.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入文件夹路径");
        return;
    }

    QDir dir(path);
    if (!dir.exists()) {
        QMessageBox::warning(this, "错误", "文件夹不存在");
        return;
    }

    QFileInfoList fileList = dir.entryInfoList(QDir::Files, QDir::Name);
    m_fileList.clear();
    m_filesTable->setRowCount(0);

    for (const QFileInfo &fileInfo : fileList) {
        m_fileList.append(fileInfo);

        int row = m_filesTable->rowCount();
        m_filesTable->insertRow(row);

        m_filesTable->setItem(row, 0, new QTableWidgetItem(fileInfo.fileName()));
        m_filesTable->setItem(row, 1, new QTableWidgetItem(""));
        m_filesTable->setItem(row, 2, new QTableWidgetItem(formatFileSize(fileInfo.size())));
        m_filesTable->setItem(row, 3, new QTableWidgetItem(fileInfo.suffix().toUpper()));
    }

    QMessageBox::information(this, "成功", QString("已加载 %1 个文件").arg(m_fileList.size()));
}

void BatchRenameDialog::addFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "选择文件", m_pathEdit->text());
    if (files.isEmpty()) {
        return;
    }

    for (const QString &filePath : files) {
        QFileInfo fileInfo(filePath);
        m_fileList.append(fileInfo);

        int row = m_filesTable->rowCount();
        m_filesTable->insertRow(row);

        m_filesTable->setItem(row, 0, new QTableWidgetItem(fileInfo.fileName()));
        m_filesTable->setItem(row, 1, new QTableWidgetItem(""));
        m_filesTable->setItem(row, 2, new QTableWidgetItem(formatFileSize(fileInfo.size())));
        m_filesTable->setItem(row, 3, new QTableWidgetItem(fileInfo.suffix().toUpper()));
    }
}

void BatchRenameDialog::removeFiles()
{
    QList<QTableWidgetSelectionRange> selectedRanges = m_filesTable->selectedRanges();
    if (selectedRanges.isEmpty()) {
        return;
    }

    // 从后往前删除，避免索引变化
    for (int i = selectedRanges.size() - 1; i >= 0; --i) {
        QTableWidgetSelectionRange range = selectedRanges[i];
        for (int row = range.bottomRow(); row >= range.topRow(); --row) {
            m_filesTable->removeRow(row);
            if (row < m_fileList.size()) {
                m_fileList.removeAt(row);
            }
        }
    }
}

void BatchRenameDialog::clearFiles()
{
    m_fileList.clear();
    m_filesTable->setRowCount(0);
}

void BatchRenameDialog::previewRename()
{
    updatePreview();
    QMessageBox::information(this, "预览", "预览已更新，请查看\"新文件名\"列");
}

void BatchRenameDialog::executeRename()
{
    if (m_fileList.isEmpty()) {
        QMessageBox::warning(this, "错误", "没有文件需要重命名");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定要执行重命名操作吗？\n此操作不可撤销！",
                              QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    int successCount = 0;
    int failCount = 0;

    for (int i = 0; i < m_fileList.size(); ++i) {
        QFileInfo fileInfo = m_fileList[i];
        QString newName = generateNewName(fileInfo.fileName(), i);

        if (newName.isEmpty() || newName == fileInfo.fileName()) {
            continue;
        }

        QString newPath = fileInfo.absolutePath() + "/" + newName;

        if (QFile::rename(fileInfo.absoluteFilePath(), newPath)) {
            successCount++;
        } else {
            failCount++;
            qWarning() << "Failed to rename:" << fileInfo.fileName() << "to" << newName;
        }
    }

    QMessageBox::information(this, "完成",
        QString("重命名完成！\n成功: %1\n失败: %2").arg(successCount).arg(failCount));

    // 重新加载文件列表
    if (!m_pathEdit->text().isEmpty()) {
        loadFiles();
    }
}

void BatchRenameDialog::onPatternChanged()
{
    int mode = m_renameModeCombo->currentIndex();

    // 根据模式启用/禁用控件
    m_prefixEdit->setEnabled(mode == 0 || mode == 3);
    m_suffixEdit->setEnabled(mode == 1 || mode == 3);
    m_counterStartSpinBox->setEnabled(mode == 0 || mode == 1 || mode == 3);
    m_replaceOldEdit->setEnabled(mode == 2);
    m_replaceNewEdit->setEnabled(mode == 2);
}

void BatchRenameDialog::onCounterStartChanged()
{
    updatePreview();
}

void BatchRenameDialog::updatePreview()
{
    for (int i = 0; i < m_fileList.size(); ++i) {
        QString newName = generateNewName(m_fileList[i].fileName(), i);
        if (m_filesTable->item(i, 1)) {
            m_filesTable->item(i, 1)->setText(newName);
        }
    }
}

QString BatchRenameDialog::generateNewName(const QString &oldName, int index)
{
    QFileInfo fileInfo(oldName);
    QString baseName = fileInfo.baseName();
    QString suffix = fileInfo.suffix();

    int mode = m_renameModeCombo->currentIndex();
    QString newName;

    switch (mode) {
        case 0: // 前缀 + 序号
        {
            QString prefix = m_prefixEdit->text();
            int counter = m_counterStartSpinBox->value() + index;
            newName = QString("%1%2.%3").arg(prefix).arg(counter, 3, 10, QChar('0')).arg(suffix);
            break;
        }
        case 1: // 后缀 + 序号
        {
            QString suffixText = m_suffixEdit->text();
            int counter = m_counterStartSpinBox->value() + index;
            newName = QString("%1%2%3.%4").arg(baseName).arg(suffixText).arg(counter, 3, 10, QChar('0')).arg(suffix);
            break;
        }
        case 2: // 替换文本
        {
            QString oldText = m_replaceOldEdit->text();
            QString newText = m_replaceNewEdit->text();
            if (!oldText.isEmpty()) {
                Qt::CaseSensitivity cs = m_caseSensitiveCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
                newName = baseName.replace(oldText, newText, cs);
            } else {
                newName = baseName;
            }
            if (!suffix.isEmpty()) {
                newName += "." + suffix;
            }
            break;
        }
        case 3: // 自定义模板
        {
            QString prefix = m_prefixEdit->text();
            QString suffixText = m_suffixEdit->text();
            int counter = m_counterStartSpinBox->value() + index;
            newName = QString("%1%2%3.%4").arg(prefix).arg(baseName).arg(suffixText).arg(counter, 3, 10, QChar('0'));
            if (!suffix.isEmpty()) {
                newName += "." + suffix;
            }
            break;
        }
    }

    return newName;
}

QString BatchRenameDialog::formatFileSize(qint64 size) const
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
