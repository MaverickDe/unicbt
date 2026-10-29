#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>

class BioPage : public QWidget {
    Q_OBJECT
public:
    explicit BioPage(QWidget* parent = nullptr);
    void reset();

signals:
    void startExamRequested(const QString& regNum, const QString& examCode);
    void resumeExamRequested(const QString& regNum, const QString& examCode, const QString& resumeCode);
    void retakeExamRequested(const QString& regNum, const QString& examCode, const QString& retakeCode);
    void exitRequested();

public slots:
    void setLoading(bool loading, const QString& msg = "");
    void setError(const QString& error);
    void showResumeCodeInput();
    void showRetakeCodeInput();

private slots:
    void onStartClicked();
    void onResumeClicked();
    void onRetakeClicked();
    void onConfirmResume();
    void onConfirmRetake();
    void onCancelExtra();

private:
    QWidget* buildMainForm();
    // matches the .cpp definition exactly — 3 parameters
    QWidget* buildCodeForm(const QString& actionLabel, const QString& placeholder, bool isResume);

    // Main form widgets
    QLineEdit*   m_regInput      = nullptr;
    QLineEdit*   m_examCodeInput = nullptr;
    QPushButton* m_startBtn      = nullptr;
    QPushButton* m_resumeBtn     = nullptr;
    QPushButton* m_retakeBtn     = nullptr;
    QPushButton* m_exitBtn       = nullptr;   // exit before exam starts
    QLabel*      m_statusLabel   = nullptr;

    // Supervisor code form widgets (shared for resume/retake)
    QLineEdit*   m_codeInput       = nullptr;
    QPushButton* m_confirmCodeBtn  = nullptr;
    QPushButton* m_cancelCodeBtn   = nullptr;
    QLabel*      m_codeFormTitle   = nullptr;
    QLabel*      m_codeStatusLabel = nullptr;

    QStackedWidget* m_stack        = nullptr;
    bool            m_isResumeMode = true;
};
