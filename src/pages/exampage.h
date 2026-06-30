#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>
#include <QDialog>
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
    void exitBlocked();        // user tried to exit, blocked
    void examAbandoned();      // time expired or forced abandon

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
    void confirmSubmit();

private:
    void buildUI();
    void showQuestion(int index);
    void updateNavGrid();
    void updateStatusBar();
    QDialog* buildSubmitModal();

    // Layout areas
    QWidget*         m_sidebar;
    QWidget*         m_mainArea;
    QuestionWidget*  m_questionWidget;
    NotificationPopup* m_notif;

    // Sidebar widgets
    TimerWidget*     m_timer;
    QLabel*          m_candidateName;
    QLabel*          m_courseName;
    QLabel*          m_answeredCount;
    QLabel*          m_totalCount;
    QLabel*          m_requiredCount;
    QWidget*         m_navGrid;
    QPushButton*     m_submitBtn;

    // Navigation
    QPushButton*     m_prevBtn;
    QPushButton*     m_nextBtn;
    QLabel*          m_questionCounter;

    ExamSession m_session;
    int         m_currentIndex = 0;

    QList<QPushButton*> m_navBtns;
};
