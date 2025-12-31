#include "logmanager.h"
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    , m_maxLogEntries(1000)
{
    loadLogs();
}

LogManager::~LogManager()
{
    saveLogs();
}

void LogManager::addLog(LogType type, const QString &message)
{
    LogEntry entry(type, message);
    m_logs.append(entry);

    // 限制日志条目数量
    while (m_logs.size() > m_maxLogEntries) {
        m_logs.removeFirst();
    }

    emit logAdded(entry);
}

void LogManager::addInfo(const QString &message)
{
    addLog(LogType::Info, message);
}

void LogManager::addSuccess(const QString &message)
{
    addLog(LogType::Success, message);
}

void LogManager::addWarning(const QString &message)
{
    addLog(LogType::Warning, message);
}

void LogManager::addError(const QString &message)
{
    addLog(LogType::Error, message);
}

QList<LogEntry> LogManager::getLogs() const
{
    return m_logs;
}

QList<LogEntry> LogManager::getLogsByType(LogType type) const
{
    QList<LogEntry> filteredLogs;
    for (const LogEntry &entry : m_logs) {
        if (entry.type == type) {
            filteredLogs.append(entry);
        }
    }
    return filteredLogs;
}

QList<LogEntry> LogManager::getLogsByDateRange(const QDateTime &start, const QDateTime &end) const
{
    QList<LogEntry> filteredLogs;
    for (const LogEntry &entry : m_logs) {
        if (entry.timestamp >= start && entry.timestamp <= end) {
            filteredLogs.append(entry);
        }
    }
    return filteredLogs;
}

void LogManager::clearLogs()
{
    m_logs.clear();
    emit logsCleared();
    saveLogs();
}

void LogManager::loadLogs()
{
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logPath);
    logPath += "/logs.json";

    QFile file(logPath);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open log file:" << logPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isArray()) {
        qWarning() << "Invalid log file format";
        return;
    }

    QJsonArray logArray = doc.array();
    m_logs.clear();

    for (const QJsonValue &value : logArray) {
        QJsonObject logObj = value.toObject();

        LogEntry entry;
        entry.timestamp = QDateTime::fromString(logObj.value("timestamp").toString(), Qt::ISODate);

        QString typeStr = logObj.value("type").toString();
        if (typeStr == "Info") {
            entry.type = LogType::Info;
        } else if (typeStr == "Success") {
            entry.type = LogType::Success;
        } else if (typeStr == "Warning") {
            entry.type = LogType::Warning;
        } else if (typeStr == "Error") {
            entry.type = LogType::Error;
        }

        entry.message = logObj.value("message").toString();

        m_logs.append(entry);
    }

    qDebug() << "Loaded" << m_logs.size() << "log entries";
}

void LogManager::saveLogs()
{
    QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logPath);
    logPath += "/logs.json";

    QJsonArray logArray;

    for (const LogEntry &entry : m_logs) {
        QJsonObject logObj;
        logObj["timestamp"] = entry.timestamp.toString(Qt::ISODate);
        logObj["type"] = logTypeToString(entry.type);
        logObj["message"] = entry.message;

        logArray.append(logObj);
    }

    QJsonDocument doc(logArray);

    QFile file(logPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write log file:" << logPath;
        return;
    }

    file.write(doc.toJson());
    file.close();
}

QString LogManager::logTypeToString(LogType type) const
{
    switch (type) {
        case LogType::Info:
            return "Info";
        case LogType::Success:
            return "Success";
        case LogType::Warning:
            return "Warning";
        case LogType::Error:
            return "Error";
        default:
            return "Unknown";
    }
}

QString LogManager::formatLogEntry(const LogEntry &entry) const
{
    return QString("[%1] [%2] %3")
        .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"))
        .arg(logTypeToString(entry.type))
        .arg(entry.message);
}