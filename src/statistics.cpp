#include "statistics.h"
#include <QSettings>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QDebug>

Statistics::Statistics(QObject *parent)
    : QObject(parent)
{
    loadStatistics();
}

Statistics::~Statistics()
{
    saveStatistics();
}

void Statistics::recordFileOrganized(const QString &extension, const QString &folder, qint64 fileSize)
{
    m_data.totalFilesOrganized++;
    m_data.totalSpaceSaved += fileSize;
    m_data.lastOrganizedTime = QDateTime::currentDateTime();

    // 更新按类型统计
    m_data.filesByType[extension.toUpper()]++;

    // 更新按文件夹统计
    m_data.filesByFolder[folder]++;

    updatePeriodicCounts();
    saveStatistics();

    emit statisticsUpdated();
}

StatisticsData Statistics::getStatistics() const
{
    return m_data;
}

void Statistics::resetStatistics()
{
    m_data = StatisticsData();
    saveStatistics();
    emit statisticsUpdated();
}

void Statistics::loadStatistics()
{
    QString statsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(statsPath);
    statsPath += "/statistics.json";

    QFile file(statsPath);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open statistics file:" << statsPath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid statistics file format";
        return;
    }

    QJsonObject obj = doc.object();

    m_data.totalFilesOrganized = obj.value("totalFilesOrganized").toInt(0);
    m_data.totalSpaceSaved = obj.value("totalSpaceSaved").toVariant().toLongLong();
    m_data.filesOrganizedToday = obj.value("filesOrganizedToday").toInt(0);
    m_data.filesOrganizedThisWeek = obj.value("filesOrganizedThisWeek").toInt(0);
    m_data.filesOrganizedThisMonth = obj.value("filesOrganizedThisMonth").toInt(0);

    QString lastTimeStr = obj.value("lastOrganizedTime").toString();
    if (!lastTimeStr.isEmpty()) {
        m_data.lastOrganizedTime = QDateTime::fromString(lastTimeStr, Qt::ISODate);
    }

    // 加载按类型统计
    QJsonObject byTypeObj = obj.value("filesByType").toObject();
    for (auto it = byTypeObj.begin(); it != byTypeObj.end(); ++it) {
        m_data.filesByType[it.key()] = it.value().toInt();
    }

    // 加载按文件夹统计
    QJsonObject byFolderObj = obj.value("filesByFolder").toObject();
    for (auto it = byFolderObj.begin(); it != byFolderObj.end(); ++it) {
        m_data.filesByFolder[it.key()] = it.value().toInt();
    }

    updatePeriodicCounts();

    qDebug() << "Loaded statistics:" << m_data.totalFilesOrganized << "files organized";
}

void Statistics::saveStatistics()
{
    QString statsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(statsPath);
    statsPath += "/statistics.json";

    QJsonObject obj;

    obj["totalFilesOrganized"] = m_data.totalFilesOrganized;
    obj["totalSpaceSaved"] = QJsonValue::fromVariant(m_data.totalSpaceSaved);
    obj["filesOrganizedToday"] = m_data.filesOrganizedToday;
    obj["filesOrganizedThisWeek"] = m_data.filesOrganizedThisWeek;
    obj["filesOrganizedThisMonth"] = m_data.filesOrganizedThisMonth;

    if (m_data.lastOrganizedTime.isValid()) {
        obj["lastOrganizedTime"] = m_data.lastOrganizedTime.toString(Qt::ISODate);
    }

    // 保存按类型统计
    QJsonObject byTypeObj;
    for (auto it = m_data.filesByType.begin(); it != m_data.filesByType.end(); ++it) {
        byTypeObj[it.key()] = it.value();
    }
    obj["filesByType"] = byTypeObj;

    // 保存按文件夹统计
    QJsonObject byFolderObj;
    for (auto it = m_data.filesByFolder.begin(); it != m_data.filesByFolder.end(); ++it) {
        byFolderObj[it.key()] = it.value();
    }
    obj["filesByFolder"] = byFolderObj;

    QJsonDocument doc(obj);

    QFile file(statsPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write statistics file:" << statsPath;
        return;
    }

    file.write(doc.toJson());
    file.close();
}

void Statistics::updatePeriodicCounts()
{
    if (!m_data.lastOrganizedTime.isValid()) {
        return;
    }

    // 如果上次整理不是今天，重置今日计数
    if (!isToday(m_data.lastOrganizedTime)) {
        m_data.filesOrganizedToday = 0;
    }

    // 如果上次整理不是本周，重置本周计数
    if (!isThisWeek(m_data.lastOrganizedTime)) {
        m_data.filesOrganizedThisWeek = 0;
    }

    // 如果上次整理不是本月，重置本月计数
    if (!isThisMonth(m_data.lastOrganizedTime)) {
        m_data.filesOrganizedThisMonth = 0;
    }

    // 增加当前时间段的计数
    QDateTime now = QDateTime::currentDateTime();
    if (isToday(now)) {
        m_data.filesOrganizedToday++;
    }
    if (isThisWeek(now)) {
        m_data.filesOrganizedThisWeek++;
    }
    if (isThisMonth(now)) {
        m_data.filesOrganizedThisMonth++;
    }
}

bool Statistics::isToday(const QDateTime &dateTime) const
{
    return dateTime.date() == QDate::currentDate();
}

bool Statistics::isThisWeek(const QDateTime &dateTime) const
{
    QDate today = QDate::currentDate();
    QDate date = dateTime.date();

    int weekNumberToday = today.weekNumber();
    int weekNumberDate = date.weekNumber();

    return weekNumberToday == weekNumberDate && today.year() == date.year();
}

bool Statistics::isThisMonth(const QDateTime &dateTime) const
{
    QDate today = QDate::currentDate();
    QDate date = dateTime.date();

    return today.month() == date.month() && today.year() == date.year();
}