#ifndef FILEORGANIZER_H
#define FILEORGANIZER_H

#include <QObject>
#include <QMap>
#include <QString>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QFileSystemWatcher;
class QTimer;
QT_END_NAMESPACE

class FileOrganizer : public QObject
{
    Q_OBJECT

public:
    explicit FileOrganizer(QObject *parent = nullptr);
    ~FileOrganizer();

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

signals:
    void organizationCompleted(int count);
    void monitoringStarted();
    void monitoringStopped();

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

    QFileSystemWatcher *m_watcher;
    QTimer *m_organizationTimer;
    QString m_downloadPath;
    bool m_isMonitoring;
    QMap<QString, QString> m_extensionRules;
    QStringList m_temporaryExtensions;
};

#endif // FILEORGANIZER_H