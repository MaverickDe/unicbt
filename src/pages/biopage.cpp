#include "biopage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

static const QString CARD_BASE_STYLE = R"(
    QWidget#card {
        background: #1c1e2a;
        border: 1px solid #2e3050;
        border-radius: 20px;
    }
    QLineEdit {
        background: #12141e;
        border: 2px solid #2e3050;
        border-radius: 8px;
        color: #f0f0f0;
        font-size: 15px;
        padding: 0 14px;
        height: 48px;
    }
    QLineEdit:focus { border-color: #5b6af0; }
    QPushButton#startBtn {
        background: #5b6af0;
        color: white;
        border: none;
        border-radius: 8px;
        font-size: 15px;
        font-weight: bold;
        height: 50px;
    }
    QPushButton#startBtn:hover    { background: #6b7aff; }
    QPushButton#startBtn:disabled { background: #2e3050; color: #555; }
    QPushButton#resumeBtn {
        background: #1a3a2a;
        color: #2ecc71;
        border: 1px solid #2ecc71;
        border-radius: 8px;
        font-size: 13px;
        font-weight: bold;
        height: 44px;
    }
    QPushButton#resumeBtn:hover { background: #2a4a3a; }
    QPushButton#retakeBtn {
        background: #3a2e1a;
        color: #f39c12;
        border: 1px solid #f39c12;
        border-radius: 8px;
        font-size: 13px;
        font-weight: bold;
        height: 44px;
    }
    QPushButton#retakeBtn:hover { background: #4a3e2a; }
    QPushButton#exitBtn {
        background: transparent;
        color: #8a8faa;
        border: 1px solid #2e3050;
        border-radius: 8px;
        font-size: 13px;
        height: 40px;
    }
    QPushButton#exitBtn:hover {
        color: #e74c3c;
        border-color: #e74c3c;
        background: #2a1a1a;
    }
    QPushButton#confirmCodeBtn {
        background: #5b6af0;
        color: white;
        border: none;
        border-radius: 8px;
        font-size: 14px;
        font-weight: bold;
        height: 48px;
    }
    QPushButton#confirmCodeBtn:hover { background: #6b7aff; }
    QPushButton#cancelCodeBtn {
        background: #2e3050;
        color: #aaa;
        border: none;
        border-radius: 8px;
        font-size: 14px;
        height: 48px;
    }
    QPushButton#cancelCodeBtn:hover { background: #3e4060; color: #fff; }
    QLabel#statusLabel, QLabel#codeStatusLabel {
        color: #e74c3c;
        font-size: 13px;
        min-height: 18px;
    }
    QFrame#hline { color: #2e3050; }
)";

BioPage::BioPage(QWidget* parent) : QWidget(parent) {
    setStyleSheet("BioPage { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                  "stop:0 #0a0c14, stop:0.5 #0f1120, stop:1 #0a0c14); }");

    auto* outerLay = new QVBoxLayout(this);
    outerLay->setAlignment(Qt::AlignCenter);

    m_stack = new QStackedWidget(this);
    m_stack->setFixedWidth(540);
    m_stack->addWidget(buildMainForm());
    m_stack->addWidget(buildCodeForm("Resume Exam", "Enter supervisor resume code", true));

    outerLay->addWidget(m_stack);
}

QWidget* BioPage::buildMainForm() {
    auto* card = new QWidget();
    card->setObjectName("card");
    card->setStyleSheet(CARD_BASE_STYLE);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(60);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(0, 12);
    card->setGraphicsEffect(shadow);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(44, 40, 44, 40);
    lay->setSpacing(0);

    // ── Header ───────────────────────────────────────────────────────────
    auto* icon = new QLabel("👤", card);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size:40px;");

    auto* title = new QLabel("Student Identification", card);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#f0f0f0; font-size:20px; font-weight:bold;");

    auto* sub = new QLabel("Enter your details to access your examination", card);
    sub->setAlignment(Qt::AlignCenter);
    sub->setStyleSheet("color:#8a8faa; font-size:13px;");

    lay->addWidget(icon);
    lay->addSpacing(8);
    lay->addWidget(title);
    lay->addSpacing(4);
    lay->addWidget(sub);
    lay->addSpacing(28);

    auto* divider = new QFrame(card);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("hline");
    lay->addWidget(divider);
    lay->addSpacing(24);

    // ── Reg number ───────────────────────────────────────────────────────
    auto* regLabel = new QLabel("REGISTRATION NUMBER", card);
    regLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");
    m_regInput = new QLineEdit(card);
    m_regInput->setPlaceholderText("e.g. CSC/2021/001");
    m_regInput->setFixedHeight(48);

    // ── Exam code ────────────────────────────────────────────────────────
    auto* examLabel = new QLabel("EXAMINATION CODE", card);
    examLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");
    m_examCodeInput = new QLineEdit(card);
    m_examCodeInput->setPlaceholderText("e.g. CSC301-2024");
    m_examCodeInput->setFixedHeight(48);

    lay->addWidget(regLabel);
    lay->addSpacing(6);
    lay->addWidget(m_regInput);
    lay->addSpacing(16);
    lay->addWidget(examLabel);
    lay->addSpacing(6);
    lay->addWidget(m_examCodeInput);
    lay->addSpacing(6);

    m_statusLabel = new QLabel("", card);
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    lay->addWidget(m_statusLabel);
    lay->addSpacing(20);

    // ── Start button ─────────────────────────────────────────────────────
    m_startBtn = new QPushButton("▶  Start Examination", card);
    m_startBtn->setObjectName("startBtn");
    m_startBtn->setFixedHeight(50);
    m_startBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(m_startBtn);
    lay->addSpacing(12);

    // ── Resume / Retake row ──────────────────────────────────────────────
    auto* actionRow = new QHBoxLayout;
    actionRow->setSpacing(10);

    m_resumeBtn = new QPushButton("▶  Resume Exam", card);
    m_resumeBtn->setObjectName("resumeBtn");
    m_resumeBtn->setCursor(Qt::PointingHandCursor);

    m_retakeBtn = new QPushButton("↺  Retake Exam", card);
    m_retakeBtn->setObjectName("retakeBtn");
    m_retakeBtn->setCursor(Qt::PointingHandCursor);

    actionRow->addWidget(m_resumeBtn);
    actionRow->addWidget(m_retakeBtn);
    lay->addLayout(actionRow);
    lay->addSpacing(12);

    // ── Exit button ──────────────────────────────────────────────────────
    m_exitBtn = new QPushButton("Exit Application", card);
    m_exitBtn->setObjectName("exitBtn");
    m_exitBtn->setFixedHeight(40);
    m_exitBtn->setCursor(Qt::PointingHandCursor);
    lay->addWidget(m_exitBtn);

    // ── Connections ──────────────────────────────────────────────────────
    connect(m_startBtn,      &QPushButton::clicked,     this, &BioPage::onStartClicked);
    connect(m_resumeBtn,     &QPushButton::clicked,     this, &BioPage::onResumeClicked);
    connect(m_retakeBtn,     &QPushButton::clicked,     this, &BioPage::onRetakeClicked);
    connect(m_exitBtn,       &QPushButton::clicked,     this, &BioPage::exitRequested);
    connect(m_examCodeInput, &QLineEdit::returnPressed, this, &BioPage::onStartClicked);

    return card;
}

QWidget* BioPage::buildCodeForm(const QString& /*actionLabel*/, const QString& /*placeholder*/, bool /*isResume*/) {
    auto* card = new QWidget();
    card->setObjectName("card");
    card->setStyleSheet(CARD_BASE_STYLE);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(60);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(0, 12);
    card->setGraphicsEffect(shadow);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(44, 40, 44, 40);
    lay->setSpacing(0);

    auto* icon = new QLabel("🔑", card);
    icon->setAlignment(Qt::AlignCenter);
    icon->setStyleSheet("font-size:40px;");

    m_codeFormTitle = new QLabel("Supervisor Code Required", card);
    m_codeFormTitle->setAlignment(Qt::AlignCenter);
    m_codeFormTitle->setStyleSheet("color:#f0f0f0; font-size:20px; font-weight:bold;");

    auto* desc = new QLabel("Please ask your supervisor for the authorisation code.", card);
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    desc->setStyleSheet("color:#8a8faa; font-size:13px;");

    auto* divider = new QFrame(card);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("hline");

    auto* codeLabel = new QLabel("SUPERVISOR CODE", card);
    codeLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");

    m_codeInput = new QLineEdit(card);
    m_codeInput->setEchoMode(QLineEdit::Password);
    m_codeInput->setPlaceholderText("Enter code...");
    m_codeInput->setFixedHeight(48);
    m_codeInput->setAlignment(Qt::AlignCenter);
    m_codeInput->setStyleSheet("font-size:20px; letter-spacing:6px; font-family: monospace;");

    m_codeStatusLabel = new QLabel("", card);
    m_codeStatusLabel->setObjectName("codeStatusLabel");
    m_codeStatusLabel->setAlignment(Qt::AlignCenter);

    m_confirmCodeBtn = new QPushButton("Confirm", card);
    m_confirmCodeBtn->setObjectName("confirmCodeBtn");
    m_confirmCodeBtn->setFixedHeight(48);
    m_confirmCodeBtn->setCursor(Qt::PointingHandCursor);

    m_cancelCodeBtn = new QPushButton("← Back", card);
    m_cancelCodeBtn->setObjectName("cancelCodeBtn");
    m_cancelCodeBtn->setFixedHeight(48);
    m_cancelCodeBtn->setCursor(Qt::PointingHandCursor);

    lay->addWidget(icon);
    lay->addSpacing(10);
    lay->addWidget(m_codeFormTitle);
    lay->addSpacing(6);
    lay->addWidget(desc);
    lay->addSpacing(24);
    lay->addWidget(divider);
    lay->addSpacing(24);
    lay->addWidget(codeLabel);
    lay->addSpacing(8);
    lay->addWidget(m_codeInput);
    lay->addSpacing(8);
    lay->addWidget(m_codeStatusLabel);
    lay->addSpacing(16);
    lay->addWidget(m_confirmCodeBtn);
    lay->addSpacing(10);
    lay->addWidget(m_cancelCodeBtn);

    connect(m_confirmCodeBtn, &QPushButton::clicked, this, [this]() {
        if (m_isResumeMode) onConfirmResume();
        else onConfirmRetake();
    });
    connect(m_cancelCodeBtn,  &QPushButton::clicked, this, &BioPage::onCancelExtra);
    connect(m_codeInput, &QLineEdit::returnPressed,  this, [this]() {
        if (m_isResumeMode) onConfirmResume();
        else onConfirmRetake();
    });

    return card;
}

void BioPage::reset() {
    m_regInput->clear();
    m_examCodeInput->clear();
    m_statusLabel->setText("");
    m_codeInput->clear();
    m_codeStatusLabel->setText("");
    m_stack->setCurrentIndex(0);
    setLoading(false);
}

void BioPage::setLoading(bool loading, const QString& msg) {
    m_startBtn->setEnabled(!loading);
    m_resumeBtn->setEnabled(!loading);
    m_retakeBtn->setEnabled(!loading);
    m_exitBtn->setEnabled(!loading);
    m_confirmCodeBtn->setEnabled(!loading);
    if (loading && !msg.isEmpty()) {
        m_statusLabel->setStyleSheet("color:#8a8faa; font-size:13px;");
        m_statusLabel->setText(msg);
    }
}

void BioPage::setError(const QString& error) {
    setLoading(false);
    m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
    m_statusLabel->setText(error);
    m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
    m_codeStatusLabel->setText(error);
}

void BioPage::showResumeCodeInput() {
    m_isResumeMode = true;
    m_codeFormTitle->setText("Resume Exam — Supervisor Code");
    m_codeInput->clear();
    m_codeStatusLabel->setText("");
    m_stack->setCurrentIndex(1);
    m_codeInput->setFocus();
}

void BioPage::showRetakeCodeInput() {
    m_isResumeMode = false;
    m_codeFormTitle->setText("Retake Exam — Supervisor Code");
    m_codeInput->clear();
    m_codeStatusLabel->setText("");
    m_stack->setCurrentIndex(1);
    m_codeInput->setFocus();
}

void BioPage::onStartClicked() {
    QString reg  = m_regInput->text().trimmed();
    QString code = m_examCodeInput->text().trimmed();
    if (reg.isEmpty() || code.isEmpty()) {
        m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_statusLabel->setText("Please fill in all fields.");
        return;
    }
    m_statusLabel->setText("");
    emit startExamRequested(reg, code);
}

void BioPage::onResumeClicked() {
    QString reg  = m_regInput->text().trimmed();
    QString code = m_examCodeInput->text().trimmed();
    if (reg.isEmpty() || code.isEmpty()) {
        m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_statusLabel->setText("Fill in registration and exam code first.");
        return;
    }
    showResumeCodeInput();
}

void BioPage::onRetakeClicked() {
    QString reg  = m_regInput->text().trimmed();
    QString code = m_examCodeInput->text().trimmed();
    if (reg.isEmpty() || code.isEmpty()) {
        m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_statusLabel->setText("Fill in registration and exam code first.");
        return;
    }
    showRetakeCodeInput();
}

void BioPage::onConfirmResume() {
    QString supCode = m_codeInput->text().trimmed();
    if (supCode.isEmpty()) {
        m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_codeStatusLabel->setText("Please enter the supervisor code.");
        return;
    }
    emit resumeExamRequested(
        m_regInput->text().trimmed(),
        m_examCodeInput->text().trimmed(),
        supCode
    );
}

void BioPage::onConfirmRetake() {
    QString supCode = m_codeInput->text().trimmed();
    if (supCode.isEmpty()) {
        m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_codeStatusLabel->setText("Please enter the supervisor code.");
        return;
    }
    emit retakeExamRequested(
        m_regInput->text().trimmed(),
        m_examCodeInput->text().trimmed(),
        supCode
    );
}

void BioPage::onCancelExtra() {
    m_codeInput->clear();
    m_codeStatusLabel->setText("");
    m_stack->setCurrentIndex(0);
}






// #include "biopage.h"
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QGraphicsDropShadowEffect>
// #include <QFrame>

// static const QString CARD_BASE_STYLE = R"(
//     QWidget#card {
//         background: #1c1e2a;
//         border: 1px solid #2e3050;
//         border-radius: 20px;
//     }
//     QLabel.sectionTitle {
//         color: #f0f0f0;
//         font-size: 20px;
//         font-weight: bold;
//     }
//     QLabel.fieldLabel {
//         color: #8a8faa;
//         font-size: 12px;
//         font-weight: 600;
//         letter-spacing: 0.5px;
//     }
//     QLineEdit {
//         background: #12141e;
//         border: 2px solid #2e3050;
//         border-radius: 8px;
//         color: #f0f0f0;
//         font-size: 15px;
//         padding: 0 14px;
//         height: 48px;
//     }
//     QLineEdit:focus { border-color: #5b6af0; }
//     QPushButton#startBtn {
//         background: #5b6af0;
//         color: white;
//         border: none;
//         border-radius: 8px;
//         font-size: 15px;
//         font-weight: bold;
//         height: 50px;
//     }
//     QPushButton#startBtn:hover { background: #6b7aff; }
//     QPushButton#startBtn:disabled { background: #2e3050; color:#555; }
//     QPushButton#resumeBtn {
//         background: #1a3a2a;
//         color: #2ecc71;
//         border: 1px solid #2ecc71;
//         border-radius: 8px;
//         font-size: 13px;
//         font-weight: bold;
//         height: 44px;
//     }
//     QPushButton#resumeBtn:hover { background: #2a4a3a; }
//     QPushButton#retakeBtn {
//         background: #3a2e1a;
//         color: #f39c12;
//         border: 1px solid #f39c12;
//         border-radius: 8px;
//         font-size: 13px;
//         font-weight: bold;
//         height: 44px;
//     }
//     QPushButton#retakeBtn:hover { background: #4a3e2a; }
//     QPushButton#confirmCodeBtn {
//         background: #5b6af0;
//         color: white;
//         border: none;
//         border-radius: 8px;
//         font-size: 14px;
//         font-weight: bold;
//         height: 48px;
//     }
//     QPushButton#confirmCodeBtn:hover { background: #6b7aff; }
//     QPushButton#cancelCodeBtn {
//         background: #2e3050;
//         color: #aaa;
//         border: none;
//         border-radius: 8px;
//         font-size: 14px;
//         height: 48px;
//     }
//     QPushButton#cancelCodeBtn:hover { background: #3e4060; color: #fff; }
//     QLabel#statusLabel, QLabel#codeStatusLabel {
//         color: #e74c3c;
//         font-size: 13px;
//         min-height: 18px;
//     }
//     QFrame#hline { color: #2e3050; }
// )";

// BioPage::BioPage(QWidget* parent) : QWidget(parent) {
//     setStyleSheet("BioPage { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
//                   "stop:0 #0a0c14, stop:0.5 #0f1120, stop:1 #0a0c14); }");

//     auto* outerLay = new QVBoxLayout(this);
//     outerLay->setAlignment(Qt::AlignCenter);

//     m_stack = new QStackedWidget(this);
//     m_stack->setFixedWidth(540);
//     m_stack->addWidget(buildMainForm());
//     m_stack->addWidget(buildCodeForm("Resume Exam", "Enter supervisor resume code", true));

//     outerLay->addWidget(m_stack);
// }

// QWidget* BioPage::buildMainForm() {
//     auto* card = new QWidget();
//     card->setObjectName("card");
//     card->setStyleSheet(CARD_BASE_STYLE);

//     auto* shadow = new QGraphicsDropShadowEffect(card);
//     shadow->setBlurRadius(60);
//     shadow->setColor(QColor(0,0,0,200));
//     shadow->setOffset(0,12);
//     card->setGraphicsEffect(shadow);

//     auto* lay = new QVBoxLayout(card);
//     lay->setContentsMargins(44, 40, 44, 40);
//     lay->setSpacing(0);

//     // Header
//     auto* icon = new QLabel("👤", card);
//     icon->setAlignment(Qt::AlignCenter);
//     icon->setStyleSheet("font-size:40px;");

//     auto* title = new QLabel("Student Identification", card);
//     title->setAlignment(Qt::AlignCenter);
//     title->setProperty("class", "sectionTitle");
//     title->setStyleSheet("color:#f0f0f0; font-size:20px; font-weight:bold;");

//     auto* sub = new QLabel("Enter your details to access your examination", card);
//     sub->setAlignment(Qt::AlignCenter);
//     sub->setStyleSheet("color:#8a8faa; font-size:13px;");

//     lay->addWidget(icon);
//     lay->addSpacing(8);
//     lay->addWidget(title);
//     lay->addSpacing(4);
//     lay->addWidget(sub);
//     lay->addSpacing(28);

//     auto* divider = new QFrame(card);
//     divider->setFrameShape(QFrame::HLine);
//     divider->setObjectName("hline");
//     lay->addWidget(divider);
//     lay->addSpacing(24);

//     // Reg number
//     auto* regLabel = new QLabel("REGISTRATION NUMBER", card);
//     regLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");
//     m_regInput = new QLineEdit(card);
//     m_regInput->setPlaceholderText("e.g. CSC/2021/001");
//     m_regInput->setFixedHeight(48);

//     // Exam code
//     auto* examLabel = new QLabel("EXAMINATION CODE", card);
//     examLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");
//     m_examCodeInput = new QLineEdit(card);
//     m_examCodeInput->setPlaceholderText("e.g. CSC301-2024");
//     m_examCodeInput->setFixedHeight(48);

//     lay->addWidget(regLabel);
//     lay->addSpacing(6);
//     lay->addWidget(m_regInput);
//     lay->addSpacing(16);
//     lay->addWidget(examLabel);
//     lay->addSpacing(6);
//     lay->addWidget(m_examCodeInput);
//     lay->addSpacing(6);

//     m_statusLabel = new QLabel("", card);
//     m_statusLabel->setObjectName("statusLabel");
//     m_statusLabel->setAlignment(Qt::AlignCenter);
//     lay->addWidget(m_statusLabel);
//     lay->addSpacing(20);

//     // Start button
//     m_startBtn = new QPushButton("▶  Start Examination", card);
//     m_startBtn->setObjectName("startBtn");
//     m_startBtn->setFixedHeight(50);
//     m_startBtn->setCursor(Qt::PointingHandCursor);
//     lay->addWidget(m_startBtn);
//     lay->addSpacing(12);

//     // Resume / Retake row
//     auto* actionRow = new QHBoxLayout;
//     actionRow->setSpacing(10);
//     m_resumeBtn = new QPushButton("▶  Resume Exam", card);
//     m_resumeBtn->setObjectName("resumeBtn");
//     m_resumeBtn->setCursor(Qt::PointingHandCursor);

//     m_retakeBtn = new QPushButton("↺  Retake Exam", card);
//     m_retakeBtn->setObjectName("retakeBtn");
//     m_retakeBtn->setCursor(Qt::PointingHandCursor);

//     actionRow->addWidget(m_resumeBtn);
//     actionRow->addWidget(m_retakeBtn);
//     lay->addLayout(actionRow);

//     connect(m_startBtn,  &QPushButton::clicked, this, &BioPage::onStartClicked);
//     connect(m_resumeBtn, &QPushButton::clicked, this, &BioPage::onResumeClicked);
//     connect(m_retakeBtn, &QPushButton::clicked, this, &BioPage::onRetakeClicked);
//     connect(m_examCodeInput, &QLineEdit::returnPressed, this, &BioPage::onStartClicked);

//     return card;
// }

// QWidget* BioPage::buildCodeForm(const QString& /*actionLabel*/, const QString& /*placeholder*/, bool /*isResume*/) {
//     auto* card = new QWidget();
//     card->setObjectName("card");
//     card->setStyleSheet(CARD_BASE_STYLE);

//     auto* shadow = new QGraphicsDropShadowEffect(card);
//     shadow->setBlurRadius(60);
//     shadow->setColor(QColor(0,0,0,200));
//     shadow->setOffset(0,12);
//     card->setGraphicsEffect(shadow);

//     auto* lay = new QVBoxLayout(card);
//     lay->setContentsMargins(44, 40, 44, 40);
//     lay->setSpacing(0);

//     auto* icon = new QLabel("🔑", card);
//     icon->setAlignment(Qt::AlignCenter);
//     icon->setStyleSheet("font-size:40px;");

//     m_codeFormTitle = new QLabel("Supervisor Code Required", card);
//     m_codeFormTitle->setAlignment(Qt::AlignCenter);
//     m_codeFormTitle->setStyleSheet("color:#f0f0f0; font-size:20px; font-weight:bold;");

//     auto* desc = new QLabel("Please ask your supervisor for the authorisation code.", card);
//     desc->setAlignment(Qt::AlignCenter);
//     desc->setWordWrap(true);
//     desc->setStyleSheet("color:#8a8faa; font-size:13px;");

//     auto* divider = new QFrame(card);
//     divider->setFrameShape(QFrame::HLine);
//     divider->setObjectName("hline");

//     auto* codeLabel = new QLabel("SUPERVISOR CODE", card);
//     codeLabel->setStyleSheet("color:#8a8faa; font-size:11px; font-weight:600; letter-spacing:1px;");

//     m_codeInput = new QLineEdit(card);
//     m_codeInput->setEchoMode(QLineEdit::Password);
//     m_codeInput->setPlaceholderText("Enter code...");
//     m_codeInput->setFixedHeight(48);
//     m_codeInput->setAlignment(Qt::AlignCenter);
//     m_codeInput->setStyleSheet("font-size:20px; letter-spacing:6px; font-family: monospace;");

//     m_codeStatusLabel = new QLabel("", card);
//     m_codeStatusLabel->setObjectName("codeStatusLabel");
//     m_codeStatusLabel->setAlignment(Qt::AlignCenter);

//     m_confirmCodeBtn = new QPushButton("Confirm", card);
//     m_confirmCodeBtn->setObjectName("confirmCodeBtn");
//     m_confirmCodeBtn->setFixedHeight(48);
//     m_confirmCodeBtn->setCursor(Qt::PointingHandCursor);

//     m_cancelCodeBtn = new QPushButton("← Back", card);
//     m_cancelCodeBtn->setObjectName("cancelCodeBtn");
//     m_cancelCodeBtn->setFixedHeight(48);
//     m_cancelCodeBtn->setCursor(Qt::PointingHandCursor);

//     lay->addWidget(icon);
//     lay->addSpacing(10);
//     lay->addWidget(m_codeFormTitle);
//     lay->addSpacing(6);
//     lay->addWidget(desc);
//     lay->addSpacing(24);
//     lay->addWidget(divider);
//     lay->addSpacing(24);
//     lay->addWidget(codeLabel);
//     lay->addSpacing(8);
//     lay->addWidget(m_codeInput);
//     lay->addSpacing(8);
//     lay->addWidget(m_codeStatusLabel);
//     lay->addSpacing(16);
//     lay->addWidget(m_confirmCodeBtn);
//     lay->addSpacing(10);
//     lay->addWidget(m_cancelCodeBtn);

//     connect(m_confirmCodeBtn, &QPushButton::clicked, this, [this]() {
//         if (m_isResumeMode) onConfirmResume();
//         else onConfirmRetake();
//     });
//     connect(m_cancelCodeBtn, &QPushButton::clicked, this, &BioPage::onCancelExtra);
//     connect(m_codeInput, &QLineEdit::returnPressed, this, [this]() {
//         if (m_isResumeMode) onConfirmResume();
//         else onConfirmRetake();
//     });

//     return card;
// }

// void BioPage::reset() {
//     m_regInput->clear();
//     m_examCodeInput->clear();
//     m_statusLabel->setText("");
//     m_codeInput->clear();
//     m_codeStatusLabel->setText("");
//     m_stack->setCurrentIndex(0);
//     setLoading(false);
// }

// void BioPage::setLoading(bool loading, const QString& msg) {
//     m_startBtn->setEnabled(!loading);
//     m_resumeBtn->setEnabled(!loading);
//     m_retakeBtn->setEnabled(!loading);
//     m_confirmCodeBtn->setEnabled(!loading);
//     if (loading && !msg.isEmpty()) {
//         m_statusLabel->setStyleSheet("color:#8a8faa; font-size:13px;");
//         m_statusLabel->setText(msg);
//     }
// }

// void BioPage::setError(const QString& error) {
//     setLoading(false);
//     m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//     m_statusLabel->setText(error);
//     m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//     m_codeStatusLabel->setText(error);
// }

// void BioPage::showResumeCodeInput() {
//     m_isResumeMode = true;
//     m_codeFormTitle->setText("Resume Exam — Supervisor Code");
//     m_codeInput->clear();
//     m_codeStatusLabel->setText("");
//     m_stack->setCurrentIndex(1);
//     m_codeInput->setFocus();
// }

// void BioPage::showRetakeCodeInput() {
//     m_isResumeMode = false;
//     m_codeFormTitle->setText("Retake Exam — Supervisor Code");
//     m_codeInput->clear();
//     m_codeStatusLabel->setText("");
//     m_stack->setCurrentIndex(1);
//     m_codeInput->setFocus();
// }

// void BioPage::onStartClicked() {
//     QString reg  = m_regInput->text().trimmed();
//     QString code = m_examCodeInput->text().trimmed();
//     if (reg.isEmpty() || code.isEmpty()) {
//         m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//         m_statusLabel->setText("Please fill in all fields.");
//         return;
//     }
//     m_statusLabel->setText("");
//     emit startExamRequested(reg, code);
// }

// void BioPage::onResumeClicked() {
//     QString reg  = m_regInput->text().trimmed();
//     QString code = m_examCodeInput->text().trimmed();
//     if (reg.isEmpty() || code.isEmpty()) {
//         m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//         m_statusLabel->setText("Fill in registration and exam code first.");
//         return;
//     }
//     showResumeCodeInput();
// }

// void BioPage::onRetakeClicked() {
//     QString reg  = m_regInput->text().trimmed();
//     QString code = m_examCodeInput->text().trimmed();
//     if (reg.isEmpty() || code.isEmpty()) {
//         m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//         m_statusLabel->setText("Fill in registration and exam code first.");
//         return;
//     }
//     showRetakeCodeInput();
// }

// void BioPage::onConfirmResume() {
//     QString supCode = m_codeInput->text().trimmed();
//     if (supCode.isEmpty()) {
//         m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//         m_codeStatusLabel->setText("Please enter the supervisor code.");
//         return;
//     }
//     emit resumeExamRequested(
//         m_regInput->text().trimmed(),
//         m_examCodeInput->text().trimmed(),
//         supCode
//     );
// }

// void BioPage::onConfirmRetake() {
//     QString supCode = m_codeInput->text().trimmed();
//     if (supCode.isEmpty()) {
//         m_codeStatusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
//         m_codeStatusLabel->setText("Please enter the supervisor code.");
//         return;
//     }
//     emit retakeExamRequested(
//         m_regInput->text().trimmed(),
//         m_examCodeInput->text().trimmed(),
//         supCode
//     );
// }

// void BioPage::onCancelExtra() {
//     m_codeInput->clear();
//     m_codeStatusLabel->setText("");
//     m_stack->setCurrentIndex(0);
// }
