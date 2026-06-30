#pragma once
#include "question.h"
#include <QString>
#include <QMap>
#include <QDateTime>

struct ExamSession {
    QString serverUrl;
    QString regNumber;
    QString examCode;
    QString candidateName;
    QString courseName;
    int durationSeconds = 0;
    int totalQuestions  = 0;
    int requiredQuestions = 0;  // minimum to answer before submit

    QList<Question> questions;
    QMap<int, QString> answers;   // questionId -> selected answer
    QMap<int, bool> flagged;      // questionId -> flagged for review

    QString resumeToken;          // populated if resumed
    QString sessionToken;         // server session token
    bool isResumed = false;

    QDateTime startTime;
    int secondsElapsed = 0;       // for resume

    int score = 0;
    int totalMarks = 0;
    QString grade;
    bool submitted = false;

    int answeredCount() const {
        return answers.size();
    }

    bool canSubmit() const {
        return answeredCount() >= requiredQuestions;
    }
};
