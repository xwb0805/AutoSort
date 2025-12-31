#ifndef FILEORGANIZER_H
#define FILEORGANIZER_H

#include "common.h"
#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
class QTimer;
class Statistics;
QT_END_NAMESPACE

class FileOrganizer : public QObject
{
    Q_OBJECT

public:
    explicit FileOrganizer(QObject *parent = nullptr);
    ~FileOrganizer();

    // 多路径支持
    void addMonitorPath(const QString &path);
    void removeMonitorPath(const QString &path);
    QStringList monitorPaths() const;
    void setMonitorPaths(const QStringList &paths);

    // 冲突处理策略
    void setConflictAction(ConflictAction action);
    ConflictAction conflictAction() const;

    // 正则表达式规则
    void addRegexRule(const QString &pattern, const QString &folder);
    void removeRegexRule(const QString &pattern);
    QMap<QString, QString> regexRules() const;
    void loadRegexRules();
    void saveRegexRules();

    void setDownloadPath(const QString &path);
    QString downloadPath() const;

    int organizeFiles();
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;

    void addCustomRule(const QString &extension, const QString &folder);
    void removeCustomRule(const QString &extension);
    QMap<QString, QString> customRules() const;
    void loadCustomRules();
    void saveCustomRules();
    void loadRulesFromFile(const QString &filePath);

    void initializeDefaultRules();
    QMap<QString, QString> defaultRules() const;

    // 排除扩展名相关方法
    void addExcludedExtension(const QString &extension);
    void removeExcludedExtension(const QString &extension);
    QStringList excludedExtensions() const;
    void loadExcludedExtensions();
    void saveExcludedExtensions();

    // 统计功能
    void setStatistics(Statistics *statistics);

signals:
    void organizationCompleted(int count);
    void monitoringStarted();
    void monitoringStopped();
    void fileConflict(const QString &sourcePath, const QString &targetPath);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);
    void delayedOrganization();

private:
    QString getTargetFolder(const QString &extension) const;
    void createFolderIfNotExists(const QString &folderPath) const;
    bool moveFile(const QString &sourcePath, const QString &targetPath) const;
    bool moveDirectory(const QString &sourcePath, const QString &targetPath) const;
    QString getFileExtension(const QString &fileName) const;
    bool isFileLocked(const QString &filePath) const;
    bool isTemporaryFile(const QString &fileName) const;
    bool isExtensionFolder(const QString &folderName) const;
    bool isExcludedExtension(const QString &extension) const;

    // 正则表达式匹配
    QString matchRegexRule(const QString &fileName) const;

    // 冲突处理
    QString handleConflict(const QString &sourcePath, const QString &targetPath);

    QFileSystemWatcher *m_watcher;
    QTimer *m_organizationTimer;
    QString m_downloadPath;
    bool m_isMonitoring;
    QMap<QString, QString> m_extensionRules;
    QStringList m_temporaryExtensions;
    QStringList m_excludedExtensions;
    QStringList m_monitorPaths;
    ConflictAction m_conflictAction;
    QMap<QString, QRegularExpression> m_regexRules;
    Statistics *m_statistics;  // 统计模块指针
};

#endif // FILEORGANIZER_H