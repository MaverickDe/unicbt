#include "timerwidget.h"
#include <QVBoxLayout>

TimerWidget::TimerWidget(QWidget* parent)
    : QWidget(parent)
    , m_timer(new QTimer(this))
    , m_display(new QLabel("00:00:00", this))
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(m_display);

    m_display->setAlignment(Qt::AlignCenter);
    m_display->setObjectName("timerDisplay");
    m_display->setStyleSheet(R"(
        QLabel#timerDisplay {
            font-family: 'Courier New', monospace;
            font-size: 28px;
            font-weight: bold;
            color: #4ecca3;
            letter-spacing: 2px;
        }
    )");

    connect(m_timer, &QTimer::timeout, this, &TimerWidget::onTick);
}

void TimerWidget::start(int totalSeconds, int elapsedSeconds) {
    m_total   = totalSeconds;
    m_elapsed = elapsedSeconds;
    m_warned5 = false;
    m_warned1 = false;
    updateDisplay();
    m_timer->start(1000);
}

void TimerWidget::stop()   { m_timer->stop(); }
void TimerWidget::pause()  { m_timer->stop(); }
void TimerWidget::resume() { m_timer->start(1000); }

void TimerWidget::onTick() {
    m_elapsed++;
    int remaining = m_total - m_elapsed;
    updateDisplay();
    emit tick(remaining);

    if (!m_warned5 && remaining <= 300) {
        m_warned5 = true;
        emit timeWarning(remaining);
        m_display->setStyleSheet(R"(
            QLabel#timerDisplay {
                font-family: 'Courier New', monospace;
                font-size: 28px;
                font-weight: bold;
                color: #f39c12;
                letter-spacing: 2px;
            }
        )");
    }
    if (!m_warned1 && remaining <= 60) {
        m_warned1 = true;
        emit timeWarning(remaining);
        m_display->setStyleSheet(R"(
            QLabel#timerDisplay {
                font-family: 'Courier New', monospace;
                font-size: 28px;
                font-weight: bold;
                color: #e74c3c;
                letter-spacing: 2px;
            }
        )");
    }
    if (remaining <= 0) {
        m_timer->stop();
        emit timeExpired();
    }
}

void TimerWidget::updateDisplay() {
    int remaining = qMax(0, m_total - m_elapsed);
    int h = remaining / 3600;
    int m = (remaining % 3600) / 60;
    int s = remaining % 60;
    m_display->setText(QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}
