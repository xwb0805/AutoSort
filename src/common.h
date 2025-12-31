#ifndef COMMON_H
#define COMMON_H

#include <QtGlobal>
#include <QDateTime>
#include <QString>

// 冲突处理策略
enum class ConflictAction {
    Overwrite,      // 覆盖当前文件
    Skip,           // 跳过当前文件
    Rename,         // 重命名当前文件
    OverwriteAll,   // 覆盖所有冲突文件
    SkipAll,        // 跳过所有冲突文件
    Ask,            // 询问用户
    Cancel          // 取消
};

// 日志类型
enum class LogType {
    Info,
    Success,
    Warning,
    Error
};

// 日志条目
struct LogEntry {
    QDateTime timestamp;
    LogType type;
    QString message;

    LogEntry() : type(LogType::Info) {}
    LogEntry(LogType t, const QString &msg)
        : timestamp(QDateTime::currentDateTime())
        , type(t)
        , message(msg)
    {}
};

#endif // COMMON_H