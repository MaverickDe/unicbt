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
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void changeEvent(QEvent* event) override;

private slots:
    // Onboard
    void onUrlConfirmed(const QString& url);
    void onPingSuccess();
    void onPingFailed(const QString& error);

    // Bio
    void onStartExamRequested(const QString& reg, const QString& code);
    void onResumeExamRequested(const QString& reg, const QString& code, const QString& supCode);
    void onRetakeExamRequested(const QString& reg, const QString& code, const QString& supCode);

    void onValidateSuccess(const QJsonObject& data);
    void onValidateFailed(const QString& error);
    void onResumeSuccess(const QJsonObject& data);
    void onResumeFailed(const QString& error);
    void onRetakeSuccess(const QJsonObject& data);
    void onRetakeFailed(const QString& error);

    void onQuestionsReady(const QJsonArray& questions, const QJsonObject& meta);
    void onQuestionsFailed(const QString& error);

    // Exam
    void onSubmitRequested(const QMap<int,QString>& answers, int secondsUsed);
    void onSubmitSuccess(const QJsonObject& result);
    void onSubmitFailed(const QString& error);
    void onExitRequested();

    // Lockdown
    void onFocusViolation(const QString& reason);

    // Result
    void onNewExamRequested();

private:
    enum Page { PageOnboard = 0, PageBio, PageExam, PageResult };
    void goTo(Page page);
    void engageLockdown();
    void disengageLockdown();
    void beginExam(const QJsonObject& studentData, bool resumed = false);

    QStackedWidget*  m_stack;
    OnboardPage*     m_onboard;
    BioPage*         m_bio;
    ExamPage*        m_exam;
    ResultPage*      m_result;
    ApiClient*       m_api;
    LockdownManager* m_lockdown;

    ExamSession m_session;
    bool        m_examActive  = false;
    int         m_closeAttempts = 0;
};
