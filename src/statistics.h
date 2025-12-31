#ifndef STATISTICS_H
#define STATISTICS_H

#include <QObject>
#include <QDateTime>
#include <QMap>

struct StatisticsData {
    int totalFilesOrganized;
    qint64 totalSpaceSaved;
    int filesOrganizedToday;
    int filesOrganizedThisWeek;
    int filesOrganizedThisMonth;
    QDateTime lastOrganizedTime;
    QMap<QString, int> filesByType;
    QMap<QString, int> filesByFolder;

    StatisticsData()
        : totalFilesOrganized(0)
        , totalSpaceSaved(0)
        , filesOrganizedToday(0)
        , filesOrganizedThisWeek(0)
        , filesOrganizedThisMonth(0)
    {}
};

class Statistics : public QObject
{
    Q_OBJECT

public:
    explicit Statistics(QObject *parent = nullptr);
    ~Statistics();

    void recordFileOrganized(const QString &extension, const QString &folder, qint64 fileSize);
    StatisticsData getStatistics() const;
    void resetStatistics();
    void loadStatistics();
    void saveStatistics();

signals:
    void statisticsUpdated();

private:
    void updatePeriodicCounts();
    bool isToday(const QDateTime &dateTime) const;
    bool isThisWeek(const QDateTime &dateTime) const;
    bool isThisMonth(const QDateTime &dateTime) const;

    StatisticsData m_data;
};

#endif // STATISTICS_H