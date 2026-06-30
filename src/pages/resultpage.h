#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QJsonObject>
#include "../models/examsession.h"

class ResultPage : public QWidget {
    Q_OBJECT
public:
    explicit ResultPage(QWidget* parent = nullptr);
    void showResult(const ExamSession& session, const QJsonObject& resultData);

signals:
    void newExamRequested();

private:
    QWidget*     m_scoreCircleWidget;
    QLabel*      m_gradeLabel;
    QLabel*      m_scoreLabel;
    QLabel*      m_candidateLabel;
    QLabel*      m_courseLabel;
    QLabel*      m_answeredLabel;
    QLabel*      m_timeLabel;
    QLabel*      m_percentLabel;
    QLabel*      m_statusLabel;
    QPushButton* m_newExamBtn;
};
