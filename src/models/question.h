#pragma once
#include <QString>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

enum class QuestionType {
    MultipleChoice,
    TrueFalse,
    ShortAnswer,
    FillBlank
};

struct Question {
    int id;
    QString text;
    QuestionType type;
    QList<QString> options;   // for MCQ / TrueFalse
    QString correctAnswer;    // server-side only (not always sent to client)
    int marks;

    static Question fromJson(const QJsonObject& obj) {
        Question q;
        q.id   = obj["id"].toInt();
        q.text = obj["text"].toString();
        q.marks = obj["marks"].toInt(1);

        QString typeStr = obj["type"].toString("mcq");
        if (typeStr == "mcq")         q.type = QuestionType::MultipleChoice;
        else if (typeStr == "tf")     q.type = QuestionType::TrueFalse;
        else if (typeStr == "short")  q.type = QuestionType::ShortAnswer;
        else                          q.type = QuestionType::FillBlank;

        QJsonArray opts = obj["options"].toArray();
        for (const auto& o : opts)
            q.options.append(o.toString());

        return q;
    }
};
