#include "onboardpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

OnboardPage::OnboardPage(QWidget* parent) : QWidget(parent) {
    setStyleSheet(R"(
        OnboardPage {
            background: qlineargradient(x1:0,y1:0,x2:1,y2:1,
                stop:0 #0a0c14, stop:0.5 #0f1120, stop:1 #0a0c14);
        }
    )");

    auto* outerLay = new QVBoxLayout(this);
    outerLay->setAlignment(Qt::AlignCenter);

    auto* card = new QWidget(this);
    card->setObjectName("card");
    card->setFixedWidth(520);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(60);
    shadow->setColor(QColor(0, 0, 0, 200));
    shadow->setOffset(0, 12);
    card->setGraphicsEffect(shadow);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(48, 44, 48, 44);
    lay->setSpacing(0);

    // ── Logo area ────────────────────────────────────────────────────────
    auto* logoArea = new QWidget(card);
    auto* logoLay  = new QVBoxLayout(logoArea);
    logoLay->setAlignment(Qt::AlignCenter);
    logoLay->setContentsMargins(0, 0, 0, 0);

    auto* logoIcon = new QLabel("🎓", card);
    logoIcon->setAlignment(Qt::AlignCenter);
    logoIcon->setStyleSheet("font-size:52px;");

    auto* appTitle = new QLabel("CBT Examination Portal", card);
    appTitle->setAlignment(Qt::AlignCenter);
    appTitle->setObjectName("appTitle");

    auto* appSub = new QLabel("Secure Computer-Based Testing System", card);
    appSub->setAlignment(Qt::AlignCenter);
    appSub->setObjectName("appSub");

    logoLay->addWidget(logoIcon);
    logoLay->addSpacing(10);
    logoLay->addWidget(appTitle);
    logoLay->addSpacing(4);
    logoLay->addWidget(appSub);

    // ── Divider ──────────────────────────────────────────────────────────
    auto* divider = new QFrame(card);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("divider");

    // ── Form label ───────────────────────────────────────────────────────
    auto* formLabel = new QLabel("Enter Examination Server URL", card);
    formLabel->setObjectName("formLabel");

    // ── URL input row ────────────────────────────────────────────────────
    auto* inputRow = new QHBoxLayout;
    inputRow->setSpacing(10);

    m_urlInput = new QLineEdit(card);
    m_urlInput->setPlaceholderText("http://192.168.1.1:8080");
    m_urlInput->setObjectName("urlInput");
    m_urlInput->setFixedHeight(48);

    m_proceedBtn = new QPushButton("Connect →", card);
    m_proceedBtn->setObjectName("proceedBtn");
    m_proceedBtn->setFixedHeight(48);
    m_proceedBtn->setCursor(Qt::PointingHandCursor);

    inputRow->addWidget(m_urlInput, 1);
    inputRow->addWidget(m_proceedBtn);

    // ── Status label ─────────────────────────────────────────────────────
    m_statusLabel = new QLabel("", card);
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setWordWrap(true);

    // ── Info banner ──────────────────────────────────────────────────────
    auto* infoBanner = new QWidget(card);
    infoBanner->setObjectName("infoBanner");
    auto* bannerLay = new QHBoxLayout(infoBanner);
    bannerLay->setContentsMargins(14, 10, 14, 10);

    auto* bannerIcon = new QLabel("ℹ", infoBanner);
    bannerIcon->setStyleSheet("color:#5b6af0; font-size:16px;");

    auto* bannerText = new QLabel(
        "This application will enter fullscreen lockdown mode when your exam begins. "
        "Ensure you are ready before proceeding.",
        infoBanner);
    bannerText->setWordWrap(true);
    bannerText->setObjectName("bannerText");

    bannerLay->addWidget(bannerIcon, 0, Qt::AlignTop);
    bannerLay->addWidget(bannerText, 1);

    // ── Exit button ──────────────────────────────────────────────────────
    m_exitBtn = new QPushButton("Exit Application", card);
    m_exitBtn->setObjectName("exitBtn");
    m_exitBtn->setFixedHeight(40);
    m_exitBtn->setCursor(Qt::PointingHandCursor);

    // ── Assemble layout ──────────────────────────────────────────────────
    lay->addWidget(logoArea);
    lay->addSpacing(28);
    lay->addWidget(divider);
    lay->addSpacing(24);
    lay->addWidget(formLabel);
    lay->addSpacing(10);
    lay->addLayout(inputRow);
    lay->addSpacing(10);
    lay->addWidget(m_statusLabel);
    lay->addSpacing(16);
    lay->addWidget(infoBanner);
    lay->addSpacing(16);
    lay->addWidget(m_exitBtn);

    outerLay->addWidget(card);

    // ── Stylesheet ───────────────────────────────────────────────────────
    card->setStyleSheet(R"(
        QWidget#card {
            background: #1c1e2a;
            border: 1px solid #2e3050;
            border-radius: 20px;
        }
        QLabel#appTitle {
            color: #f0f0f0;
            font-size: 22px;
            font-weight: bold;
        }
        QLabel#appSub {
            color: #5b6af0;
            font-size: 13px;
            letter-spacing: 1px;
        }
        QFrame#divider { color: #2e3050; }
        QLabel#formLabel {
            color: #8a8faa;
            font-size: 13px;
            font-weight: 600;
            letter-spacing: 0.5px;
        }
        QLineEdit#urlInput {
            background: #12141e;
            border: 2px solid #2e3050;
            border-radius: 8px;
            color: #f0f0f0;
            font-size: 14px;
            padding: 0 14px;
            font-family: 'Courier New', monospace;
        }
        QLineEdit#urlInput:focus { border-color: #5b6af0; }
        QPushButton#proceedBtn {
            background: #5b6af0;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
            padding: 0 22px;
            min-width: 110px;
        }
        QPushButton#proceedBtn:hover    { background: #6b7aff; }
        QPushButton#proceedBtn:disabled { background: #2e3050; color: #555; }
        QLabel#statusLabel {
            color: #e74c3c;
            font-size: 13px;
            min-height: 18px;
        }
        QWidget#infoBanner {
            background: #12141e;
            border: 1px solid #2e3050;
            border-left: 3px solid #5b6af0;
            border-radius: 8px;
        }
        QLabel#bannerText { color: #8a8faa; font-size: 12px; }
        QPushButton#exitBtn {
            background: transparent;
            color: #8a8faa;
            border: 1px solid #2e3050;
            border-radius: 8px;
            font-size: 13px;
        }
        QPushButton#exitBtn:hover {
            color: #e74c3c;
            border-color: #e74c3c;
            background: #2a1a1a;
        }
    )");

    // ── Connections ──────────────────────────────────────────────────────
    connect(m_proceedBtn, &QPushButton::clicked,    this, &OnboardPage::onProceedClicked);
    connect(m_urlInput,   &QLineEdit::returnPressed, this, &OnboardPage::onProceedClicked);
    connect(m_exitBtn,    &QPushButton::clicked,    this, &OnboardPage::exitRequested);
}

// ── Public slots ─────────────────────────────────────────────────────────────

void OnboardPage::reset() {
    m_urlInput->clear();
    m_statusLabel->setText("");
    setConnecting(false);
}

void OnboardPage::onPingError(const QString& error) {
    setConnecting(false);
    m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
    m_statusLabel->setText("❌ Cannot reach server: " + error);
}

// ── Private slots ─────────────────────────────────────────────────────────────

void OnboardPage::onProceedClicked() {
    if (m_connecting) return;

    QString url = m_urlInput->text().trimmed();
    if (url.isEmpty()) {
        m_statusLabel->setStyleSheet("color:#e74c3c; font-size:13px;");
        m_statusLabel->setText("Please enter the server URL.");
        return;
    }
    // Auto-prepend http:// if missing
    if (!url.startsWith("http://") && !url.startsWith("https://")) {
        url = "http://" + url;
        m_urlInput->setText(url);
    }
    m_statusLabel->setStyleSheet("color:#8a8faa; font-size:13px;");
    m_statusLabel->setText("Connecting to server...");
    setConnecting(true);
    emit urlConfirmed(url);
}

// ── Private helpers ───────────────────────────────────────────────────────────

void OnboardPage::setConnecting(bool val) {
    m_connecting = val;
    m_proceedBtn->setEnabled(!val);
    m_proceedBtn->setText(val ? "Connecting..." : "Connect →");
    m_exitBtn->setEnabled(!val);  // also disable exit while connecting
}
