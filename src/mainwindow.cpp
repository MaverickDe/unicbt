#include "mainwindow.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QJsonArray>
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_stack(new QStackedWidget(this))
    , m_onboard(new OnboardPage(this))
    , m_bio(new BioPage(this))
    , m_exam(new ExamPage(this))
    , m_result(new ResultPage(this))
    , m_api(new ApiClient(this))
    , m_lockdown(new LockdownManager(this))
{
    setCentralWidget(m_stack);
    setWindowTitle("CBT Examination Portal");
    setMinimumSize(1024, 700);

    m_stack->addWidget(m_onboard);   // 0
    m_stack->addWidget(m_bio);       // 1
    m_stack->addWidget(m_exam);      // 2
    m_stack->addWidget(m_result);    // 3

    // ─── Onboard ─────────────────────────────────────────────────────────
    connect(m_onboard, &OnboardPage::urlConfirmed,  this, &MainWindow::onUrlConfirmed);
    connect(m_onboard, &OnboardPage::exitRequested, this, &MainWindow::onExitRequested);

    // ─── Bio ─────────────────────────────────────────────────────────────
    connect(m_bio, &BioPage::startExamRequested,  this, &MainWindow::onStartExamRequested);
    connect(m_bio, &BioPage::resumeExamRequested, this, &MainWindow::onResumeExamRequested);
    connect(m_bio, &BioPage::retakeExamRequested, this, &MainWindow::onRetakeExamRequested);
    connect(m_bio, &BioPage::exitRequested,       this, &MainWindow::onExitRequested);

    // ─── API ─────────────────────────────────────────────────────────────
    connect(m_api, &ApiClient::pingSuccess,     this, &MainWindow::onPingSuccess);
    connect(m_api, &ApiClient::pingFailed,      this, &MainWindow::onPingFailed);
    connect(m_api, &ApiClient::validateSuccess, this, &MainWindow::onValidateSuccess);
    connect(m_api, &ApiClient::validateFailed,  this, &MainWindow::onValidateFailed);
    connect(m_api, &ApiClient::questionsReady,  this, &MainWindow::onQuestionsReady);
    connect(m_api, &ApiClient::questionsFailed, this, &MainWindow::onQuestionsFailed);
    connect(m_api, &ApiClient::resumeSuccess,   this, &MainWindow::onResumeSuccess);
    connect(m_api, &ApiClient::resumeFailed,    this, &MainWindow::onResumeFailed);
    connect(m_api, &ApiClient::retakeSuccess,   this, &MainWindow::onRetakeSuccess);
    connect(m_api, &ApiClient::retakeFailed,    this, &MainWindow::onRetakeFailed);
    connect(m_api, &ApiClient::submitSuccess,   this, &MainWindow::onSubmitSuccess);
    connect(m_api, &ApiClient::submitFailed,    this, &MainWindow::onSubmitFailed);

    // ─── Exam ─────────────────────────────────────────────────────────────
    connect(m_exam, &ExamPage::submitRequested,   this, &MainWindow::onSubmitRequested);
    connect(m_exam, &ExamPage::exitExamRequested, this, &MainWindow::onExitExamRequested);

    // ─── Lockdown ─────────────────────────────────────────────────────────
    connect(m_lockdown, &LockdownManager::focusViolationDetected,
            this, &MainWindow::onFocusViolation);

    // ─── Result ───────────────────────────────────────────────────────────
    connect(m_result, &ResultPage::newExamRequested, this, &MainWindow::onNewExamRequested);

    goTo(PageOnboard);
}

// ── Navigation ──────────────────────────────────────────────────────────────
void MainWindow::goTo(Page page) {
    m_stack->setCurrentIndex(static_cast<int>(page));
}

void MainWindow::engageLockdown() {
    m_examActive    = true;
    m_closeAttempts = 0;
    m_lockdown->engage(this);
}

void MainWindow::disengageLockdown() {
    m_examActive = false;
    m_lockdown->disengage();
}

// ── Window guards ────────────────────────────────────────────────────────────
void MainWindow::closeEvent(QCloseEvent* event) {
    if (m_examActive) {
        event->ignore();
        m_closeAttempts++;
        m_exam->onFocusViolation(
            QString("Close attempt #%1 blocked — use the Exit button in the sidebar")
                .arg(m_closeAttempts));
        showFullScreen();
        raise();
        activateWindow();
        return;
    }
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (m_examActive) {
        switch (event->key()) {
            case Qt::Key_Escape:
            case Qt::Key_F1:  case Qt::Key_F2:  case Qt::Key_F3:
            case Qt::Key_F4:  case Qt::Key_F5:  case Qt::Key_F6:
            case Qt::Key_F7:  case Qt::Key_F8:  case Qt::Key_F9:
            case Qt::Key_F10: case Qt::Key_F11: case Qt::Key_F12:
                event->ignore();
                return;
            default: break;
        }
        if (event->modifiers() & Qt::AltModifier)  { event->ignore(); return; }
        if (event->modifiers() & Qt::MetaModifier) { event->ignore(); return; }
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::changeEvent(QEvent* event) {
    if (m_examActive && event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            showFullScreen();
            raise();
            activateWindow();
            m_exam->onFocusViolation("Minimise attempt blocked");
        }
    }
    QMainWindow::changeEvent(event);
}

// ── Onboard ──────────────────────────────────────────────────────────────────
void MainWindow::onUrlConfirmed(const QString& url) {
    m_api->setBaseUrl(url);
    m_api->pingServer();
}

void MainWindow::onPingSuccess() {
    m_session.serverUrl = m_api->baseUrl();
    m_bio->reset();
    goTo(PageBio);
    // No lockdown here — exam hasn't started yet
}

void MainWindow::onPingFailed(const QString& error) {
    m_onboard->onPingError(error);
}

// ── Bio ───────────────────────────────────────────────────────────────────────
void MainWindow::onStartExamRequested(const QString& reg, const QString& code) {
    m_session.regNumber = reg;
    m_session.examCode  = code;
    m_bio->setLoading(true, "Validating credentials...");
    m_api->validateStudent(reg, code);
}

void MainWindow::onResumeExamRequested(const QString& reg, const QString& code, const QString& supCode) {
    m_session.regNumber = reg;
    m_session.examCode  = code;
    m_bio->setLoading(true, "Verifying resume code...");
    m_api->resumeExam(reg, code, supCode);
}

void MainWindow::onRetakeExamRequested(const QString& reg, const QString& code, const QString& supCode) {
    m_session.regNumber = reg;
    m_session.examCode  = code;
    m_bio->setLoading(true, "Verifying retake code...");
    m_api->retakeExam(reg, code, supCode);
}

void MainWindow::onValidateSuccess(const QJsonObject& data) { beginExam(data, false); }
void MainWindow::onValidateFailed(const QString& error)     { m_bio->setError("❌ " + error); }
void MainWindow::onResumeSuccess(const QJsonObject& data)   { beginExam(data, true); }
void MainWindow::onResumeFailed(const QString& error)       { m_bio->setError("❌ " + error); }
void MainWindow::onRetakeSuccess(const QJsonObject& data)   { beginExam(data, false); }
void MainWindow::onRetakeFailed(const QString& error)       { m_bio->setError("❌ " + error); }

void MainWindow::beginExam(const QJsonObject& studentData, bool resumed) {
    m_session.candidateName     = studentData["name"].toString("Student");
    m_session.courseName        = studentData["course"].toString(m_session.examCode);
    m_session.sessionToken      = studentData["session_token"].toString();
    m_session.durationSeconds   = studentData["duration_seconds"].toInt(3600);
    m_session.requiredQuestions = studentData["required_questions"].toInt(1);
    m_session.isResumed         = resumed;

    if (resumed) {
        m_session.secondsElapsed = studentData["seconds_elapsed"].toInt(0);
        QJsonObject saved = studentData["saved_answers"].toObject();
        for (auto it = saved.begin(); it != saved.end(); ++it)
            m_session.answers[it.key().toInt()] = it.value().toString();
    } else {
        m_session.secondsElapsed = 0;
        m_session.answers.clear();
    }

    m_bio->setLoading(true, "Loading exam questions...");
    m_api->fetchQuestions(m_session.sessionToken);
}

void MainWindow::onQuestionsReady(const QJsonArray& questions, const QJsonObject& meta) {
    m_session.questions.clear();
    for (const auto& q : questions)
        m_session.questions.append(Question::fromJson(q.toObject()));

    m_session.totalQuestions = m_session.questions.size();
    if (meta.contains("required_questions"))
        m_session.requiredQuestions = meta["required_questions"].toInt(1);

    m_bio->setLoading(false);
    m_exam->reset();
    m_exam->loadSession(m_session);
    goTo(PageExam);

    // Lockdown engages HERE — real exam start
    engageLockdown();
}

void MainWindow::onQuestionsFailed(const QString& error) {
    m_bio->setError("❌ Failed to load questions: " + error);
}

// ── Exam ──────────────────────────────────────────────────────────────────────
void MainWindow::onSubmitRequested(const QMap<int, QString>& answers, int secondsUsed) {
    m_session.answers = answers;
    m_api->submitExam(m_session.sessionToken, answers, secondsUsed);
}

void MainWindow::onSubmitSuccess(const QJsonObject& result) {
    disengageLockdown();
    m_exam->reset();
    m_result->showResult(m_session, result);
    goTo(PageResult);
    showNormal();
    setWindowFlags(Qt::Window);
    show();
}

void MainWindow::onSubmitFailed(const QString& error) {
    m_exam->onFocusViolation("Submit failed: " + error + ". Please try again.");
}

void MainWindow::onExitExamRequested(const QString& supervisorCode) {
    // Save progress then release lockdown — exam marked as abandoned
    // supervisorCode would be validated server-side in a real deployment
    Q_UNUSED(supervisorCode)

    m_api->submitExam(
        m_session.sessionToken,
        m_session.answers,
        0   // server calculates time from session start timestamp
    );

    disengageLockdown();
    m_exam->reset();

    // Reset and return to onboard for next candidate
    m_session = ExamSession{};
    m_session.serverUrl = m_api->baseUrl();
    m_onboard->reset();
    m_bio->reset();
    goTo(PageOnboard);
}

// ── Lockdown events ───────────────────────────────────────────────────────────
void MainWindow::onFocusViolation(const QString& reason) {
    // Forward violation notification to the exam page UI
    m_exam->onFocusViolation(reason);
}

// ── Result ────────────────────────────────────────────────────────────────────
void MainWindow::onNewExamRequested() {
    m_session = ExamSession{};
    m_session.serverUrl = m_api->baseUrl();
    m_onboard->reset();
    m_bio->reset();
    goTo(PageOnboard);
}

// ── Exit (Onboard / Bio only — before exam starts) ────────────────────────────
void MainWindow::onExitRequested() {
    QApplication::quit();
}