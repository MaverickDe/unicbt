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

public slots:
    void onPingError(const QString& error);

signals:
    void urlConfirmed(const QString& url);
    void exitRequested();

private slots:
    void onProceedClicked();

private:
    void setConnecting(bool val);

    QLineEdit*   m_urlInput    = nullptr;
    QPushButton* m_proceedBtn  = nullptr;
    QPushButton* m_exitBtn     = nullptr;
    QLabel*      m_statusLabel = nullptr;
    bool         m_connecting  = false;
};
