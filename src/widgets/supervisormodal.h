#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

enum class SupervisorAction { Exit, Resume, Retake };

class SupervisorModal : public QDialog {
    Q_OBJECT
public:
    explicit SupervisorModal(SupervisorAction action, QWidget* parent = nullptr);
    QString enteredCode() const;

signals:
    void codeAccepted(const QString& code);

private:
    QLineEdit* m_codeInput;
    SupervisorAction m_action;
    void onConfirm();
};
