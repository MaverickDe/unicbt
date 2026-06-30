#pragma once
#include <QWidget>
#include <QTimer>
#include <QLabel>

class TimerWidget : public QWidget {
    Q_OBJECT
public:
    explicit TimerWidget(QWidget* parent = nullptr);

    void start(int totalSeconds, int elapsedSeconds = 0);
    void stop();
    void pause();
    void resume();

    int elapsedSeconds() const { return m_elapsed; }
    int remainingSeconds() const { return m_total - m_elapsed; }

signals:
    void tick(int remaining);
    void timeWarning(int remaining);   // at 5 min, 1 min
    void timeExpired();

private slots:
    void onTick();

private:
    void updateDisplay();

    QTimer* m_timer;
    QLabel* m_display;
    int m_total   = 0;
    int m_elapsed = 0;
    bool m_warned5 = false;
    bool m_warned1 = false;
};
