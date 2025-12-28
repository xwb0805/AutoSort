#include "mainwindow.h"
#include "fileorganizer.h"
#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QApplication>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QMenu>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_fileOrganizer(nullptr)
    , m_trayIcon(nullptr)
    , m_trayIconMenu(nullptr)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_statusLabel(nullptr)
    , m_organizeButton(nullptr)
    , m_settingsButton(nullptr)
    , m_toggleButton(nullptr)
    , m_isMonitoring(false)
{
    setupUI();
    createActions();
    createTrayIcon();

    m_fileOrganizer = new FileOrganizer(this);
    connect(m_fileOrganizer, &FileOrganizer::organizationCompleted, this, &MainWindow::onOrganizationCompleted);

    loadSettings();

    setWindowTitle("自动整理工具");
    setFixedSize(400, 300);

    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
    }
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);

    m_statusLabel = new QLabel("状态: 未启动", this);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 20px; }");

    m_organizeButton = new QPushButton("立即整理", this);
    m_organizeButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_settingsButton = new QPushButton("设置", this);
    m_settingsButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; }");

    m_toggleButton = new QPushButton("开始监控", this);
    m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #4CAF50; color: white; }");

    m_mainLayout->addWidget(m_statusLabel);
    m_mainLayout->addWidget(m_organizeButton);
    m_mainLayout->addWidget(m_settingsButton);
    m_mainLayout->addWidget(m_toggleButton);
    m_mainLayout->addStretch();

    connect(m_organizeButton, &QPushButton::clicked, this, &MainWindow::organizeNow);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::showSettings);
    connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::toggleMonitoring);
}

void MainWindow::createActions()
{
    m_toggleAction = new QAction("开始监控", this);
    connect(m_toggleAction, &QAction::triggered, this, &MainWindow::toggleMonitoring);

    m_organizeAction = new QAction("立即整理", this);
    connect(m_organizeAction, &QAction::triggered, this, &MainWindow::organizeNow);

    m_settingsAction = new QAction("设置", this);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);

    m_aboutAction = new QAction("关于", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);

    m_quitAction = new QAction("退出", this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWindow::createTrayIcon()
{
    m_trayIconMenu = new QMenu(this);
    m_trayIconMenu->addAction(m_toggleAction);
    m_trayIconMenu->addAction(m_organizeAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_settingsAction);
    m_trayIconMenu->addAction(m_aboutAction);
    m_trayIconMenu->addSeparator();
    m_trayIconMenu->addAction(m_quitAction);

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setContextMenu(m_trayIconMenu);
    m_trayIcon->setToolTip("自动整理工具");
    m_trayIcon->setIcon(QIcon(":/resources/icon.svg"));

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);

    m_trayIcon->show();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        setVisible(!isVisible());
    }
}

void MainWindow::showSettings()
{
    SettingsDialog dialog(this, m_fileOrganizer);
    if (dialog.exec() == QDialog::Accepted) {
        loadSettings();
    }
}

void MainWindow::organizeNow()
{
    if (!m_fileOrganizer) return;

    m_statusLabel->setText("状态: 正在整理...");
    QApplication::processEvents();

    int organizedCount = m_fileOrganizer->organizeFiles();

    m_statusLabel->setText(QString("状态: 已整理 %1 个文件").arg(organizedCount));

    if (m_trayIcon) {
        m_trayIcon->showMessage("整理完成", QString("已整理 %1 个文件").arg(organizedCount),
                               QSystemTrayIcon::Information, 3000);
    }
}

void MainWindow::toggleMonitoring()
{
    m_isMonitoring = !m_isMonitoring;

    if (m_isMonitoring) {
        m_fileOrganizer->startMonitoring();
        m_statusLabel->setText("状态: 监控中");
        m_toggleButton->setText("停止监控");
        m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #f44336; color: white; }");
        m_toggleAction->setText("停止监控");

        if (m_trayIcon) {
            m_trayIcon->showMessage("监控已开始", "正在监控下载文件夹", QSystemTrayIcon::Information, 2000);
        }
    } else {
        m_fileOrganizer->stopMonitoring();
        m_statusLabel->setText("状态: 已停止");
        m_toggleButton->setText("开始监控");
        m_toggleButton->setStyleSheet("QPushButton { padding: 10px; font-size: 14px; background-color: #4CAF50; color: white; }");
        m_toggleAction->setText("开始监控");

        if (m_trayIcon) {
            m_trayIcon->showMessage("监控已停止", "文件监控已停止", QSystemTrayIcon::Information, 2000);
        }
    }

    saveSettings();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "关于",
                      "自动整理工具 v1.0\n\n"
                      "一个基于Qt的自动文件整理工具\n"
                      "按文件扩展名自动分类整理文件\n\n"
                      "特性:\n"
                      "• 后台运行\n"
                      "• 系统托盘集成\n"
                      "• 实时文件监控\n"
                      "• 自定义整理规则");
}

void MainWindow::quitApplication()
{
    qApp->quit();
}

void MainWindow::onOrganizationCompleted(int count)
{
    m_statusLabel->setText(QString("状态: 已整理 %1 个文件").arg(count));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::loadSettings()
{
    QSettings settings;
    QString downloadPath = settings.value("downloadPath", QDir::homePath() + "/Downloads").toString();
    bool autoStart = settings.value("autoStart", false).toBool();

    if (m_fileOrganizer) {
        m_fileOrganizer->setDownloadPath(downloadPath);
        m_fileOrganizer->loadCustomRules();
    }

    if (autoStart && !m_isMonitoring) {
        toggleMonitoring();
    }
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("isMonitoring", m_isMonitoring);
}
