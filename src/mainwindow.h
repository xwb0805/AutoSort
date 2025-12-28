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
QT_END_NAMESPACE

class FileOrganizer;

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
    void quitApplication();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onOrganizationCompleted(int count);

private:
    void setupUI();
    void createActions();
    void createTrayIcon();
    void updateUI();
    void loadSettings();
    void saveSettings();

    FileOrganizer *m_fileOrganizer;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayIconMenu;

    QAction *m_toggleAction;
    QAction *m_organizeAction;
    QAction *m_settingsAction;
    QAction *m_aboutAction;
    QAction *m_quitAction;

    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QLabel *m_statusLabel;
    QPushButton *m_organizeButton;
    QPushButton *m_settingsButton;
    QPushButton *m_toggleButton;

    bool m_isMonitoring;
};

#endif // MAINWINDOW_H