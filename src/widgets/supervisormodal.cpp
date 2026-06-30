#include "supervisormodal.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMessageBox>

SupervisorModal::SupervisorModal(SupervisorAction action, QWidget* parent)
    : QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint)
    , m_action(action)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(true);
    setFixedWidth(420);

    auto* container = new QWidget(this);
    container->setObjectName("container");

    auto* outerLay = new QVBoxLayout(this);
    outerLay->setContentsMargins(0,0,0,0);
    outerLay->addWidget(container);

    auto* lay = new QVBoxLayout(container);
    lay->setContentsMargins(32, 28, 32, 28);
    lay->setSpacing(16);

    // Icon
    auto* iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);

    // Title
    auto* title = new QLabel(this);
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("titleLabel");

    // Description
    auto* desc = new QLabel(this);
    desc->setWordWrap(true);
    desc->setAlignment(Qt::AlignCenter);
    desc->setObjectName("descLabel");

    QString iconText, titleText, descText;
    switch (action) {
        case SupervisorAction::Exit:
            iconText  = "🔒";
            titleText = "Supervisor Authorisation Required";
            descText  = "To exit the exam, a supervisor must provide the exit code.";
            break;
        case SupervisorAction::Resume:
            iconText  = "▶";
            titleText = "Resume Exam";
            descText  = "Enter the resume code provided by your supervisor to continue your exam.";
            break;
        case SupervisorAction::Retake:
            iconText  = "↺";
            titleText = "Retake Exam";
            descText  = "Enter the retake code provided by your supervisor to retake this exam.";
            break;
    }

    iconLabel->setText(iconText);
    iconLabel->setStyleSheet("font-size: 36px;");
    title->setText(titleText);
    desc->setText(descText);

    // Code input
    m_codeInput = new QLineEdit(this);
    m_codeInput->setPlaceholderText("Enter supervisor code...");
    m_codeInput->setEchoMode(QLineEdit::Password);
    m_codeInput->setAlignment(Qt::AlignCenter);
    m_codeInput->setObjectName("codeInput");

    // Buttons
    auto* btnRow = new QHBoxLayout;
    auto* cancelBtn = new QPushButton("Cancel", this);
    auto* confirmBtn = new QPushButton("Confirm", this);
    cancelBtn->setObjectName("cancelBtn");
    confirmBtn->setObjectName("confirmBtn");
    cancelBtn->setFixedHeight(42);
    confirmBtn->setFixedHeight(42);
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(confirmBtn);

    lay->addWidget(iconLabel);
    lay->addWidget(title);
    lay->addWidget(desc);
    lay->addSpacing(8);
    lay->addWidget(m_codeInput);
    lay->addSpacing(8);
    lay->addLayout(btnRow);

    // Shadow
    auto* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(40);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(0, 8);
    container->setGraphicsEffect(shadow);

    setStyleSheet(R"(
        QWidget#container {
            background: #1c1e2a;
            border: 1px solid #2e3050;
            border-radius: 16px;
        }
        QLabel#titleLabel {
            color: #f0f0f0;
            font-size: 16px;
            font-weight: bold;
        }
        QLabel#descLabel {
            color: #8a8faa;
            font-size: 13px;
        }
        QLineEdit#codeInput {
            background: #12141e;
            border: 2px solid #2e3050;
            border-radius: 8px;
            color: #f0f0f0;
            font-size: 18px;
            letter-spacing: 6px;
            padding: 10px;
            font-family: monospace;
        }
        QLineEdit#codeInput:focus {
            border-color: #5b6af0;
        }
        QPushButton#cancelBtn {
            background: #2e3050;
            color: #aaa;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton#cancelBtn:hover { background: #3e4060; color: #fff; }
        QPushButton#confirmBtn {
            background: #5b6af0;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton#confirmBtn:hover { background: #6b7aff; }
    )");

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, this, &SupervisorModal::onConfirm);
    connect(m_codeInput, &QLineEdit::returnPressed, this, &SupervisorModal::onConfirm);
}

QString SupervisorModal::enteredCode() const {
    return m_codeInput->text().trimmed();
}

void SupervisorModal::onConfirm() {
    QString code = m_codeInput->text().trimmed();
    if (code.isEmpty()) {
        m_codeInput->setStyleSheet(
            m_codeInput->styleSheet() + "QLineEdit { border-color: #e74c3c; }"
        );
        return;
    }
    emit codeAccepted(code);
    accept();
}
