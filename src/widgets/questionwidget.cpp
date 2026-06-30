#include "questionwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

QuestionWidget::QuestionWidget(QWidget* parent)
    : QWidget(parent)
    , m_btnGroup(new QButtonGroup(this))
    , m_textInput(nullptr)
{
    auto* mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->setSpacing(0);

    // Header bar
    auto* header = new QWidget(this);
    header->setObjectName("questionHeader");
    auto* headerLay = new QHBoxLayout(header);
    headerLay->setContentsMargins(24, 14, 24, 14);

    m_progressLabel = new QLabel(this);
    m_progressLabel->setObjectName("progressLabel");

    m_marksLabel = new QLabel(this);
    m_marksLabel->setObjectName("marksLabel");
    m_marksLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_flagBtn = new QPushButton("⚑ Flag for Review", this);
    m_flagBtn->setObjectName("flagBtn");
    m_flagBtn->setCheckable(true);
    m_flagBtn->setCursor(Qt::PointingHandCursor);

    headerLay->addWidget(m_progressLabel);
    headerLay->addStretch();
    headerLay->addWidget(m_flagBtn);
    headerLay->addSpacing(16);
    headerLay->addWidget(m_marksLabel);

    // Question body
    auto* body = new QWidget(this);
    body->setObjectName("questionBody");
    auto* bodyLay = new QVBoxLayout(body);
    bodyLay->setContentsMargins(32, 28, 32, 28);
    bodyLay->setSpacing(20);

    m_questionText = new QLabel(this);
    m_questionText->setObjectName("questionText");
    m_questionText->setWordWrap(true);
    m_questionText->setTextFormat(Qt::RichText);

    m_optionsContainer = new QWidget(this);
    m_optionsContainer->setObjectName("optionsContainer");

    bodyLay->addWidget(m_questionText);
    bodyLay->addWidget(m_optionsContainer);
    bodyLay->addStretch();

    mainLay->addWidget(header);
    mainLay->addWidget(body, 1);

    setStyleSheet(R"(
        QWidget#questionHeader {
            background: #1a1d2e;
            border-bottom: 1px solid #2e3050;
        }
        QLabel#progressLabel {
            color: #8a8faa;
            font-size: 13px;
            font-weight: 600;
        }
        QLabel#marksLabel {
            color: #4ecca3;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton#flagBtn {
            background: transparent;
            color: #8a8faa;
            border: 1px solid #2e3050;
            border-radius: 6px;
            padding: 6px 14px;
            font-size: 12px;
        }
        QPushButton#flagBtn:hover { color: #f39c12; border-color: #f39c12; }
        QPushButton#flagBtn:checked { background: #3a2e1a; color: #f39c12; border-color: #f39c12; }
        QWidget#questionBody { background: #12141e; }
        QLabel#questionText {
            color: #e8e8f0;
            font-size: 17px;
            line-height: 1.6;
        }
        QRadioButton {
            color: #c0c4d8;
            font-size: 15px;
            padding: 14px 18px;
            spacing: 12px;
            border: 1px solid transparent;
            border-radius: 8px;
        }
        QRadioButton:hover {
            background: #1c1e2a;
            border-color: #3e4060;
        }
        QRadioButton:checked {
            background: #1a2040;
            border-color: #5b6af0;
            color: #fff;
        }
        QRadioButton::indicator {
            width: 18px; height: 18px;
            border-radius: 9px;
            border: 2px solid #3e4060;
        }
        QRadioButton::indicator:checked {
            background: #5b6af0;
            border-color: #5b6af0;
        }
        QLineEdit {
            background: #1c1e2a;
            border: 2px solid #2e3050;
            border-radius: 8px;
            color: #f0f0f0;
            font-size: 15px;
            padding: 12px 16px;
        }
        QLineEdit:focus { border-color: #5b6af0; }
    )");

    connect(m_flagBtn, &QPushButton::toggled, this, [this](bool checked) {
        m_flagged = checked;
        emit flagToggled(m_questionId, checked);
    });
}

void QuestionWidget::loadQuestion(const Question& q, int number, int total, const QString& existingAnswer) {
    m_questionId = q.id;

    m_progressLabel->setText(QString("Question %1 of %2").arg(number).arg(total));
    m_marksLabel->setText(QString("%1 mark%2").arg(q.marks).arg(q.marks != 1 ? "s" : ""));
    m_questionText->setText(QString("<b>Q%1.</b> %2").arg(number).arg(q.text));

    // Clear old options
    delete m_optionsContainer->layout();
    for (auto* btn : m_btnGroup->buttons())
        m_btnGroup->removeButton(btn);
    qDeleteAll(m_optionsContainer->children());
    m_textInput = nullptr;

    auto* optLay = new QVBoxLayout(m_optionsContainer);
    optLay->setContentsMargins(0, 0, 0, 0);
    optLay->setSpacing(8);

    switch (q.type) {
        case QuestionType::MultipleChoice:
            buildMCQ(q, existingAnswer);
            break;
        case QuestionType::TrueFalse:
            buildTrueFalse(existingAnswer);
            break;
        default:
            buildShortAnswer(existingAnswer);
            break;
    }
}

void QuestionWidget::buildMCQ(const Question& q, const QString& existing) {
    auto* lay = m_optionsContainer->layout();
    QChar labels[] = {'A','B','C','D','E','F'};
    int i = 0;
    for (const auto& opt : q.options) {
        auto* rb = new QRadioButton(
            QString("%1.  %2").arg(labels[i]).arg(opt),
            m_optionsContainer
        );
        m_btnGroup->addButton(rb, i);
        lay->addWidget(rb);
        if (!existing.isEmpty() && existing == opt)
            rb->setChecked(true);
        connect(rb, &QRadioButton::clicked, this, [this, opt]() {
            emit answerChanged(m_questionId, opt);
        });
        i++;
    }
    static_cast<QVBoxLayout*>(lay)->addStretch();
}

void QuestionWidget::buildTrueFalse(const QString& existing) {
    auto* lay = m_optionsContainer->layout();
    for (const auto& opt : QStringList{"True", "False"}) {
        auto* rb = new QRadioButton(opt, m_optionsContainer);
        m_btnGroup->addButton(rb);
        lay->addWidget(rb);
        if (!existing.isEmpty() && existing == opt)
            rb->setChecked(true);
        connect(rb, &QRadioButton::clicked, this, [this, opt]() {
            emit answerChanged(m_questionId, opt);
        });
    }
    static_cast<QVBoxLayout*>(lay)->addStretch();
}

void QuestionWidget::buildShortAnswer(const QString& existing) {
    auto* lay = m_optionsContainer->layout();
    m_textInput = new QLineEdit(m_optionsContainer);
    m_textInput->setPlaceholderText("Type your answer here...");
    m_textInput->setText(existing);
    lay->addWidget(m_textInput);
    static_cast<QVBoxLayout*>(lay)->addStretch();
    connect(m_textInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        emit answerChanged(m_questionId, text);
    });
}

QString QuestionWidget::currentAnswer() const {
    if (m_textInput)
        return m_textInput->text().trimmed();
    auto* checked = m_btnGroup->checkedButton();
    if (checked) return checked->text().section("  ", 1); // strip "A.  "
    return {};
}
