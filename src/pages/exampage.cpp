#include "exampage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

ExamPage::ExamPage(QWidget* parent) : QWidget(parent) {
    buildUI();
    m_notif = new NotificationPopup(this);
}

void ExamPage::buildUI() {
    setStyleSheet(R"(
        ExamPage { background: #0f1020; }
        QWidget#topbar {
            background: #1a1d2e;
            border-bottom: 2px solid #2e3050;
        }
        QWidget#sidebar {
            background: #131520;
            border-right: 1px solid #2e3050;
        }
        QLabel#sideHeader {
            color: #8a8faa;
            font-size: 11px;
            font-weight: 600;
            letter-spacing: 1px;
        }
        QLabel#candidateName { color: #f0f0f0; font-size: 15px; font-weight: bold; }
        QLabel#courseName    { color: #5b6af0; font-size: 13px; }
        QLabel#statValue     { color: #4ecca3; font-size: 18px; font-weight: bold; }
        QLabel#statLabel     { color: #8a8faa; font-size: 11px; }
        QPushButton#submitBtn {
            background: #5b6af0;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            height: 48px;
        }
        QPushButton#submitBtn:hover { background: #6b7aff; }
        QPushButton#submitBtn:disabled { background: #2e3050; color: #555; }
        QPushButton#navBtn {
            background: #1c1e2e;
            color: #8a8faa;
            border: 1px solid #2e3050;
            border-radius: 6px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton#navBtn:hover { background: #252840; color: #f0f0f0; border-color:#5b6af0; }
        QPushButton#navBtn[answered="true"] {
            background: #1a3a2a; color: #2ecc71; border-color: #2ecc71;
        }
        QPushButton#navBtn[current="true"] {
            background: #1a2040; color: #fff; border-color: #5b6af0;
        }
        QPushButton#navBtn[flagged="true"] {
            background: #3a2e1a; color: #f39c12; border-color: #f39c12;
        }
        QPushButton#arrowBtn {
            background: #1c1e2e;
            color: #f0f0f0;
            border: 1px solid #2e3050;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            height: 44px;
            padding: 0 20px;
        }
        QPushButton#arrowBtn:hover { background: #252840; border-color:#5b6af0; }
        QPushButton#arrowBtn:disabled { color: #444; border-color:#1e2035; }
        QLabel#questionCounter { color: #8a8faa; font-size: 13px; }
        QFrame#hline { color: #2e3050; }
    )");

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0,0,0,0);
    root->setSpacing(0);

    // ─── SIDEBAR ──────────────────────────────────────────────────────
    m_sidebar = new QWidget(this);
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(260);

    auto* sideScroll = new QScrollArea(this);
    sideScroll->setWidget(m_sidebar);
    sideScroll->setWidgetResizable(true);
    sideScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sideScroll->setFixedWidth(260);
    sideScroll->setStyleSheet("QScrollArea { border:none; background:#13152000; }");

    auto* sideLay = new QVBoxLayout(m_sidebar);
    sideLay->setContentsMargins(16, 20, 16, 20);
    sideLay->setSpacing(0);

    // Candidate info
    auto* infoHeader = new QLabel("CANDIDATE", m_sidebar);
    infoHeader->setObjectName("sideHeader");
    m_candidateName = new QLabel("—", m_sidebar);
    m_candidateName->setObjectName("candidateName");
    m_candidateName->setWordWrap(true);
    m_courseName = new QLabel("—", m_sidebar);
    m_courseName->setObjectName("courseName");

    sideLay->addWidget(infoHeader);
    sideLay->addSpacing(6);
    sideLay->addWidget(m_candidateName);
    sideLay->addSpacing(2);
    sideLay->addWidget(m_courseName);
    sideLay->addSpacing(20);

    auto* d1 = new QFrame(m_sidebar); d1->setFrameShape(QFrame::HLine); d1->setObjectName("hline");
    sideLay->addWidget(d1);
    sideLay->addSpacing(16);

    // Timer
    auto* timerHeader = new QLabel("TIME REMAINING", m_sidebar);
    timerHeader->setObjectName("sideHeader");
    m_timer = new TimerWidget(m_sidebar);
    sideLay->addWidget(timerHeader);
    sideLay->addSpacing(6);
    sideLay->addWidget(m_timer);
    sideLay->addSpacing(20);

    auto* d2 = new QFrame(m_sidebar); d2->setFrameShape(QFrame::HLine); d2->setObjectName("hline");
    sideLay->addWidget(d2);
    sideLay->addSpacing(16);

    // Stats row
    auto* statsLay = new QHBoxLayout;
    statsLay->setSpacing(0);

    auto makeStatCell = [&](QLabel*& valLabel, const QString& lbl) {
        auto* cell = new QWidget(m_sidebar);
        auto* cl   = new QVBoxLayout(cell);
        cl->setContentsMargins(0,0,0,0);
        cl->setAlignment(Qt::AlignCenter);
        valLabel = new QLabel("0", cell);
        valLabel->setObjectName("statValue");
        valLabel->setAlignment(Qt::AlignCenter);
        auto* ll = new QLabel(lbl, cell);
        ll->setObjectName("statLabel");
        ll->setAlignment(Qt::AlignCenter);
        cl->addWidget(valLabel);
        cl->addWidget(ll);
        return cell;
    };

    statsLay->addWidget(makeStatCell(m_answeredCount, "Answered"));
    statsLay->addWidget(makeStatCell(m_totalCount,    "Total"));
    statsLay->addWidget(makeStatCell(m_requiredCount, "Required"));

    sideLay->addLayout(statsLay);
    sideLay->addSpacing(20);

    auto* d3 = new QFrame(m_sidebar); d3->setFrameShape(QFrame::HLine); d3->setObjectName("hline");
    sideLay->addWidget(d3);
    sideLay->addSpacing(16);

    // Nav grid header
    auto* navHeader = new QLabel("QUESTION NAVIGATOR", m_sidebar);
    navHeader->setObjectName("sideHeader");
    sideLay->addWidget(navHeader);
    sideLay->addSpacing(10);

    // Nav grid container (filled dynamically)
    m_navGrid = new QWidget(m_sidebar);
    m_navGrid->setLayout(new QGridLayout(m_navGrid));
    static_cast<QGridLayout*>(m_navGrid->layout())->setSpacing(6);
    static_cast<QGridLayout*>(m_navGrid->layout())->setContentsMargins(0,0,0,0);
    sideLay->addWidget(m_navGrid);
    sideLay->addSpacing(16);

    // Legend
    auto* legendLay = new QHBoxLayout;
    auto makeChip = [&](const QString& color, const QString& label) {
        auto* w = new QWidget;
        auto* wl = new QHBoxLayout(w);
        wl->setContentsMargins(0,0,0,0);
        wl->setSpacing(4);
        auto* dot = new QLabel("●");
        dot->setStyleSheet(QString("color:%1; font-size:10px;").arg(color));
        auto* lbl = new QLabel(label);
        lbl->setStyleSheet("color:#8a8faa; font-size:10px;");
        wl->addWidget(dot); wl->addWidget(lbl);
        return w;
    };
    legendLay->addWidget(makeChip("#2ecc71","Answered"));
    legendLay->addWidget(makeChip("#f39c12","Flagged"));
    legendLay->addWidget(makeChip("#8a8faa","Unanswered"));
    sideLay->addLayout(legendLay);
    sideLay->addStretch();
    sideLay->addSpacing(16);

    // Submit button
    m_submitBtn = new QPushButton("Submit Examination", m_sidebar);
    m_submitBtn->setObjectName("submitBtn");
    m_submitBtn->setFixedHeight(48);
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    sideLay->addWidget(m_submitBtn);

    // ─── MAIN AREA ───────────────────────────────────────────────────
    auto* mainWrapper = new QWidget(this);
    auto* mainLay = new QVBoxLayout(mainWrapper);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);

    m_questionWidget = new QuestionWidget(mainWrapper);
    mainLay->addWidget(m_questionWidget, 1);

    // Bottom nav bar
    auto* navBar = new QWidget(mainWrapper);
    navBar->setObjectName("topbar");
    navBar->setFixedHeight(64);
    auto* navBarLay = new QHBoxLayout(navBar);
    navBarLay->setContentsMargins(24, 10, 24, 10);
    navBarLay->setSpacing(12);

    m_prevBtn = new QPushButton("← Previous", navBar);
    m_prevBtn->setObjectName("arrowBtn");
    m_prevBtn->setCursor(Qt::PointingHandCursor);

    m_questionCounter = new QLabel("", navBar);
    m_questionCounter->setObjectName("questionCounter");
    m_questionCounter->setAlignment(Qt::AlignCenter);

    m_nextBtn = new QPushButton("Next →", navBar);
    m_nextBtn->setObjectName("arrowBtn");
    m_nextBtn->setCursor(Qt::PointingHandCursor);

    navBarLay->addWidget(m_prevBtn);
    navBarLay->addStretch();
    navBarLay->addWidget(m_questionCounter);
    navBarLay->addStretch();
    navBarLay->addWidget(m_nextBtn);

    mainLay->addWidget(navBar);

    root->addWidget(sideScroll);
    root->addWidget(mainWrapper, 1);

    // Connections
    connect(m_prevBtn,      &QPushButton::clicked, this, &ExamPage::onNavPrev);
    connect(m_nextBtn,      &QPushButton::clicked, this, &ExamPage::onNavNext);
    connect(m_submitBtn,    &QPushButton::clicked, this, &ExamPage::onSubmitClicked);
    connect(m_questionWidget, &QuestionWidget::answerChanged, this, &ExamPage::onAnswerChanged);
    connect(m_questionWidget, &QuestionWidget::flagToggled,   this, &ExamPage::onFlagToggled);
    connect(m_timer, &TimerWidget::timeWarning, this, &ExamPage::onTimeWarning);
    connect(m_timer, &TimerWidget::timeExpired, this, &ExamPage::onTimeExpired);
}

void ExamPage::loadSession(const ExamSession& session) {
    m_session      = session;
    m_currentIndex = 0;

    m_candidateName->setText(session.candidateName);
    m_courseName->setText(session.courseName);
    m_totalCount->setText(QString::number(session.totalQuestions));
    m_requiredCount->setText(QString::number(session.requiredQuestions));

    // Build nav grid
    qDeleteAll(m_navBtns);
    m_navBtns.clear();
    auto* grid = static_cast<QGridLayout*>(m_navGrid->layout());

    int cols = 5;
    for (int i = 0; i < session.questions.size(); i++) {
        auto* btn = new QPushButton(QString::number(i + 1), m_navGrid);
        btn->setObjectName("navBtn");
        btn->setFixedSize(38, 38);
        btn->setCursor(Qt::PointingHandCursor);
        grid->addWidget(btn, i / cols, i % cols);
        m_navBtns.append(btn);
        connect(btn, &QPushButton::clicked, this, [this, i]() { onNavJump(i); });
    }

    m_timer->start(session.durationSeconds, session.secondsElapsed);
    showQuestion(0);
    updateStatusBar();
}

void ExamPage::reset() {
    m_timer->stop();
    m_session = ExamSession{};
    m_currentIndex = 0;
    qDeleteAll(m_navBtns);
    m_navBtns.clear();
}

void ExamPage::showQuestion(int index) {
    if (index < 0 || index >= m_session.questions.size()) return;
    m_currentIndex = index;

    const auto& q = m_session.questions[index];
    QString existing = m_session.answers.value(q.id, "");
    m_questionWidget->loadQuestion(q, index + 1, m_session.questions.size(), existing);

    m_prevBtn->setEnabled(index > 0);
    m_nextBtn->setEnabled(index < m_session.questions.size() - 1);
    m_questionCounter->setText(QString("Question %1 / %2")
        .arg(index + 1).arg(m_session.questions.size()));

    updateNavGrid();
}

void ExamPage::updateNavGrid() {
    for (int i = 0; i < m_navBtns.size(); i++) {
        auto* btn = m_navBtns[i];
        int qid   = m_session.questions[i].id;
        bool answered = m_session.answers.contains(qid);
        bool flagged  = m_session.flagged.value(qid, false);
        bool current  = (i == m_currentIndex);

        btn->setProperty("current",  current);
        btn->setProperty("answered", !current && answered);
        btn->setProperty("flagged",  !current && flagged);
        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
    }
}

void ExamPage::updateStatusBar() {
    m_answeredCount->setText(QString::number(m_session.answeredCount()));
    m_totalCount->setText(QString::number(m_session.questions.size()));

    bool canSub = m_session.canSubmit();
    m_submitBtn->setEnabled(canSub);
    if (!canSub) {
        int need = m_session.requiredQuestions - m_session.answeredCount();
        m_submitBtn->setToolTip(QString("Answer %1 more question(s) to submit").arg(need));
    }
}

void ExamPage::onNavNext() { showQuestion(m_currentIndex + 1); }
void ExamPage::onNavPrev() { showQuestion(m_currentIndex - 1); }
void ExamPage::onNavJump(int idx) { showQuestion(idx); }

void ExamPage::onAnswerChanged(int questionId, const QString& answer) {
    if (answer.isEmpty()) m_session.answers.remove(questionId);
    else m_session.answers[questionId] = answer;
    updateNavGrid();
    updateStatusBar();
}

void ExamPage::onFlagToggled(int questionId, bool flagged) {
    m_session.flagged[questionId] = flagged;
    updateNavGrid();
    if (flagged)
        m_notif->show("Question flagged for review.", NotifType::Warning, 2000);
    else
        m_notif->show("Flag removed.", NotifType::Info, 1500);
}

void ExamPage::onSubmitClicked() {
    if (!m_session.canSubmit()) {
        m_notif->show(
            QString("You must answer at least %1 question(s) before submitting.")
                .arg(m_session.requiredQuestions),
            NotifType::Warning, 4000
        );
        return;
    }
    confirmSubmit();
}

void ExamPage::confirmSubmit() {
    // Count unanswered
    int unanswered = m_session.questions.size() - m_session.answeredCount();

    auto* dialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
    dialog->setModal(true);
    dialog->setFixedWidth(460);
    dialog->setAttribute(Qt::WA_TranslucentBackground);

    auto* card = new QWidget(dialog);
    card->setObjectName("modalCard");

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(50);
    shadow->setColor(QColor(0,0,0,200));
    shadow->setOffset(0,8);
    card->setGraphicsEffect(shadow);

    auto* outerLay = new QVBoxLayout(dialog);
    outerLay->setContentsMargins(0,0,0,0);
    outerLay->addWidget(card);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(36, 32, 36, 32);
    lay->setSpacing(14);

    auto* icon  = new QLabel("📋", card);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size:44px;");

    auto* title = new QLabel("Submit Examination?", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#f0f0f0; font-size:18px; font-weight:bold;");

    auto* stats = new QLabel(
        QString("<center>"
                "<span style='color:#4ecca3; font-size:15px;'>%1</span>"
                "<span style='color:#8a8faa; font-size:13px;'> answered &nbsp;&nbsp; </span>"
                "<span style='color:%3; font-size:15px;'>%2</span>"
                "<span style='color:#8a8faa; font-size:13px;'> unanswered</span>"
                "</center>")
            .arg(m_session.answeredCount())
            .arg(unanswered)
            .arg(unanswered > 0 ? "#e74c3c" : "#2ecc71"),
        card);
    stats->setTextFormat(Qt::RichText);

    auto* warning = new QLabel(
        unanswered > 0
            ? "⚠ You have unanswered questions. Once submitted, you cannot return."
            : "✓ All questions answered. Ready to submit.",
        card
    );
    warning->setAlignment(Qt::AlignCenter);
    warning->setWordWrap(true);
    warning->setStyleSheet(unanswered > 0
        ? "color:#f39c12; font-size:13px;"
        : "color:#2ecc71; font-size:13px;");

    auto* btnRow = new QHBoxLayout;
    auto* cancelBtn = new QPushButton("Go Back", card);
    auto* confirmBtn = new QPushButton("Submit Now", card);
    cancelBtn->setFixedHeight(44);
    confirmBtn->setFixedHeight(44);

    cancelBtn->setStyleSheet(R"(
        QPushButton { background:#2e3050; color:#aaa; border:none; border-radius:8px;
            font-size:14px; font-weight:bold; }
        QPushButton:hover { background:#3e4060; color:#fff; }
    )");
    confirmBtn->setStyleSheet(R"(
        QPushButton { background:#5b6af0; color:white; border:none; border-radius:8px;
            font-size:14px; font-weight:bold; }
        QPushButton:hover { background:#6b7aff; }
    )");

    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(confirmBtn);

    lay->addWidget(icon);
    lay->addWidget(title);
    lay->addWidget(stats);
    lay->addWidget(warning);
    lay->addSpacing(8);
    lay->addLayout(btnRow);

    card->setStyleSheet(R"(
        QWidget#modalCard {
            background: #1c1e2a;
            border: 1px solid #2e3050;
            border-radius: 16px;
        }
    )");

    connect(cancelBtn,  &QPushButton::clicked, dialog, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, dialog, &QDialog::accept);

    if (dialog->exec() == QDialog::Accepted) {
        m_timer->stop();
        emit submitRequested(m_session.answers, m_timer->elapsedSeconds());
    }
    delete dialog;
}

void ExamPage::onTimeWarning(int remaining) {
    if (remaining <= 60) {
        m_notif->show("⚠ Less than 1 minute remaining!", NotifType::Error, 5000);
    } else {
        m_notif->show("⚠ 5 minutes remaining — consider submitting soon.", NotifType::Warning, 5000);
    }
}

void ExamPage::onTimeExpired() {
    m_notif->show("Time is up! Submitting your exam automatically...", NotifType::Error, 0);
    // Auto-submit
    emit submitRequested(m_session.answers, m_session.durationSeconds);
}

void ExamPage::onFocusViolation(const QString& reason) {
    m_notif->show(
        QString("⚠ Focus violation detected: %1. Your activity is being logged.").arg(reason),
        NotifType::Error, 5000
    );
}
