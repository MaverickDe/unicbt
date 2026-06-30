#include <QApplication>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char* argv[]) {
    // High-DPI support
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("CBT Examination Portal");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("University CBT");

    // Base dark palette
    QPalette palette;
    palette.setColor(QPalette::Window,          QColor("#0f1020"));
    palette.setColor(QPalette::WindowText,      QColor("#f0f0f0"));
    palette.setColor(QPalette::Base,            QColor("#12141e"));
    palette.setColor(QPalette::AlternateBase,   QColor("#1c1e2a"));
    palette.setColor(QPalette::Text,            QColor("#f0f0f0"));
    palette.setColor(QPalette::Button,          QColor("#1c1e2a"));
    palette.setColor(QPalette::ButtonText,      QColor("#f0f0f0"));
    palette.setColor(QPalette::Highlight,       QColor("#5b6af0"));
    palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    palette.setColor(QPalette::Link,            QColor("#5b6af0"));
    app.setPalette(palette);

    // Global font
    app.setFont(QFont("Segoe UI", 10));

    // Prevent system gestures from minimising/switching
    app.setQuitOnLastWindowClosed(true);

    MainWindow window;
    window.show();

    return app.exec();
}
