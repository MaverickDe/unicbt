#include "notificationpopup.h"
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

NotificationPopup::NotificationPopup(QWidget* parent)
    : QWidget(parent, Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(16, 12, 16, 12);
    lay->setSpacing(10);

    m_icon     = new QLabel(this);
    m_label    = new QLabel(this);
    m_closeBtn = new QPushButton("✕", this);
    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);

    m_label->setWordWrap(true);
    m_closeBtn->setFixedSize(20, 20);
    m_closeBtn->setCursor(Qt::PointingHandCursor);

    lay->addWidget(m_icon);
    lay->addWidget(m_label, 1);
    lay->addWidget(m_closeBtn);

    setMinimumWidth(320);
    setMaximumWidth(460);

    connect(m_closeBtn, &QPushButton::clicked, this, &QWidget::hide);
    connect(m_hideTimer, &QTimer::timeout, this, &QWidget::hide);
}

void NotificationPopup::show(const QString& message, NotifType type, int durationMs) {
    m_label->setText(message);
    styleFor(type);

    if (parentWidget()) {
        auto pw = parentWidget();
        int x = pw->width() - width() - 24;
        int y = 24;
        move(pw->mapToGlobal(QPoint(x, y)));
    }

    QWidget::show();
    raise();

    if (durationMs > 0)
        m_hideTimer->start(durationMs);
}

void NotificationPopup::styleFor(NotifType type) {
    QString bg, icon, border;
    switch (type) {
        case NotifType::Success:
            bg = "#1a3a2a"; border = "#2ecc71"; icon = "✓";
            m_icon->setStyleSheet("color:#2ecc71; font-size:18px; font-weight:bold;");
            break;
        case NotifType::Warning:
            bg = "#3a2e1a"; border = "#f39c12"; icon = "⚠";
            m_icon->setStyleSheet("color:#f39c12; font-size:18px; font-weight:bold;");
            break;
        case NotifType::Error:
            bg = "#3a1a1a"; border = "#e74c3c"; icon = "✕";
            m_icon->setStyleSheet("color:#e74c3c; font-size:18px; font-weight:bold;");
            break;
        default:
            bg = "#1a2a3a"; border = "#3498db"; icon = "ℹ";
            m_icon->setStyleSheet("color:#3498db; font-size:18px; font-weight:bold;");
            break;
    }
    m_icon->setText(icon);

    setStyleSheet(QString(
        "NotificationPopup { background:%1; border:2px solid %2; border-radius:10px; }"
        "QLabel { color:#f0f0f0; font-size:13px; background:transparent; }"
        "QPushButton { background:transparent; color:#aaa; border:none; font-size:13px; }"
        "QPushButton:hover { color:#fff; }"
    ).arg(bg, border));
}
