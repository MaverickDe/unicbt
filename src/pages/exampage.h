#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QDialog>
#include <QTimer>
#include "../models/examsession.h"
#include "../widgets/timerwidget.h"
#include "../widgets/questionwidget.h"
#include "../widgets/notificationpopup.h"

class ExamPage : public QWidget {
    Q_OBJECT
public:
    explicit ExamPage(QWidget* parent = nullptr);
    void loadSession(const ExamSession& session);
    void reset();

signals:
    void submitRequested(const QMap<int,QString>& answers, int secondsUsed);
    void exitExamRequested(const QString& supervisorCode);   // supervised exit
    void violationOccurred(const QString& reason);           // logged to server

public slots:
    void onTimeWarning(int remaining);
    void onTimeExpired();
    void onFocusViolation(const QString& reason);

private slots:
    void onNavNext();
    void onNavPrev();
    void onNavJump(int questionIndex);
    void onAnswerChanged(int questionId, const QString& answer);
    void onFlagToggled(int questionId, bool flagged);
    void onSubmitClicked();
    void onExitClicked();
    void confirmSubmit();
    void onAutoSave();

private:
    void buildUI();
    void showQuestion(int index);
    void updateNavGrid();
    void updateStatusBar();

    // ── Sidebar ──────────────────────────────────────────────────────────
    QWidget*      m_sidebar        = nullptr;
    TimerWidget*  m_timer          = nullptr;
    QLabel*       m_candidateName  = nullptr;
    QLabel*       m_courseName     = nullptr;
    QLabel*       m_answeredCount  = nullptr;
    QLabel*       m_totalCount     = nullptr;
    QLabel*       m_requiredCount  = nullptr;
    QWidget*      m_navGrid        = nullptr;
    QPushButton*  m_submitBtn      = nullptr;
    QPushButton*  m_exitExamBtn    = nullptr;

    // ── Main area ────────────────────────────────────────────────────────
    QuestionWidget*    m_questionWidget   = nullptr;
    NotificationPopup* m_notif           = nullptr;
    QPushButton*       m_prevBtn         = nullptr;
    QPushButton*       m_nextBtn         = nullptr;
    QLabel*            m_questionCounter = nullptr;

    // ── State ────────────────────────────────────────────────────────────
    ExamSession         m_session;
    int                 m_currentIndex   = 0;
    int                 m_violationCount = 0;
    QTimer*             m_autoSaveTimer  = nullptr;
    QList<QPushButton*> m_navBtns;
};
