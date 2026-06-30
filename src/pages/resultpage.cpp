#include "resultpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

ResultPage::ResultPage(QWidget* parent) : QWidget(parent) {
    setStyleSheet("ResultPage { background: qlineargradient(x1:0,y1:0,x2:1,y2:1,"
                  "stop:0 #0a0c14, stop:0.5 #0f1120, stop:1 #0a0c14); }");

    auto* outerLay = new QVBoxLayout(this);
    outerLay->setAlignment(Qt::AlignCenter);

    auto* card = new QWidget(this);
    card->setObjectName("resultCard");
    card->setFixedWidth(560);

    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(60);
    shadow->setColor(QColor(0,0,0,200));
    shadow->setOffset(0,12);
    card->setGraphicsEffect(shadow);

    auto* lay = new QVBoxLayout(card);
    lay->setContentsMargins(48, 44, 48, 44);
    lay->setSpacing(0);

    auto* completedIcon = new QLabel("✅", card);
    completedIcon->setAlignment(Qt::AlignCenter);
    completedIcon->setStyleSheet("font-size:48px;");

    auto* titleLabel = new QLabel("Examination Complete", card);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color:#f0f0f0; font-size:22px; font-weight:bold;");

    lay->addWidget(completedIcon);
    lay->addSpacing(8);
    lay->addWidget(titleLabel);
    lay->addSpacing(24);

    auto mkDiv = [&]() {
        auto* d = new QFrame(card); d->setFrameShape(QFrame::HLine);
        d->setStyleSheet("color:#2e3050;"); return d;
    };
    lay->addWidget(mkDiv());
    lay->addSpacing(20);

    m_candidateLabel = new QLabel("", card);
    m_candidateLabel->setAlignment(Qt::AlignCenter);
    m_candidateLabel->setStyleSheet("color:#f0f0f0; font-size:16px; font-weight:bold;");
    m_courseLabel = new QLabel("", card);
    m_courseLabel->setAlignment(Qt::AlignCenter);
    m_courseLabel->setStyleSheet("color:#5b6af0; font-size:13px;");
    lay->addWidget(m_candidateLabel);
    lay->addSpacing(4);
    lay->addWidget(m_courseLabel);
    lay->addSpacing(24);

    // Score circle
    m_scoreCircleWidget = new QWidget(card);
    m_scoreCircleWidget->setFixedSize(164, 164);
    m_scoreCircleWidget->setObjectName("scoreCircle");
    auto* circleLay = new QVBoxLayout(m_scoreCircleWidget);
    circleLay->setAlignment(Qt::AlignCenter);
    m_gradeLabel = new QLabel("—", m_scoreCircleWidget);
    m_gradeLabel->setAlignment(Qt::AlignCenter);
    m_gradeLabel->setStyleSheet("color:#4ecca3; font-size:52px; font-weight:bold;");
    m_scoreLabel = new QLabel("", m_scoreCircleWidget);
    m_scoreLabel->setAlignment(Qt::AlignCenter);
    m_scoreLabel->setStyleSheet("color:#8a8faa; font-size:12px;");
    circleLay->addWidget(m_gradeLabel);
    circleLay->addWidget(m_scoreLabel);

    auto* circleRow = new QHBoxLayout;
    circleRow->setAlignment(Qt::AlignCenter);
    circleRow->addWidget(m_scoreCircleWidget);
    lay->addLayout(circleRow);
    lay->addSpacing(10);

    m_statusLabel = new QLabel("", card);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    m_statusLabel->setStyleSheet("font-size:16px; font-weight:bold;");
    lay->addWidget(m_statusLabel);
    lay->addSpacing(24);
    lay->addWidget(mkDiv());
    lay->addSpacing(20);

    // Stats row
    auto* statsRow = new QHBoxLayout;
    auto makeStat = [&](QLabel*& valLbl, const QString& hdr) -> QWidget* {
        auto* w = new QWidget(card);
        auto* wl = new QVBoxLayout(w);
        wl->setAlignment(Qt::AlignCenter);
        wl->setContentsMargins(0,0,0,0);
        valLbl = new QLabel("—", w);
        valLbl->setAlignment(Qt::AlignCenter);
        valLbl->setStyleSheet("color:#4ecca3; font-size:18px; font-weight:bold;");
        auto* hl = new QLabel(hdr, w);
        hl->setAlignment(Qt::AlignCenter);
        hl->setStyleSheet("color:#8a8faa; font-size:11px; letter-spacing:0.5px;");
        wl->addWidget(valLbl);
        wl->addWidget(hl);
        return w;
    };

    auto mkVLine = [&]() {
        auto* f = new QFrame(card); f->setFrameShape(QFrame::VLine);
        f->setStyleSheet("color:#2e3050;"); return f;
    };

    statsRow->addWidget(makeStat(m_answeredLabel, "ANSWERED"));
    statsRow->addWidget(mkVLine());
    statsRow->addWidget(makeStat(m_timeLabel,     "TIME USED"));
    statsRow->addWidget(mkVLine());
    statsRow->addWidget(makeStat(m_percentLabel,  "SCORE %"));
    lay->addLayout(statsRow);
    lay->addSpacing(28);

    m_newExamBtn = new QPushButton("Start New Exam / Next Candidate  →", card);
    m_newExamBtn->setFixedHeight(52);
    m_newExamBtn->setCursor(Qt::PointingHandCursor);
    m_newExamBtn->setStyleSheet(R"(
        QPushButton {
            background:#5b6af0; color:white; border:none;
            border-radius:10px; font-size:14px; font-weight:bold;
        }
        QPushButton:hover { background:#6b7aff; }
    )");
    lay->addWidget(m_newExamBtn);

    card->setStyleSheet(R"(
        QWidget#resultCard {
            background:#1c1e2a; border:1px solid #2e3050; border-radius:20px;
        }
        QWidget#scoreCircle {
            background:#12141e; border:3px solid #4ecca3; border-radius:82px;
        }
    )");

    outerLay->addWidget(card);
    connect(m_newExamBtn, &QPushButton::clicked, this, &ResultPage::newExamRequested);
}

void ResultPage::showResult(const ExamSession& session, const QJsonObject& resultData) {
    m_candidateLabel->setText(session.candidateName);
    m_courseLabel->setText(session.courseName + "  ·  " + session.examCode);

    int score     = resultData["score"].toInt();
    int total     = resultData["total_marks"].toInt(session.totalQuestions);
    QString grade = resultData["grade"].toString("N/A");
    int pct       = total > 0 ? (score * 100 / total) : 0;
    bool passed   = resultData["passed"].toBool(pct >= 50);
    int timeUsed  = resultData["seconds_used"].toInt(0);

    m_gradeLabel->setText(grade);
    m_scoreLabel->setText(QString("%1 / %2").arg(score).arg(total));

    QString gc = pct >= 70 ? "#4ecca3" : pct >= 50 ? "#f39c12" : "#e74c3c";
    m_gradeLabel->setStyleSheet(QString("color:%1; font-size:52px; font-weight:bold;").arg(gc));
    m_scoreCircleWidget->setStyleSheet(
        QString("QWidget#scoreCircle { background:#12141e; border:3px solid %1;"
                " border-radius:82px; }").arg(gc));

    m_statusLabel->setText(passed ? "✓  PASSED" : "✗  FAILED");
    m_statusLabel->setStyleSheet(
        QString("font-size:16px; font-weight:bold; color:%1;")
            .arg(passed ? "#2ecc71" : "#e74c3c"));

    m_answeredLabel->setText(QString::number(session.answeredCount()));
    m_percentLabel->setText(QString("%1%").arg(pct));

    int h = timeUsed / 3600, m = (timeUsed % 3600) / 60, s = timeUsed % 60;
    m_timeLabel->setText(h > 0
        ? QString("%1h %2m").arg(h).arg(m)
        : QString("%1m %2s").arg(m).arg(s));
}
