#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include "../models/examsession.h"

class ApiClient : public QObject {
    Q_OBJECT
public:
    explicit ApiClient(QObject* parent = nullptr);

    void setBaseUrl(const QString& url);
    QString baseUrl() const { return m_baseUrl; }

    // Ping server to validate URL
    void pingServer();

    // Validate student credentials and get session
    void validateStudent(const QString& regNum, const QString& examCode);

    // Fetch exam questions
    void fetchQuestions(const QString& sessionToken);

    // Resume exam with supervisor code
    void resumeExam(const QString& regNum, const QString& examCode, const QString& resumeCode);

    // Retake exam with supervisor code
    void retakeExam(const QString& regNum, const QString& examCode, const QString& retakeCode);

    // Submit answers
    void submitExam(const QString& sessionToken, const QMap<int, QString>& answers, int secondsUsed);

signals:
    void pingSuccess();
    void pingFailed(const QString& error);

    void validateSuccess(const QJsonObject& studentData);
    void validateFailed(const QString& error);

    void questionsReady(const QJsonArray& questions, const QJsonObject& meta);
    void questionsFailed(const QString& error);

    void resumeSuccess(const QJsonObject& sessionData);
    void resumeFailed(const QString& error);

    void retakeSuccess(const QJsonObject& studentData);
    void retakeFailed(const QString& error);

    void submitSuccess(const QJsonObject& result);
    void submitFailed(const QString& error);

// private slots:
//     void onPingReply(QNetworkReply* reply);

private:
    QNetworkReply* post(const QString& endpoint, const QJsonObject& body);
    QNetworkReply* get(const QString& endpoint, const QString& token = "");

    QNetworkAccessManager* m_nam;
    QString m_baseUrl;
};
