#include "statisticsdialog.h"
#include "statistics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>
#include <QProgressBar>
#include <QFrame>
#include <QDebug>

StatisticsDialog::StatisticsDialog(Statistics *statistics, QWidget *parent)
    : QDialog(parent)
    , m_statistics(statistics)
{
    qDebug() << "StatisticsDialog constructor called";
    qDebug() << "Parent widget:" << parent;
    qDebug() << "Statistics pointer:" << statistics;

    setupUI();
    qDebug() << "setupUI completed";

    updateStatisticsDisplay();
    qDebug() << "updateStatisticsDisplay completed";

    qDebug() << "Dialog size:" << size();
    qDebug() << "Dialog visible:" << isVisible();
}

StatisticsDialog::~StatisticsDialog()
{
}

void StatisticsDialog::setupUI()
{
    qDebug() << "StatisticsDialog::setupUI() started";

    setWindowTitle("统计信息");
    setMinimumSize(600, 400);
    resize(800, 600);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    qDebug() << "Created main layout";

    // 摘要信息
    m_summaryGroup = new QGroupBox("摘要", this);
    QGridLayout *summaryLayout = new QGridLayout(m_summaryGroup);

    summaryLayout->addWidget(new QLabel("总整理文件数:", this), 0, 0);
    m_totalFilesLabel = new QLabel("0", this);
    m_totalFilesLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #2196F3;");
    summaryLayout->addWidget(m_totalFilesLabel, 0, 1);

    summaryLayout->addWidget(new QLabel("总节省空间:", this), 1, 0);
    m_totalSpaceLabel = new QLabel("0 B", this);
    m_totalSpaceLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #4CAF50;");
    summaryLayout->addWidget(m_totalSpaceLabel, 1, 1);

    summaryLayout->addWidget(new QLabel("今日整理:", this), 2, 0);
    m_todayFilesLabel = new QLabel("0", this);
    summaryLayout->addWidget(m_todayFilesLabel, 2, 1);

    summaryLayout->addWidget(new QLabel("本周整理:", this), 3, 0);
    m_weekFilesLabel = new QLabel("0", this);
    summaryLayout->addWidget(m_weekFilesLabel, 3, 1);

    summaryLayout->addWidget(new QLabel("本月整理:", this), 4, 0);
    m_monthFilesLabel = new QLabel("0", this);
    summaryLayout->addWidget(m_monthFilesLabel, 4, 1);

    summaryLayout->addWidget(new QLabel("上次整理时间:", this), 5, 0);
    mLastOrganizedLabel = new QLabel("从未", this);
    summaryLayout->addWidget(mLastOrganizedLabel, 5, 1);

    m_mainLayout->addWidget(m_summaryGroup);

    qDebug() << "Created summary group";

    // 按类型统计
    m_byTypeGroup = new QGroupBox("按文件类型统计", this);
    QVBoxLayout *byTypeLayout = new QVBoxLayout(m_byTypeGroup);

    m_byTypeTable = new QTableWidget(this);
    m_byTypeTable->setColumnCount(2);
    m_byTypeTable->setHorizontalHeaderLabels(QStringList() << "文件类型" << "数量");
    m_byTypeTable->horizontalHeader()->setStretchLastSection(true);
    m_byTypeTable->verticalHeader()->setVisible(false);
    m_byTypeTable->setMinimumHeight(150);
    m_byTypeTable->setAlternatingRowColors(true);
    byTypeLayout->addWidget(m_byTypeTable);

    m_mainLayout->addWidget(m_byTypeGroup);

    qDebug() << "Created by type group";

    // 按文件夹统计
    m_byFolderGroup = new QGroupBox("按文件夹统计", this);
    QVBoxLayout *byFolderLayout = new QVBoxLayout(m_byFolderGroup);

    m_byFolderTable = new QTableWidget(this);
    m_byFolderTable->setColumnCount(2);
    m_byFolderTable->setHorizontalHeaderLabels(QStringList() << "文件夹" << "数量");
    m_byFolderTable->horizontalHeader()->setStretchLastSection(true);
    m_byFolderTable->verticalHeader()->setVisible(false);
    m_byFolderTable->setMinimumHeight(150);
    m_byFolderTable->setAlternatingRowColors(true);
    byFolderLayout->addWidget(m_byFolderTable);

    m_mainLayout->addWidget(m_byFolderGroup);

    qDebug() << "Created by folder group";

    // 按钮布局
    m_buttonLayout = new QHBoxLayout();

    m_refreshButton = new QPushButton("刷新", this);
    m_buttonLayout->addWidget(m_refreshButton);

    m_resetButton = new QPushButton("重置统计", this);
    m_buttonLayout->addWidget(m_resetButton);

    m_exportButton = new QPushButton("导出统计", this);
    m_buttonLayout->addWidget(m_exportButton);

    m_buttonLayout->addStretch();

    m_closeButton = new QPushButton("关闭", this);
    m_buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_buttonLayout);

    qDebug() << "Created buttons";

    // 连接信号
    connect(m_refreshButton, &QPushButton::clicked, this, &StatisticsDialog::refreshStatistics);
    connect(m_resetButton, &QPushButton::clicked, this, &StatisticsDialog::resetStatistics);
    connect(m_exportButton, &QPushButton::clicked, this, &StatisticsDialog::exportStatistics);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);

    qDebug() << "StatisticsDialog::setupUI() completed";
}

void StatisticsDialog::refreshStatistics()
{
    updateStatisticsDisplay();
}

void StatisticsDialog::resetStatistics()
{
    if (QMessageBox::question(this, "确认", "确定要重置所有统计数据吗？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_statistics) {
            m_statistics->resetStatistics();
            updateStatisticsDisplay();
        }
    }
}

void StatisticsDialog::exportStatistics()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出统计", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    QTextStream out(&file);
    out << "AutoSort 统计信息\n";
    out << "导出时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "=========================================\n\n";

    if (m_statistics) {
        StatisticsData data = m_statistics->getStatistics();

        out << "摘要信息:\n";
        out << "  总整理文件数: " << data.totalFilesOrganized << "\n";
        out << "  总节省空间: " << formatFileSize(data.totalSpaceSaved) << "\n";
        out << "  今日整理: " << data.filesOrganizedToday << "\n";
        out << "  本周整理: " << data.filesOrganizedThisWeek << "\n";
        out << "  本月整理: " << data.filesOrganizedThisMonth << "\n";
        out << "  上次整理时间: " << (data.lastOrganizedTime.isValid() ?
            data.lastOrganizedTime.toString("yyyy-MM-dd hh:mm:ss") : "从未") << "\n\n";

        out << "按文件类型统计:\n";
        for (auto it = data.filesByType.begin(); it != data.filesByType.end(); ++it) {
            out << "  " << it.key() << ": " << it.value() << "\n";
        }
        out << "\n";

        out << "按文件夹统计:\n";
        for (auto it = data.filesByFolder.begin(); it != data.filesByFolder.end(); ++it) {
            out << "  " << it.key() << ": " << it.value() << "\n";
        }
    }

    file.close();

    QMessageBox::information(this, "成功", "统计导出成功");
}

void StatisticsDialog::updateStatisticsDisplay()
{
    qDebug() << "StatisticsDialog::updateStatisticsDisplay() called";

    if (!m_statistics) {
        qDebug() << "m_statistics is null!";
        QMessageBox::warning(this, "错误", "统计模块未初始化");
        return;
    }

    StatisticsData data = m_statistics->getStatistics();
    qDebug() << "Total files organized:" << data.totalFilesOrganized;
    qDebug() << "Files by type count:" << data.filesByType.size();
    qDebug() << "Files by folder count:" << data.filesByFolder.size();

    // 更新摘要信息
    m_totalFilesLabel->setText(QString::number(data.totalFilesOrganized));
    m_totalSpaceLabel->setText(formatFileSize(data.totalSpaceSaved));
    m_todayFilesLabel->setText(QString::number(data.filesOrganizedToday));
    m_weekFilesLabel->setText(QString::number(data.filesOrganizedThisWeek));
    m_monthFilesLabel->setText(QString::number(data.filesOrganizedThisMonth));
    mLastOrganizedLabel->setText(data.lastOrganizedTime.isValid() ?
        data.lastOrganizedTime.toString("yyyy-MM-dd hh:mm:ss") : "从未");

    // 更新按类型统计表格
    m_byTypeTable->setRowCount(0);
    for (auto it = data.filesByType.begin(); it != data.filesByType.end(); ++it) {
        int row = m_byTypeTable->rowCount();
        m_byTypeTable->insertRow(row);
        m_byTypeTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        m_byTypeTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
    }
    m_byTypeTable->resizeColumnsToContents();

    // 更新按文件夹统计表格
    m_byFolderTable->setRowCount(0);
    for (auto it = data.filesByFolder.begin(); it != data.filesByFolder.end(); ++it) {
        int row = m_byFolderTable->rowCount();
        m_byFolderTable->insertRow(row);
        m_byFolderTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        m_byFolderTable->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
    }
    m_byFolderTable->resizeColumnsToContents();

    qDebug() << "Statistics display updated successfully";
}

QString StatisticsDialog::formatFileSize(qint64 size) const
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