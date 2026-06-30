#pragma once
#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QPushButton>

enum class NotifType { Info, Success, Warning, Error };

class NotificationPopup : public QWidget {
    Q_OBJECT
public:
    explicit NotificationPopup(QWidget* parent = nullptr);
    void show(const QString& message, NotifType type = NotifType::Info, int durationMs = 3000);

private:
    QLabel*      m_icon;
    QLabel*      m_label;
    QTimer*      m_hideTimer;
    QPushButton* m_closeBtn;

    void styleFor(NotifType type);
};
