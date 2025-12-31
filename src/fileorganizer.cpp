#include "fileorganizer.h"
#include "statistics.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QDebug>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QSettings>

FileOrganizer::FileOrganizer(QObject *parent)
    : QObject(parent)
    , m_watcher(nullptr)
    , m_organizationTimer(nullptr)
    , m_isMonitoring(false)
    , m_conflictAction(ConflictAction::Rename)
    , m_statistics(nullptr)
{
    m_watcher = new QFileSystemWatcher(this);
    m_organizationTimer = new QTimer(this);
    m_organizationTimer->setSingleShot(true);
    m_organizationTimer->setInterval(5000);

    // 初始化监控路径列表
    m_monitorPaths.clear();

    connect(m_watcher, &QFileSystemWatcher::directoryChanged, this, &FileOrganizer::onDirectoryChanged);
    connect(m_watcher, &QFileSystemWatcher::fileChanged, this, &FileOrganizer::onFileChanged);
    connect(m_organizationTimer, &QTimer::timeout, this, &FileOrganizer::delayedOrganization);

    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (defaultPath.isEmpty()) {
        defaultPath = QDir::homePath() + "/Downloads";
    }
    m_downloadPath = defaultPath;

    m_temporaryExtensions << "tmp" << "temp" << "crdownload" << "part" << "partial"
                         << "download" << "filepart" << "!qb" << "!ut"
                         << "aria2" << "td" << "pdownload";

    initializeDefaultRules();

    // 加载排除扩展名列表
    loadExcludedExtensions();

    // 优先从项目目录加载 rules.json
    QString projectRulesPath = QCoreApplication::applicationDirPath() + "/rules.json";
    if (QFile::exists(projectRulesPath)) {
        loadRulesFromFile(projectRulesPath);
        qDebug() << "Loaded rules from project directory:" << projectRulesPath;
    } else {
        loadCustomRules();
    }
}

FileOrganizer::~FileOrganizer()
{
    if (m_isMonitoring) {
        stopMonitoring();
    }
}

void FileOrganizer::setDownloadPath(const QString &path)
{
    if (m_downloadPath != path) {
        bool wasMonitoring = m_isMonitoring;
        if (wasMonitoring) {
            stopMonitoring();
        }

        m_downloadPath = path;

        if (wasMonitoring) {
            startMonitoring();
        }
    }
}

QString FileOrganizer::downloadPath() const
{
    return m_downloadPath;
}

void FileOrganizer::setConflictAction(ConflictAction action)
{
    m_conflictAction = action;
}

ConflictAction FileOrganizer::conflictAction() const
{
    return m_conflictAction;
}

void FileOrganizer::initializeDefaultRules()
{
    m_extensionRules.clear();

    m_extensionRules["pdf"] = "PDF";
    m_extensionRules["doc"] = "DOC";
    m_extensionRules["docx"] = "DOCX";
    m_extensionRules["txt"] = "TXT";
    m_extensionRules["rtf"] = "RTF";
    m_extensionRules["odt"] = "ODT";

    m_extensionRules["xls"] = "XLS";
    m_extensionRules["xlsx"] = "XLSX";
    m_extensionRules["csv"] = "CSV";

    m_extensionRules["ppt"] = "PPT";
    m_extensionRules["pptx"] = "PPTX";

    m_extensionRules["jpg"] = "JPG";
    m_extensionRules["jpeg"] = "JPEG";
    m_extensionRules["png"] = "PNG";
    m_extensionRules["gif"] = "GIF";
    m_extensionRules["bmp"] = "BMP";
    m_extensionRules["tiff"] = "TIFF";
    m_extensionRules["svg"] = "SVG";
    m_extensionRules["webp"] = "WEBP";
    m_extensionRules["ico"] = "ICO";

    m_extensionRules["mp4"] = "MP4";
    m_extensionRules["avi"] = "AVI";
    m_extensionRules["mkv"] = "MKV";
    m_extensionRules["mov"] = "MOV";
    m_extensionRules["wmv"] = "WMV";
    m_extensionRules["flv"] = "FLV";
    m_extensionRules["webm"] = "WEBM";
    m_extensionRules["m4v"] = "M4V";
    m_extensionRules["3gp"] = "3GP";

    m_extensionRules["mp3"] = "MP3";
    m_extensionRules["wav"] = "WAV";
    m_extensionRules["flac"] = "FLAC";
    m_extensionRules["aac"] = "AAC";
    m_extensionRules["ogg"] = "OGG";
    m_extensionRules["wma"] = "WMA";
    m_extensionRules["m4a"] = "M4A";

    m_extensionRules["zip"] = "ZIP";
    m_extensionRules["rar"] = "RAR";
    m_extensionRules["7z"] = "7Z";
    m_extensionRules["tar"] = "TAR";
    m_extensionRules["gz"] = "GZ";
    m_extensionRules["bz2"] = "BZ2";
    m_extensionRules["xz"] = "XZ";

    m_extensionRules["exe"] = "EXE";
    m_extensionRules["msi"] = "MSI";
    m_extensionRules["deb"] = "DEB";
    m_extensionRules["rpm"] = "RPM";
    m_extensionRules["dmg"] = "DMG";
    m_extensionRules["pkg"] = "PKG";
    m_extensionRules["appimage"] = "AppImage";

    m_extensionRules["cpp"] = "CPP";
    m_extensionRules["c"] = "C";
    m_extensionRules["h"] = "H";
    m_extensionRules["hpp"] = "HPP";
    m_extensionRules["java"] = "JAVA";
    m_extensionRules["py"] = "PY";
    m_extensionRules["js"] = "JS";
    m_extensionRules["html"] = "HTML";
    m_extensionRules["css"] = "CSS";
    m_extensionRules["php"] = "PHP";
    m_extensionRules["rb"] = "RB";
    m_extensionRules["go"] = "GO";
    m_extensionRules["rs"] = "RS";
    m_extensionRules["swift"] = "SWIFT";
    m_extensionRules["kt"] = "KT";
    m_extensionRules["ts"] = "TS";
}

QMap<QString, QString> FileOrganizer::defaultRules() const
{
    QMap<QString, QString> rules;

    rules["pdf"] = "PDF";
    rules["doc"] = "DOC";
    rules["docx"] = "DOCX";
    rules["txt"] = "TXT";
    rules["rtf"] = "RTF";
    rules["odt"] = "ODT";

    rules["xls"] = "XLS";
    rules["xlsx"] = "XLSX";
    rules["csv"] = "CSV";

    rules["ppt"] = "PPT";
    rules["pptx"] = "PPTX";

    rules["jpg"] = "JPG";
    rules["jpeg"] = "JPEG";
    rules["png"] = "PNG";
    rules["gif"] = "GIF";
    rules["bmp"] = "BMP";
    rules["tiff"] = "TIFF";
    rules["svg"] = "SVG";
    rules["webp"] = "WEBP";
    rules["ico"] = "ICO";

    rules["mp4"] = "MP4";
    rules["avi"] = "AVI";
    rules["mkv"] = "MKV";
    rules["mov"] = "MOV";
    rules["wmv"] = "WMV";
    rules["flv"] = "FLV";
    rules["webm"] = "WEBM";
    rules["m4v"] = "M4V";
    rules["3gp"] = "3GP";

    rules["mp3"] = "MP3";
    rules["wav"] = "WAV";
    rules["flac"] = "FLAC";
    rules["aac"] = "AAC";
    rules["ogg"] = "OGG";
    rules["wma"] = "WMA";
    rules["m4a"] = "M4A";

    rules["zip"] = "ZIP";
    rules["rar"] = "RAR";
    rules["7z"] = "7Z";
    rules["tar"] = "TAR";
    rules["gz"] = "GZ";
    rules["bz2"] = "BZ2";
    rules["xz"] = "XZ";

    rules["exe"] = "EXE";
    rules["msi"] = "MSI";
    rules["deb"] = "DEB";
    rules["rpm"] = "RPM";
    rules["dmg"] = "DMG";
    rules["pkg"] = "PKG";
    rules["appimage"] = "AppImage";

    rules["cpp"] = "CPP";
    rules["c"] = "C";
    rules["h"] = "H";
    rules["hpp"] = "HPP";
    rules["java"] = "JAVA";
    rules["py"] = "PY";
    rules["js"] = "JS";
    rules["html"] = "HTML";
    rules["css"] = "CSS";
    rules["php"] = "PHP";
    rules["rb"] = "RB";
    rules["go"] = "GO";
    rules["rs"] = "RS";
    rules["swift"] = "SWIFT";
    rules["kt"] = "KT";
    rules["ts"] = "TS";

    return rules;
}

void FileOrganizer::addCustomRule(const QString &extension, const QString &folder)
{
    QString ext = extension.toLower().trimmed();
    if (ext.startsWith(".")) {
        ext = ext.mid(1);
    }
    m_extensionRules[ext] = folder;
}

void FileOrganizer::removeCustomRule(const QString &extension)
{
    QString ext = extension.toLower().trimmed();
    if (ext.startsWith(".")) {
        ext = ext.mid(1);
    }
    m_extensionRules.remove(ext);
}

QMap<QString, QString> FileOrganizer::customRules() const
{
    return m_extensionRules;
}

void FileOrganizer::loadCustomRules()
{
    QString rulesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(rulesPath);
    rulesPath += "/rules.json";

    QFile file(rulesPath);
    if (!file.exists()) {
        initializeDefaultRules();
        saveCustomRules();
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open rules file:" << rulesPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid rules file format";
        return;
    }

    QJsonObject obj = doc.object();
    QJsonObject rulesObj = obj.value("rules").toObject();

    m_extensionRules.clear();

    // 检查规则格式：支持两种格式
    // 格式1: "扩展名": "文件夹名"
    // 格式2: "文件夹名": ["扩展名1", "扩展名2", ...]

    bool isArrayFormat = false;
    for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
        if (it.value().isArray()) {
            isArrayFormat = true;
            break;
        }
    }

    if (isArrayFormat) {
        // 格式2: "文件夹名": ["扩展名1", "扩展名2", ...]
        for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
            QString folderName = it.key();
            QJsonArray extArray = it.value().toArray();

            for (const QJsonValue &extValue : extArray) {
                QString extension = extValue.toString().toLower().trimmed();
                if (!extension.isEmpty()) {
                    m_extensionRules[extension] = folderName;
                }
            }
        }
    } else {
        // 格式1: "扩展名": "文件夹名"
        for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
            QString extension = it.key().toLower().trimmed();
            QString folder = it.value().toString().trimmed();
            if (!extension.isEmpty() && !folder.isEmpty()) {
                m_extensionRules[extension] = folder;
            }
        }
    }

    qDebug() << "Loaded" << m_extensionRules.size() << "file rules";
}

void FileOrganizer::saveCustomRules()
{
    QString rulesPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(rulesPath);
    rulesPath += "/rules.json";

    QJsonObject rulesObj;
    for (auto it = m_extensionRules.begin(); it != m_extensionRules.end(); ++it) {
        rulesObj[it.key()] = it.value();
    }

    QJsonObject mainObj;
    mainObj["rules"] = rulesObj;

    QJsonDocument doc(mainObj);

    QFile file(rulesPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write rules file:" << rulesPath;
        return;
    }

    file.write(doc.toJson());
    file.close();
}

void FileOrganizer::loadRulesFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        qWarning() << "Rules file does not exist:" << filePath;
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open rules file:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid rules file format:" << filePath;
        return;
    }

    QJsonObject obj = doc.object();
    QJsonObject rulesObj = obj.value("rules").toObject();

    m_extensionRules.clear();

    // 检查规则格式：支持两种格式
    // 格式1: "扩展名": "文件夹名"
    // 格式2: "文件夹名": ["扩展名1", "扩展名2", ...]

    bool isArrayFormat = false;
    for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
        if (it.value().isArray()) {
            isArrayFormat = true;
            break;
        }
    }

    if (isArrayFormat) {
        // 格式2: "文件夹名": ["扩展名1", "扩展名2", ...]
        for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
            QString folderName = it.key();
            QJsonArray extArray = it.value().toArray();

            for (const QJsonValue &extValue : extArray) {
                QString extension = extValue.toString().toLower().trimmed();
                if (!extension.isEmpty()) {
                    m_extensionRules[extension] = folderName;
                }
            }
        }
    } else {
        // 格式1: "扩展名": "文件夹名"
        for (auto it = rulesObj.begin(); it != rulesObj.end(); ++it) {
            QString extension = it.key().toLower().trimmed();
            QString folder = it.value().toString().trimmed();
            if (!extension.isEmpty() && !folder.isEmpty()) {
                m_extensionRules[extension] = folder;
            }
        }
    }

    qDebug() << "Loaded" << m_extensionRules.size() << "rules from" << filePath;
}

void FileOrganizer::startMonitoring()
{
    if (m_isMonitoring) {
        return;
    }

    QStringList pathsToMonitor = m_monitorPaths.isEmpty() ? QStringList() << m_downloadPath : m_monitorPaths;

    for (const QString &path : pathsToMonitor) {
        if (!m_watcher->addPath(path)) {
            qWarning() << "Failed to watch directory:" << path;
        } else {
            qDebug() << "Watching directory:" << path;
        }
    }

    if (!pathsToMonitor.isEmpty()) {
        m_isMonitoring = true;
        emit monitoringStarted();
        qDebug() << "Started monitoring" << pathsToMonitor.size() << "directories";
    }
}

void FileOrganizer::stopMonitoring()
{
    if (!m_isMonitoring) {
        return;
    }

    QStringList pathsToMonitor = m_monitorPaths.isEmpty() ? QStringList() << m_downloadPath : m_monitorPaths;

    for (const QString &path : pathsToMonitor) {
        m_watcher->removePath(path);
    }

    m_isMonitoring = false;
    emit monitoringStopped();
    qDebug() << "Stopped monitoring";
}

bool FileOrganizer::isMonitoring() const
{
    return m_isMonitoring;
}

void FileOrganizer::onDirectoryChanged(const QString &path)
{
    Q_UNUSED(path);
    m_organizationTimer->start();
}

void FileOrganizer::onFileChanged(const QString &path)
{
    Q_UNUSED(path);
    m_organizationTimer->start();
}

void FileOrganizer::delayedOrganization()
{
    organizeFiles();
}

int FileOrganizer::organizeFiles()
{
    qDebug() << "=== Starting file organization ===";
    qDebug() << "Download path:" << m_downloadPath;
    qDebug() << "Monitor paths:" << m_monitorPaths;
    qDebug() << "Conflict action:" << static_cast<int>(m_conflictAction);

    QStringList pathsToMonitor = m_monitorPaths.isEmpty() ? QStringList() << m_downloadPath : m_monitorPaths;
    int organizedCount = 0;

    for (const QString &downloadPath : pathsToMonitor) {
        qDebug() << "Processing path:" << downloadPath;
        if (downloadPath.isEmpty()) {
            continue;
        }

        QDir downloadDir(downloadPath);
        if (!downloadDir.exists()) {
            qWarning() << "Download directory does not exist:" << downloadPath;
            continue;
        }

        // 处理文件
        QStringList files = downloadDir.entryList(QDir::Files | QDir::NoDotAndDotDot);

        for (const QString &fileName : files) {
            QString filePath = downloadPath + "/" + fileName;

            if (isTemporaryFile(fileName)) {
                qDebug() << "Skipping temporary file:" << fileName;
                continue;
            }

            if (isFileLocked(filePath)) {
                qDebug() << "File is locked (probably downloading):" << fileName;
                continue;
            }

            QString extension = getFileExtension(fileName);

            // 检查是否在排除列表中
            if (!extension.isEmpty() && isExcludedExtension(extension)) {
                qDebug() << "Skipping excluded extension:" << fileName;
                continue;
            }

            QString targetFolder;

            // 先尝试正则表达式匹配
            QString regexFolder = matchRegexRule(fileName);
            if (!regexFolder.isEmpty()) {
                targetFolder = regexFolder;
                qDebug() << "Matched regex rule for" << fileName << "->" << targetFolder;
            } else if (extension.isEmpty()) {
                qDebug() << "File has no extension, moving to Other folder:" << fileName;
                targetFolder = "Other";
            } else {
                targetFolder = getTargetFolder(extension);
                if (targetFolder.isEmpty()) {
                    qDebug() << "No rule for extension" << extension << "using Other folder";
                    targetFolder = "Other";
                }
            }

            QString targetDirPath = downloadPath + "/" + targetFolder;
            createFolderIfNotExists(targetDirPath);

            QString targetFilePath = targetDirPath + "/" + fileName;

            // 处理文件冲突
            if (QFile::exists(targetFilePath)) {
                QString finalTargetPath = handleConflict(filePath, targetFilePath);
                if (finalTargetPath.isEmpty()) {
                    qDebug() << "Skipped file due to conflict:" << fileName;
                    continue;
                }
                targetFilePath = finalTargetPath;
            }

            if (moveFile(filePath, targetFilePath)) {
                organizedCount++;
                qDebug() << "Moved" << fileName << "to" << targetFolder;

                // 记录统计信息
                if (m_statistics) {
                    QFileInfo fileInfo(targetFilePath);
                    m_statistics->recordFileOrganized(extension, targetFolder, fileInfo.size());
                }
            } else {
                qWarning() << "Failed to move" << fileName << "to" << targetFolder;
            }
        }

        // 处理文件夹：将非扩展名文件夹移动到 Folder 文件夹
        QStringList folders = downloadDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString &folderName : folders) {
            QString folderPath = downloadPath + "/" + folderName;

            // 跳过扩展名文件夹和特殊文件夹
            if (isExtensionFolder(folderName) || folderName == "Folder" || folderName == "Other") {
                qDebug() << "Skipping extension or special folder:" << folderName;
                continue;
            }

            QString targetDirPath = downloadPath + "/Folder";
            createFolderIfNotExists(targetDirPath);

            QString targetFolderPath = targetDirPath + "/" + folderName;
            if (QDir(targetFolderPath).exists()) {
                int counter = 1;
                do {
                    targetFolderPath = targetDirPath + "/" + folderName + "_" + QString::number(counter);
                    counter++;
                } while (QDir(targetFolderPath).exists());
            }

            if (moveDirectory(folderPath, targetFolderPath)) {
                organizedCount++;
                qDebug() << "Moved folder" << folderName << "to Folder";
            } else {
                qWarning() << "Failed to move folder" << folderName;
            }
        }
    }

    if (organizedCount > 0) {
        emit organizationCompleted(organizedCount);
    }

    return organizedCount;
}

QString FileOrganizer::getTargetFolder(const QString &extension) const
{
    QString ext = extension.toLower();
    return m_extensionRules.value(ext, QString());
}

void FileOrganizer::createFolderIfNotExists(const QString &folderPath) const
{
    QDir dir;
    if (!dir.exists(folderPath)) {
        dir.mkpath(folderPath);
    }
}

bool FileOrganizer::moveFile(const QString &sourcePath, const QString &targetPath) const
{
    return QFile::rename(sourcePath, targetPath);
}

QString FileOrganizer::getFileExtension(const QString &fileName) const
{
    int dotIndex = fileName.lastIndexOf('.');
    if (dotIndex > 0 && dotIndex < fileName.length() - 1) {
        return fileName.mid(dotIndex + 1).toLower();
    }
    return QString();
}

bool FileOrganizer::isFileLocked(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.exists()) {
        return false;
    }

    return !file.open(QIODevice::ReadOnly);
}

bool FileOrganizer::isTemporaryFile(const QString &fileName) const
{
    if (fileName.startsWith('.') || fileName.startsWith('~')) {
        return true;
    }

    QString extension = getFileExtension(fileName);
    return m_temporaryExtensions.contains(extension);
}

bool FileOrganizer::moveDirectory(const QString &sourcePath, const QString &targetPath) const
{
    QDir sourceDir(sourcePath);
    QDir targetDir;

    if (!targetDir.mkpath(targetPath)) {
        qWarning() << "Failed to create target directory:" << targetPath;
        return false;
    }

    // 移动文件夹中的所有文件和子文件夹
    QFileInfoList fileList = sourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo &fileInfo : fileList) {
        QString sourceItemPath = fileInfo.filePath();
        QString targetItemPath = targetPath + "/" + fileInfo.fileName();

        if (fileInfo.isDir()) {
            if (!moveDirectory(sourceItemPath, targetItemPath)) {
                return false;
            }
        } else {
            if (!QFile::rename(sourceItemPath, targetItemPath)) {
                qWarning() << "Failed to move file:" << sourceItemPath;
                return false;
            }
        }
    }

    // 删除空的原文件夹
    if (!sourceDir.rmdir(sourcePath)) {
        qWarning() << "Failed to remove source directory:" << sourcePath;
    }

    return true;
}

bool FileOrganizer::isExtensionFolder(const QString &folderName) const
{
    // 检查文件夹名是否在扩展名规则中
    for (const QString &folder : m_extensionRules) {
        if (folder == folderName) {
            return true;
        }
    }
    return false;
}

// 排除扩展名相关方法实现

void FileOrganizer::addExcludedExtension(const QString &extension)
{
    QString ext = extension.toLower().trimmed();
    if (ext.startsWith(".")) {
        ext = ext.mid(1);
    }
    if (!ext.isEmpty() && !m_excludedExtensions.contains(ext)) {
        m_excludedExtensions.append(ext);
        m_excludedExtensions.sort();
        saveExcludedExtensions();
    }
}

void FileOrganizer::removeExcludedExtension(const QString &extension)
{
    QString ext = extension.toLower().trimmed();
    if (ext.startsWith(".")) {
        ext = ext.mid(1);
    }
    m_excludedExtensions.removeAll(ext);
    m_excludedExtensions.sort();
    saveExcludedExtensions();
}

QStringList FileOrganizer::excludedExtensions() const
{
    return m_excludedExtensions;
}

void FileOrganizer::loadExcludedExtensions()
{
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(settingsPath);
    settingsPath += "/config.json";

    QFile file(settingsPath);
    if (!file.exists()) {
        return; // 如果文件不存在，使用空列表
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open config file:" << settingsPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid config file format";
        return;
    }

    QJsonObject obj = doc.object();
    QJsonArray excludedArray = obj.value("excluded_extensions").toArray();

    m_excludedExtensions.clear();
    for (const QJsonValue &value : excludedArray) {
        QString ext = value.toString().toLower().trimmed();
        if (!ext.isEmpty()) {
            m_excludedExtensions.append(ext);
        }
    }

    m_excludedExtensions.sort();
    qDebug() << "Loaded" << m_excludedExtensions.size() << "excluded extensions";
}

void FileOrganizer::saveExcludedExtensions()
{
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(settingsPath);
    settingsPath += "/config.json";

    QJsonObject mainObj;

    // 保存排除扩展名列表
    QJsonArray excludedArray;
    for (const QString &ext : m_excludedExtensions) {
        excludedArray.append(ext);
    }
    mainObj["excluded_extensions"] = excludedArray;

    QJsonDocument doc(mainObj);

    QFile file(settingsPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write config file:" << settingsPath;
        return;
    }

    file.write(doc.toJson());
    file.close();
}

bool FileOrganizer::isExcludedExtension(const QString &extension) const
{
    QString ext = extension.toLower();
    return m_excludedExtensions.contains(ext);
}

// 正则表达式匹配
QString FileOrganizer::matchRegexRule(const QString &fileName) const
{
    for (auto it = m_regexRules.begin(); it != m_regexRules.end(); ++it) {
        const QRegularExpression &regex = it.value();
        QRegularExpressionMatch match = regex.match(fileName);
        if (match.hasMatch()) {
            // 如果正则表达式有捕获组，使用第一个捕获组作为文件夹名
            if (match.capturedTexts().size() > 1) {
                return match.captured(1);
            }
            // 否则返回空字符串，让调用者使用默认文件夹
            return "";
        }
    }
    return "";
}

// 冲突处理
QString FileOrganizer::handleConflict(const QString &sourcePath, const QString &targetPath)
{
    switch (m_conflictAction) {
        case ConflictAction::Overwrite:
        case ConflictAction::OverwriteAll:
            return targetPath;

        case ConflictAction::Skip:
        case ConflictAction::SkipAll:
            return QString();

        case ConflictAction::Rename: {
            QFileInfo fileInfo(sourcePath);
            QString baseName = fileInfo.baseName();
            QString suffix = fileInfo.suffix();
            QString targetDir = QFileInfo(targetPath).absolutePath();

            int counter = 1;
            QString newPath;
            do {
                newPath = targetDir + "/" + baseName + "_" + QString::number(counter);
                if (!suffix.isEmpty()) {
                    newPath += "." + suffix;
                }
                counter++;
            } while (QFile::exists(newPath));

            return newPath;
        }

        case ConflictAction::Ask:
            emit fileConflict(sourcePath, targetPath);
            return QString();  // 等待用户响应

        default:
            return QString();
    }
}

// 多路径监控功能实现
void FileOrganizer::addMonitorPath(const QString &path)
{
    if (!m_monitorPaths.contains(path)) {
        m_monitorPaths.append(path);
        m_watcher->addPath(path);
    }
}

void FileOrganizer::removeMonitorPath(const QString &path)
{
    if (m_monitorPaths.contains(path)) {
        m_monitorPaths.removeAll(path);
        m_watcher->removePath(path);
    }
}

QStringList FileOrganizer::monitorPaths() const
{
    return m_monitorPaths;
}

void FileOrganizer::setMonitorPaths(const QStringList &paths)
{
    // 移除旧路径
    for (const QString &path : m_monitorPaths) {
        m_watcher->removePath(path);
    }
    // 添加新路径
    m_monitorPaths = paths;
    for (const QString &path : m_monitorPaths) {
        m_watcher->addPath(path);
    }
}

// 正则表达式规则功能实现
void FileOrganizer::addRegexRule(const QString &pattern, const QString &folder)
{
    QRegularExpression regex(pattern);
    if (regex.isValid()) {
        m_regexRules[pattern] = regex;
        // 保存到配置
        saveRegexRules();
    }
}

void FileOrganizer::removeRegexRule(const QString &pattern)
{
    m_regexRules.remove(pattern);
    saveRegexRules();
}

QMap<QString, QString> FileOrganizer::regexRules() const
{
    QMap<QString, QString> result;
    for (auto it = m_regexRules.constBegin(); it != m_regexRules.constEnd(); ++it) {
        result[it.key()] = "";  // 返回模式，文件夹需要单独获取
    }
    return result;
}

void FileOrganizer::loadRegexRules()
{
    QSettings settings;
    m_regexRules.clear();

    int count = settings.beginReadArray("regexRules");
    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        QString pattern = settings.value("pattern").toString();
        QString folder = settings.value("folder").toString();
        if (!pattern.isEmpty()) {
            QRegularExpression regex(pattern);
            if (regex.isValid()) {
                m_regexRules[pattern] = regex;
            }
        }
    }
    settings.endArray();
}

void FileOrganizer::saveRegexRules()
{
    QSettings settings;
    settings.beginWriteArray("regexRules");

    int index = 0;
    for (auto it = m_regexRules.constBegin(); it != m_regexRules.constEnd(); ++it) {
        settings.setArrayIndex(index++);
        settings.setValue("pattern", it.key());
        // 文件夹信息需要从其他地方获取，这里暂时只保存模式
    }

    settings.endArray();
}

void FileOrganizer::setStatistics(Statistics *statistics)
{
    m_statistics = statistics;
}