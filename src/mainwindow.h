#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QCloseEvent>
#include "pages/onboardpage.h"
#include "pages/biopage.h"
#include "pages/exampage.h"
#include "pages/resultpage.h"
#include "network/apiclient.h"
#include "lockdown/lockdownmanager.h"
#include "models/examsession.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event)  override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event)      override;

private slots:
    // ── Onboard
    void onUrlConfirmed(const QString& url);
    void onPingSuccess();
    void onPingFailed(const QString& error);

    // ── Bio
    void onStartExamRequested(const QString& reg, const QString& code);
    void onResumeExamRequested(const QString& reg, const QString& code, const QString& supCode);
    void onRetakeExamRequested(const QString& reg, const QString& code, const QString& supCode);

    // ── API responses
    void onValidateSuccess(const QJsonObject& data);
    void onValidateFailed(const QString& error);
    void onResumeSuccess(const QJsonObject& data);
    void onResumeFailed(const QString& error);
    void onRetakeSuccess(const QJsonObject& data);
    void onRetakeFailed(const QString& error);
    void onQuestionsReady(const QJsonArray& questions, const QJsonObject& meta);
    void onQuestionsFailed(const QString& error);

    // ── Exam
    void onSubmitRequested(const QMap<int, QString>& answers, int secondsUsed);
    void onSubmitSuccess(const QJsonObject& result);
    void onSubmitFailed(const QString& error);
    void onExitExamRequested(const QString& supervisorCode);  // supervised exit

    // ── Lockdown
    void onFocusViolation(const QString& reason);

    // ── Result
    void onNewExamRequested();

    // ── Exit (Onboard / Bio only)
    void onExitRequested();

private:
    enum Page { PageOnboard = 0, PageBio, PageExam, PageResult };

    void goTo(Page page);
    void engageLockdown();
    void disengageLockdown();
    void beginExam(const QJsonObject& studentData, bool resumed);

    QStackedWidget*  m_stack    = nullptr;
    OnboardPage*     m_onboard  = nullptr;
    BioPage*         m_bio      = nullptr;
    ExamPage*        m_exam     = nullptr;
    ResultPage*      m_result   = nullptr;
    ApiClient*       m_api      = nullptr;
    LockdownManager* m_lockdown = nullptr;

    ExamSession m_session;
    bool        m_examActive    = false;
    int         m_closeAttempts = 0;
};
