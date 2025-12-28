#include <QApplication>
#include <QStyleFactory>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSystemTrayIcon>
#include <QSettings>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("AutoSort");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("AutoSort");
    app.setOrganizationDomain("autosort.local");

    QSharedMemory sharedMemory("AutoSort_SingleInstance");
    if (!sharedMemory.create(1)) {
        QMessageBox::information(nullptr, "自动整理工具", "程序已经在运行中");
        return 0;
    }

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "自动整理工具", "系统托盘不可用，程序无法正常运行");
        return 1;
    }

    app.setStyle(QStyleFactory::create("Fusion"));
    app.setWindowIcon(QIcon(":/resources/icon.svg"));

    MainWindow window;

    if (argc > 1) {
        QString arg = QString(argv[1]).toLower();
        if (arg == "--hidden" || arg == "-h") {
            window.hide();
        } else if (arg == "--organize" || arg == "-o") {
            window.organizeNow();
            return 0;
        } else if (arg == "--help" || arg == "-?") {
            QMessageBox::information(nullptr, "帮助",
                "自动整理工具 v1.0\n\n"
                "命令行参数:\n"
                "--hidden, -h    : 隐藏启动\n"
                "--organize, -o  : 立即整理一次后退出\n"
                "--help, -?      : 显示帮助信息");
            return 0;
        }
    }

    QSettings settings;
    if (!settings.value("startMinimized", false).toBool()) {
        window.show();
    }

    return app.exec();
}