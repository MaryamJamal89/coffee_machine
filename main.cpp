// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <iostream>
#include <QString>
#include <QCoreApplication>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QUrlQuery>
#include <QUrl>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonArray>

void handleNetworkReply(QNetworkReply *reply, QNetworkAccessManager *manager) {
    // Connect signals for handling response or errors
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
        // Handle response here (e.g., check for errors)
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "HTTP Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Error:" << reply->errorString();
        } else {
            QByteArray responseData = reply->readAll();
            qDebug() << "HTTP Status Code:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << "Response:" << responseData;
        }
        // Clean up
        reply->deleteLater();
        manager->deleteLater();
    });
}

void sendPageLevelDataToUA(QUrlQuery & urlQuery) {
    // Add other custom parameters
    urlQuery.addQueryItem("cd2","User ID");
    urlQuery.addQueryItem("cd3","Login Status");
    urlQuery.addQueryItem("cd4",  "Language");
    urlQuery.addQueryItem("cd5","Page Name");
    urlQuery.addQueryItem("cd7","Line Of Business");
    urlQuery.addQueryItem("cd12","User Type");
    urlQuery.addQueryItem("cd16","Platform");
    urlQuery.addQueryItem("cd22","Category");
    urlQuery.addQueryItem("cd27","Learning Mode");
    urlQuery.addQueryItem("cd28","Tip Rank Mode");
    urlQuery.addQueryItem("cd35","Discreet mode");
    urlQuery.addQueryItem("cd38","Country");
    urlQuery.addQueryItem("cd39","Page Location");
    urlQuery.addQueryItem("cd40","Page Referrer");
    urlQuery.addQueryItem("cd41","Page Title");
    urlQuery.addQueryItem("cd42","Subcategory");
    urlQuery.addQueryItem("cd42","Color Theme Mode"); // check the duplication in cd index (42)
    urlQuery.addQueryItem("cd45","Invitation Status");
    urlQuery.addQueryItem("cd46","co_user_status");
    urlQuery.addQueryItem("cd47","Application Type");
    urlQuery.addQueryItem("cd57","Tech Profile");
}

void sendGoogleAnalyticsHit(QString tid, QString cid, QString hitType) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // Construct the URL with the Measurement Protocol parameters
    QUrl url("https://www.google-analytics.com/collect");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("v", "1");  // Protocol version
    urlQuery.addQueryItem("tid", tid);  // Property ID
    urlQuery.addQueryItem("cid", cid);  // Dummey Client ID
    urlQuery.addQueryItem("t", hitType);  // Hit type (e.g., 'event')
    urlQuery.addQueryItem("dp", "/test");

    urlQuery.addQueryItem("ec", "test_cat");  // Event category
    urlQuery.addQueryItem("ea", "test_act");  // Event action
    urlQuery.addQueryItem("el", "test_label");  // Event label
    urlQuery.addQueryItem("ev", "1");  // Event value

    sendPageLevelDataToUA(urlQuery); // send page-level data points

    url.setQuery(urlQuery);

    QNetworkRequest request(url);

    // Send the request
    QNetworkReply* reply = manager->get(request);

    // Connect signals for handling response or errors
    handleNetworkReply(reply, manager);
}

void sendPageLevelDataToGA4(QJsonObject & paramsObject) {
    // Add other custom parameters
    paramsObject["language"] = "Language";
    paramsObject["login_status"] = "Login Status";
    paramsObject["page_location"] = "Page Location";
    paramsObject["line_of_business"] = "Line Of Business";
    paramsObject["user_type"] = "User Type";
    paramsObject["page_referrer"] = "Page Referrer";
    paramsObject["page_title"] = "Page Name";
    paramsObject["platform"] = "platform";
    paramsObject["learning_mode"] = "Learning Mode";
    paramsObject["tip_rank_mode"] = "Tip Rank Mode";
    paramsObject["discreet_mode"] = "Discreet mode";
    paramsObject["page_name"] = "Page Name";
    paramsObject["user_id"] = "User ID";
    paramsObject["country"] = "Country";
    paramsObject["color_theme_mode"] = "Color Theme Mode";
    paramsObject["category"] = "Category";
    paramsObject["sub_category"] = "SubCategory";
    paramsObject["invitation_status"] = "invitation status";
    paramsObject["co_user_status"] = "co_user_status";
    paramsObject["application_type"] = "Application Type";
    paramsObject["consent_status"] = "Consent Status";
    paramsObject["tech_profile"] = "Tech Profile";
}

void sendGA4Hit(QString measurement_id, QString api_secret, QString cid) {
    QNetworkAccessManager* manager = new QNetworkAccessManager();

    // Construct the URL with the GA4 Measurement API endpoint
    QUrl url("https://www.google-analytics.com/mp/collect");

    QUrlQuery urlQuery;
    urlQuery.addQueryItem("api_secret", api_secret);
    urlQuery.addQueryItem("measurement_id", measurement_id);
    url.setQuery(urlQuery);

    // Get the current timestamp in microseconds
    qint64 timestamp_micros = QDateTime::currentMSecsSinceEpoch() * 1000;

    // Create the JSON payload
    QJsonObject payload;
    payload["client_id"] = cid;
    payload["timestamp_micros"] = timestamp_micros;
    payload["non_personalized_ads"] = false;

    // Create a User Properties object
    QJsonObject userPropertiesObject;

    // Create the 1st User Property object
    QJsonObject userPropertyObject1;
    userPropertyObject1["value"] = "Test User Property Object 1";
    userPropertiesObject["test_user_property_1"] = userPropertyObject1;
    // Create the 2nd User Property object
    QJsonObject userPropertyObject2;
    userPropertyObject2["value"] = "Test User Property Object 2";
    userPropertiesObject["test_user_property_2"] = userPropertyObject2;

    payload["user_properties"] = userPropertiesObject;

    // Create an events object
    QJsonArray eventsArray;

    QJsonObject eventObject;
    eventObject["name"] = "navigation_click";

    QJsonObject paramsObject;
    paramsObject["link_url"] = "link_url";
    paramsObject["link_text"] = "link_text";
    paramsObject["link_type"] = "click";
    paramsObject["engagement_time_msec"] = "1";
    paramsObject["debug_mode"] = 1; //true - show debug view

    sendPageLevelDataToGA4(paramsObject);

    eventObject["params"] = paramsObject;
    eventsArray.append(eventObject);

    payload["events"] = eventsArray;
    qDebug() << "payload:" << payload;

    // Convert the JSON payload to a QByteArray
    QJsonDocument jsonDoc(payload);
    QByteArray jsonData = jsonDoc.toJson();

    // Construct the request with POST method
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Send the POST request
    QNetworkReply* reply = manager->post(request, jsonData);

    // Connect signals for handling response or errors
    handleNetworkReply(reply, manager);
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QString cid = "132082374.1692290545"; // dummy data - needs to get dynamic solution

    // UA configuration, and finction call
    QString tid = "UA-XXXXXX-XX";
    sendGoogleAnalyticsHit(tid, cid, "pageview" /* or "event" */);

    // GA4 configuration, and finction call
    QString measurement_id = "G-XXXXXXXXXX";
    QString api_secret = "XXXXXXXXXX-XXXXXXXXXXX";
    sendGA4Hit(measurement_id, api_secret, cid);

    // Lunching the app window
    QQmlApplicationEngine engine;
    engine.addImportPath(":/imports");
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
