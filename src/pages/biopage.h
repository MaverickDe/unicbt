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
    QWidget* buildCodeForm(const QString& actionLabel, const QString& placeholder, bool isResume);

    // Main form
    QLineEdit*     m_regInput;
    QLineEdit*     m_examCodeInput;
    QPushButton*   m_startBtn;
    QPushButton*   m_resumeBtn;
    QPushButton*   m_retakeBtn;
    QLabel*        m_statusLabel;

    // Code input form (reused for resume/retake)
    QLineEdit*     m_codeInput;
    QPushButton*   m_confirmCodeBtn;
    QPushButton*   m_cancelCodeBtn;
    QLabel*        m_codeFormTitle;
    QLabel*        m_codeStatusLabel;

    QStackedWidget* m_stack;
    bool m_isResumeMode = true;
};
