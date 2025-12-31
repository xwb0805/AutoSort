#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "common.h"
#include <QObject>
#include <QList>

class LogManager : public QObject
{
    Q_OBJECT

public:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();

    void addLog(LogType type, const QString &message);
    void addInfo(const QString &message);
    void addSuccess(const QString &message);
    void addWarning(const QString &message);
    void addError(const QString &message);

    QList<LogEntry> getLogs() const;
    QList<LogEntry> getLogsByType(LogType type) const;
    QList<LogEntry> getLogsByDateRange(const QDateTime &start, const QDateTime &end) const;
    void clearLogs();
    void loadLogs();
    void saveLogs();

signals:
    void logAdded(const LogEntry &entry);
    void logsCleared();

private:
    QString logTypeToString(LogType type) const;
    QString formatLogEntry(const LogEntry &entry) const;

    QList<LogEntry> m_logs;
    int m_maxLogEntries;
};

#endif // LOGMANAGER_H