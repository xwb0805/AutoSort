#include "logviewerdialog.h"
#include "logmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QDateTime>

LogViewerDialog::LogViewerDialog(LogManager *logManager, QWidget *parent)
    : QDialog(parent)
    , m_logManager(logManager)
{
    setupUI();
    refreshLogs();
}

LogViewerDialog::~LogViewerDialog()
{
}

void LogViewerDialog::setupUI()
{
    setWindowTitle("日志查看器");
    setMinimumSize(800, 600);

    m_mainLayout = new QVBoxLayout(this);

    // 过滤器布局
    m_filterLayout = new QHBoxLayout();

    m_filterLayout->addWidget(new QLabel("过滤:", this));
    m_filterComboBox = new QComboBox(this);
    m_filterComboBox->addItem("全部日志", static_cast<int>(LogType::Info) - 1);
    m_filterComboBox->addItem("信息", static_cast<int>(LogType::Info));
    m_filterComboBox->addItem("成功", static_cast<int>(LogType::Success));
    m_filterComboBox->addItem("警告", static_cast<int>(LogType::Warning));
    m_filterComboBox->addItem("错误", static_cast<int>(LogType::Error));
    m_filterLayout->addWidget(m_filterComboBox);

    m_autoRefreshCheckBox = new QCheckBox("自动刷新", this);
    m_filterLayout->addWidget(m_autoRefreshCheckBox);

    m_filterLayout->addStretch();

    m_logCountLabel = new QLabel("日志数量: 0", this);
    m_filterLayout->addWidget(m_logCountLabel);

    m_mainLayout->addLayout(m_filterLayout);

    // 日志文本框
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setFont(QFont("Courier New", 9));
    m_mainLayout->addWidget(m_logTextEdit);

    // 按钮布局
    m_buttonLayout = new QHBoxLayout();

    m_refreshButton = new QPushButton("刷新", this);
    m_buttonLayout->addWidget(m_refreshButton);

    m_clearButton = new QPushButton("清空日志", this);
    m_buttonLayout->addWidget(m_clearButton);

    m_exportButton = new QPushButton("导出日志", this);
    m_buttonLayout->addWidget(m_exportButton);

    m_buttonLayout->addStretch();

    m_closeButton = new QPushButton("关闭", this);
    m_buttonLayout->addWidget(m_closeButton);

    m_mainLayout->addLayout(m_buttonLayout);

    // 连接信号
    connect(m_refreshButton, &QPushButton::clicked, this, &LogViewerDialog::refreshLogs);
    connect(m_clearButton, &QPushButton::clicked, this, &LogViewerDialog::clearLogs);
    connect(m_exportButton, &QPushButton::clicked, this, &LogViewerDialog::exportLogs);
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_filterComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LogViewerDialog::filterLogs);

    // 自动刷新
    connect(m_autoRefreshCheckBox, &QCheckBox::toggled, [this](bool checked) {
        if (checked) {
            refreshLogs();
        }
    });

    // 监听日志添加
    if (m_logManager) {
        connect(m_logManager, &LogManager::logAdded, this, [this](const LogEntry &entry) {
            if (m_autoRefreshCheckBox->isChecked()) {
                refreshLogs();
            }
        });
    }
}

void LogViewerDialog::refreshLogs()
{
    updateLogDisplay();
}

void LogViewerDialog::filterLogs()
{
    updateLogDisplay();
}

void LogViewerDialog::clearLogs()
{
    if (QMessageBox::question(this, "确认", "确定要清空所有日志吗？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (m_logManager) {
            m_logManager->clearLogs();
            refreshLogs();
        }
    }
}

void LogViewerDialog::exportLogs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出日志", "", "Text Files (*.txt);;Log Files (*.log)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    QTextStream out(&file);
    out << "AutoSort 日志导出\n";
    out << "导出时间: " << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "=========================================\n\n";

    QList<LogEntry> logs;
    int filterIndex = m_filterComboBox->currentData().toInt();

    if (filterIndex < 0) {
        logs = m_logManager ? m_logManager->getLogs() : QList<LogEntry>();
    } else {
        LogType type = static_cast<LogType>(filterIndex);
        logs = m_logManager ? m_logManager->getLogsByType(type) : QList<LogEntry>();
    }

    for (const LogEntry &entry : logs) {
        out << formatLogEntry(entry) << "\n";
    }

    file.close();

    QMessageBox::information(this, "成功", "日志导出成功");
}

void LogViewerDialog::updateLogDisplay()
{
    if (!m_logManager) {
        return;
    }

    QList<LogEntry> logs;
    int filterIndex = m_filterComboBox->currentData().toInt();

    if (filterIndex < 0) {
        logs = m_logManager->getLogs();
    } else {
        LogType type = static_cast<LogType>(filterIndex);
        logs = m_logManager->getLogsByType(type);
    }

    m_logTextEdit->clear();

    QString htmlContent;
    htmlContent += "<html><body style='font-family: Courier New, monospace; font-size: 9pt;'>";

    for (const LogEntry &entry : logs) {
        QString color = logTypeToColor(entry.type);
        QString typeStr;

        switch (entry.type) {
            case LogType::Info:
                typeStr = "INFO";
                break;
            case LogType::Success:
                typeStr = "SUCCESS";
                break;
            case LogType::Warning:
                typeStr = "WARNING";
                break;
            case LogType::Error:
                typeStr = "ERROR";
                break;
        }

        htmlContent += QString("<div style='color: %1;'>[%2] [%3] %4</div>")
            .arg(color)
            .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
            .arg(typeStr)
            .arg(entry.message.toHtmlEscaped());
    }

    htmlContent += "</body></html>";

    m_logTextEdit->setHtml(htmlContent);

    // 滚动到底部
    QTextCursor cursor = m_logTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_logTextEdit->setTextCursor(cursor);

    m_logCountLabel->setText(QString("日志数量: %1").arg(logs.size()));
}

QString LogViewerDialog::formatLogEntry(const LogEntry &entry) const
{
    QString typeStr;

    switch (entry.type) {
        case LogType::Info:
            typeStr = "INFO";
            break;
        case LogType::Success:
            typeStr = "SUCCESS";
            break;
        case LogType::Warning:
            typeStr = "WARNING";
            break;
        case LogType::Error:
            typeStr = "ERROR";
            break;
    }

    return QString("[%1] [%2] %3")
        .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
        .arg(typeStr)
        .arg(entry.message);
}

QString LogViewerDialog::logTypeToColor(LogType type) const
{
    switch (type) {
        case LogType::Info:
            return "#000000";  // 黑色
        case LogType::Success:
            return "#00AA00";  // 绿色
        case LogType::Warning:
            return "#FF8800";  // 橙色
        case LogType::Error:
            return "#FF0000";  // 红色
        default:
            return "#000000";
    }
}