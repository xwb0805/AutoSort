#include "filepreviewdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QPixmap>
#include <QMovie>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QGroupBox>

FilePreviewDialog::FilePreviewDialog(const QFileInfo &fileInfo, QWidget *parent)
    : QDialog(parent)
    , m_fileInfo(fileInfo)
{
    setupUI();
    previewFile();
}

FilePreviewDialog::~FilePreviewDialog()
{
}

void FilePreviewDialog::setupUI()
{
    setWindowTitle("文件预览");
    setMinimumSize(600, 500);

    m_mainLayout = new QVBoxLayout(this);

    // 文件信息区域
    QGroupBox *infoGroup = new QGroupBox("文件信息", this);
    QGridLayout *infoLayout = new QGridLayout(infoGroup);

    infoLayout->addWidget(new QLabel("路径:", this), 0, 0);
    m_filePathLabel = new QLabel(this);
    m_filePathLabel->setWordWrap(true);
    infoLayout->addWidget(m_filePathLabel, 0, 1);

    infoLayout->addWidget(new QLabel("大小:", this), 1, 0);
    m_fileSizeLabel = new QLabel(this);
    infoLayout->addWidget(m_fileSizeLabel, 1, 1);

    infoLayout->addWidget(new QLabel("类型:", this), 2, 0);
    m_fileTypeLabel = new QLabel(this);
    infoLayout->addWidget(m_fileTypeLabel, 2, 1);

    infoLayout->addWidget(new QLabel("修改时间:", this), 3, 0);
    m_modifiedTimeLabel = new QLabel(this);
    infoLayout->addWidget(m_modifiedTimeLabel, 3, 1);

    m_mainLayout->addWidget(infoGroup);

    // 预览标签页
    m_tabWidget = new QTabWidget(this);

    // 图片预览
    m_imagePreviewWidget = new QWidget(this);
    QVBoxLayout *imageLayout = new QVBoxLayout(m_imagePreviewWidget);
    m_imageLabel = new QLabel(this);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet("QLabel { background-color: #f0f0f0; }");
    imageLayout->addWidget(m_imageLabel);

    // 文本预览
    m_textPreviewWidget = new QWidget(this);
    QVBoxLayout *textLayout = new QVBoxLayout(m_textPreviewWidget);
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);
    m_textEdit->setFont(QFont("Courier New", 10));
    textLayout->addWidget(m_textEdit);

    // 详细信息预览
    m_infoPreviewWidget = new QWidget(this);
    QVBoxLayout *detailsLayout = new QVBoxLayout(m_infoPreviewWidget);
    m_infoTable = new QTableWidget(this);
    m_infoTable->setColumnCount(2);
    m_infoTable->setHorizontalHeaderLabels(QStringList() << "属性" << "值");
    m_infoTable->horizontalHeader()->setStretchLastSection(true);
    m_infoTable->verticalHeader()->setVisible(false);
    detailsLayout->addWidget(m_infoTable);

    m_tabWidget->addTab(m_imagePreviewWidget, "图片");
    m_tabWidget->addTab(m_textPreviewWidget, "文本");
    m_tabWidget->addTab(m_infoPreviewWidget, "详细信息");

    m_mainLayout->addWidget(m_tabWidget);

    // 关闭按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    m_closeButton = new QPushButton("关闭", this);
    buttonLayout->addWidget(m_closeButton);
    m_mainLayout->addLayout(buttonLayout);

    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void FilePreviewDialog::previewFile()
{
    // 设置文件信息
    m_filePathLabel->setText(m_fileInfo.absoluteFilePath());
    m_fileSizeLabel->setText(formatFileSize(m_fileInfo.size()));
    m_fileTypeLabel->setText(m_fileInfo.suffix().toUpper() + " 文件");
    m_modifiedTimeLabel->setText(m_fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss"));

    // 填充详细信息表格
    m_infoTable->setRowCount(0);
    m_infoTable->insertRow(0);
    m_infoTable->setItem(0, 0, new QTableWidgetItem("文件名"));
    m_infoTable->setItem(0, 1, new QTableWidgetItem(m_fileInfo.fileName()));

    m_infoTable->insertRow(1);
    m_infoTable->setItem(1, 0, new QTableWidgetItem("完整路径"));
    m_infoTable->setItem(1, 1, new QTableWidgetItem(m_fileInfo.absoluteFilePath()));

    m_infoTable->insertRow(2);
    m_infoTable->setItem(2, 0, new QTableWidgetItem("文件大小"));
    m_infoTable->setItem(2, 1, new QTableWidgetItem(formatFileSize(m_fileInfo.size())));

    m_infoTable->insertRow(3);
    m_infoTable->setItem(3, 0, new QTableWidgetItem("文件类型"));
    m_infoTable->setItem(3, 1, new QTableWidgetItem(m_fileInfo.suffix().toUpper()));

    m_infoTable->insertRow(4);
    m_infoTable->setItem(4, 0, new QTableWidgetItem("创建时间"));
    m_infoTable->setItem(4, 1, new QTableWidgetItem(m_fileInfo.birthTime().toString("yyyy-MM-dd hh:mm:ss")));

    m_infoTable->insertRow(5);
    m_infoTable->setItem(5, 0, new QTableWidgetItem("修改时间"));
    m_infoTable->setItem(5, 1, new QTableWidgetItem(m_fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss")));

    m_infoTable->insertRow(6);
    m_infoTable->setItem(6, 0, new QTableWidgetItem("访问时间"));
    m_infoTable->setItem(6, 1, new QTableWidgetItem(m_fileInfo.lastRead().toString("yyyy-MM-dd hh:mm:ss")));

    m_infoTable->insertRow(7);
    m_infoTable->setItem(7, 0, new QTableWidgetItem("是否可读"));
    m_infoTable->setItem(7, 1, new QTableWidgetItem(m_fileInfo.isReadable() ? "是" : "否"));

    m_infoTable->insertRow(8);
    m_infoTable->setItem(8, 0, new QTableWidgetItem("是否可写"));
    m_infoTable->setItem(8, 1, new QTableWidgetItem(m_fileInfo.isWritable() ? "是" : "否"));

    m_infoTable->insertRow(9);
    m_infoTable->setItem(9, 0, new QTableWidgetItem("是否可执行"));
    m_infoTable->setItem(9, 1, new QTableWidgetItem(m_fileInfo.isExecutable() ? "是" : "否"));

    m_infoTable->resizeColumnsToContents();

    // 根据文件类型预览
    if (m_fileInfo.isDir()) {
        previewDirectory();
    } else if (m_fileInfo.suffix().toLower() == "jpg" ||
               m_fileInfo.suffix().toLower() == "jpeg" ||
               m_fileInfo.suffix().toLower() == "png" ||
               m_fileInfo.suffix().toLower() == "gif" ||
               m_fileInfo.suffix().toLower() == "bmp" ||
               m_fileInfo.suffix().toLower() == "webp") {
        previewImage();
    } else if (m_fileInfo.suffix().toLower() == "txt" ||
               m_fileInfo.suffix().toLower() == "md" ||
               m_fileInfo.suffix().toLower() == "json" ||
               m_fileInfo.suffix().toLower() == "xml" ||
               m_fileInfo.suffix().toLower() == "html" ||
               m_fileInfo.suffix().toLower() == "css" ||
               m_fileInfo.suffix().toLower() == "js" ||
               m_fileInfo.suffix().toLower() == "cpp" ||
               m_fileInfo.suffix().toLower() == "h" ||
               m_fileInfo.suffix().toLower() == "py" ||
               m_fileInfo.suffix().toLower() == "java") {
        previewText();
    } else {
        previewBinary();
    }
}

void FilePreviewDialog::previewImage()
{
    QPixmap pixmap(m_fileInfo.absoluteFilePath());
    if (!pixmap.isNull()) {
        // 缩放图片以适应窗口
        QPixmap scaledPixmap = pixmap.scaled(500, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_imageLabel->setPixmap(scaledPixmap);
        m_tabWidget->setCurrentWidget(m_imagePreviewWidget);
    } else {
        m_imageLabel->setText("无法预览此图片");
    }
}

void FilePreviewDialog::previewText()
{
    QFile file(m_fileInfo.absoluteFilePath());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        // 限制显示的文本长度
        if (content.length() > 50000) {
            content = content.left(50000) + "\n\n... (内容过长，已截断) ...";
        }

        m_textEdit->setPlainText(content);
        m_tabWidget->setCurrentWidget(m_textPreviewWidget);
    } else {
        m_textEdit->setPlainText("无法预览此文件");
    }
}

void FilePreviewDialog::previewBinary()
{
    m_textEdit->setPlainText("此文件类型不支持文本预览。\n\n支持预览的文件类型：\n• 图片: JPG, PNG, GIF, BMP, WEBP\n• 文本: TXT, MD, JSON, XML, HTML, CSS, JS, CPP, H, PY, JAVA");
    m_tabWidget->setCurrentWidget(m_textPreviewWidget);
}

void FilePreviewDialog::previewDirectory()
{
    m_textEdit->setPlainText("这是一个文件夹，无法预览内容。\n\n文件夹信息：\n路径: " + m_fileInfo.absoluteFilePath());
    m_tabWidget->setCurrentWidget(m_textPreviewWidget);
}

QString FilePreviewDialog::formatFileSize(qint64 size) const
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