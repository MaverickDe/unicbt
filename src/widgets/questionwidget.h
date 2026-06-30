#pragma once
#include <QWidget>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include "../models/question.h"

class QuestionWidget : public QWidget {
    Q_OBJECT
public:
    explicit QuestionWidget(QWidget* parent = nullptr);
    void loadQuestion(const Question& q, int number, int total, const QString& existingAnswer = "");

    QString currentAnswer() const;
    int questionId() const { return m_questionId; }

signals:
    void answerChanged(int questionId, const QString& answer);
    void flagToggled(int questionId, bool flagged);

private:
    void buildMCQ(const Question& q, const QString& existing);
    void buildTrueFalse(const QString& existing);
    void buildShortAnswer(const QString& existing);

    QLabel*       m_questionText;
    QLabel*       m_progressLabel;
    QLabel*       m_marksLabel;
    QButtonGroup* m_btnGroup;
    QLineEdit*    m_textInput;
    QPushButton*  m_flagBtn;
    QWidget*      m_optionsContainer;

    int  m_questionId = -1;
    bool m_flagged    = false;
};
