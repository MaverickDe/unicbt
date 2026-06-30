#include "apiclient.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QDebug>

ApiClient::ApiClient(QObject* parent)
    : QObject(parent)
    , m_nam(new QNetworkAccessManager(this))
{}

void ApiClient::setBaseUrl(const QString& url) {
    m_baseUrl = url.endsWith('/') ? url.chopped(1) : url;
}

void ApiClient::pingServer() {
    QNetworkRequest req(QUrl(m_baseUrl + "/api/ping"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    auto* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit pingFailed(reply->errorString());
            return;
        }
        emit pingSuccess();
    });
}

QNetworkReply* ApiClient::post(const QString& endpoint, const QJsonObject& body) {
    QNetworkRequest req(QUrl(m_baseUrl + endpoint));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonDocument doc(body);
    return m_nam->post(req, doc.toJson());
}

QNetworkReply* ApiClient::get(const QString& endpoint, const QString& token) {
    QNetworkRequest req(QUrl(m_baseUrl + endpoint));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!token.isEmpty())
        req.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    return m_nam->get(req);
}

void ApiClient::validateStudent(const QString& regNum, const QString& examCode) {
    auto* reply = post("/api/exam/validate", {
        {"reg_number", regNum},
        {"exam_code",  examCode}
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit validateFailed(reply->errorString());
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (obj["success"].toBool()) emit validateSuccess(obj["data"].toObject());
        else emit validateFailed(obj["message"].toString("Validation failed"));
    });
}

void ApiClient::fetchQuestions(const QString& sessionToken) {
    auto* reply = get("/api/exam/questions", sessionToken);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit questionsFailed(reply->errorString());
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (obj["success"].toBool()) {
            emit questionsReady(obj["questions"].toArray(), obj["meta"].toObject());
        } else {
            emit questionsFailed(obj["message"].toString("Failed to load questions"));
        }
    });
}

void ApiClient::resumeExam(const QString& regNum, const QString& examCode, const QString& resumeCode) {
    auto* reply = post("/api/exam/resume", {
        {"reg_number",   regNum},
        {"exam_code",    examCode},
        {"resume_code",  resumeCode}
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit resumeFailed(reply->errorString());
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (obj["success"].toBool()) emit resumeSuccess(obj["data"].toObject());
        else emit resumeFailed(obj["message"].toString("Resume failed"));
    });
}

void ApiClient::retakeExam(const QString& regNum, const QString& examCode, const QString& retakeCode) {
    auto* reply = post("/api/exam/retake", {
        {"reg_number",   regNum},
        {"exam_code",    examCode},
        {"retake_code",  retakeCode}
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit retakeFailed(reply->errorString());
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (obj["success"].toBool()) emit retakeSuccess(obj["data"].toObject());
        else emit retakeFailed(obj["message"].toString("Retake failed"));
    });
}

void ApiClient::submitExam(const QString& sessionToken, const QMap<int, QString>& answers, int secondsUsed) {
    QJsonObject answersObj;
    for (auto it = answers.begin(); it != answers.end(); ++it)
        answersObj[QString::number(it.key())] = it.value();

    QNetworkRequest req(QUrl(m_baseUrl + "/api/exam/submit"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + sessionToken).toUtf8());

    QJsonObject body;
    body["answers"]      = answersObj;
    body["seconds_used"] = secondsUsed;

    auto* reply = m_nam->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            emit submitFailed(reply->errorString());
            return;
        }
        auto doc = QJsonDocument::fromJson(reply->readAll());
        auto obj = doc.object();
        if (obj["success"].toBool()) emit submitSuccess(obj["data"].toObject());
        else emit submitFailed(obj["message"].toString("Submission failed"));
    });
}
