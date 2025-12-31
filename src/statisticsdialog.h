#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QDialog>
#include "statistics.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;
class QGroupBox;
class QTableWidget;
class QProgressBar;
class QDateTimeEdit;
QT_END_NAMESPACE

class StatisticsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StatisticsDialog(Statistics *statistics, QWidget *parent = nullptr);
    ~StatisticsDialog();

private slots:
    void refreshStatistics();
    void resetStatistics();
    void exportStatistics();

private:
    void setupUI();
    void updateStatisticsDisplay();
    QString formatFileSize(qint64 size) const;

    Statistics *m_statistics;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_buttonLayout;

    QGroupBox *m_summaryGroup;
    QLabel *m_totalFilesLabel;
    QLabel *m_totalSpaceLabel;
    QLabel *m_todayFilesLabel;
    QLabel *m_weekFilesLabel;
    QLabel *m_monthFilesLabel;
    QLabel *mLastOrganizedLabel;

    QGroupBox *m_byTypeGroup;
    QTableWidget *m_byTypeTable;

    QGroupBox *m_byFolderGroup;
    QTableWidget *m_byFolderTable;

    QPushButton *m_refreshButton;
    QPushButton *m_resetButton;
    QPushButton *m_exportButton;
    QPushButton *m_closeButton;
};

#endif // STATISTICSDIALOG_H