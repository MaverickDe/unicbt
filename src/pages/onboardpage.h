#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

class OnboardPage : public QWidget {
    Q_OBJECT
public:
    explicit OnboardPage(QWidget* parent = nullptr);
    void reset();

signals:
    void urlConfirmed(const QString& url);
     void exitRequested();   // ← new

private slots:
    void onProceedClicked();
    void onExitClicked();   // ← new

private:
    QLineEdit*   m_urlInput;
    QPushButton* m_proceedBtn;
     QPushButton* m_exitBtn;   // ← new
    QLabel*      m_statusLabel;
    QLabel*      m_spinner;
    bool         m_connecting = false;

    void setConnecting(bool val);
};
