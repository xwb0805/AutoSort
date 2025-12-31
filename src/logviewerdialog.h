#ifndef LOGVIEWERDIALOG_H
#define LOGVIEWERDIALOG_H

#include <QDialog>
#include <QList>
#include "logmanager.h"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QTextEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QCheckBox;
QT_END_NAMESPACE

class LogViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogViewerDialog(LogManager *logManager, QWidget *parent = nullptr);
    ~LogViewerDialog();

private slots:
    void refreshLogs();
    void filterLogs();
    void clearLogs();
    void exportLogs();

private:
    void setupUI();
    void updateLogDisplay();
    QString formatLogEntry(const LogEntry &entry) const;
    QString logTypeToColor(LogType type) const;

    LogManager *m_logManager;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_filterLayout;
    QHBoxLayout *m_buttonLayout;
    QTextEdit *m_logTextEdit;
    QComboBox *m_filterComboBox;
    QCheckBox *m_autoRefreshCheckBox;
    QPushButton *m_refreshButton;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    QPushButton *m_closeButton;
    QLabel *m_logCountLabel;
};

#endif // LOGVIEWERDIALOG_H