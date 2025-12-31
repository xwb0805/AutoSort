#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QWidget;
class QTabWidget;
class QTableWidget;
class QTranslator;
QT_END_NAMESPACE

class FileOrganizer;
class Statistics;
class LogManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void organizeNow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void toggleMonitoring();
    void showSettings();
    void showAbout();
    void showStatistics();
    void showLogViewer();
    void showFileSearch();
    void showBatchRename();
    void showFileFilter();
    void quitApplication();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onOrganizationCompleted(int count);
    void onFileConflict(const QString &sourcePath, const QString &targetPath);

private:
    void setupUI();
    void createActions();
    void createTrayIcon();
    void updateUI();
    void loadSettings();
    void saveSettings();
    void loadLanguage(const QString &language);

    FileOrganizer *m_fileOrganizer;
    Statistics *m_statistics;
    LogManager *m_logManager;
    QSystemTrayIcon *m_trayIcon;
    QTranslator *m_translator;  // 翻译器
    QMenu *m_trayIconMenu;

    QAction *m_toggleAction;
    QAction *m_organizeAction;
    QAction *m_settingsAction;
    QAction *m_statisticsAction;
    QAction *m_logViewerAction;
    QAction *m_fileSearchAction;
    QAction *m_batchRenameAction;
    QAction *m_fileFilterAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QLabel *m_statusLabel;
    QPushButton *m_organizeButton;
    QPushButton *m_settingsButton;
    QPushButton *m_statisticsButton;
    QPushButton *m_logViewerButton;
    QPushButton *m_fileSearchButton;
    QPushButton *m_batchRenameButton;
    QPushButton *m_fileFilterButton;
    QPushButton *m_toggleButton;

    bool m_isMonitoring;
};

#endif // MAINWINDOW_H