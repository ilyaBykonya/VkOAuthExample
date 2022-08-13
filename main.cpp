#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QApplication>
#include <QJsonArray>
#include <QWidget>
#include <QFile>
#include <QUrl>

const QUrl authUrl{ "https://oauth.vk.com/authorize" };
const QUrl tokenUrl{ "https://oauth.vk.com/access_token" };
const QString clientSecret{ "d4rZuR8zHiYHgntXC1kp" };
const QString clientId{ "51400815" };
constexpr quint32 scopeMask = 2;//https://dev.vk.com/reference/access-rights


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QWidget mainWindow;

    auto oauth = new QOAuth2AuthorizationCodeFlow(&mainWindow);
    auto replyHandler = new QOAuthHttpServerReplyHandler(6543, &mainWindow);


    oauth->setReplyHandler(replyHandler);
    oauth->setAccessTokenUrl(tokenUrl);
    oauth->setAuthorizationUrl(authUrl);
    oauth->setClientIdentifier(clientId);
    oauth->setClientIdentifierSharedKey(clientSecret);
    oauth->setScope(QString::number(scopeMask));
    QObject::connect(oauth, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);


    QObject::connect(oauth, &QOAuth2AuthorizationCodeFlow::granted, [oauth]() {
        const QUrl getFriends{ "https://api.vk.com/method/friends.get" };
        auto network_reply = oauth->post(getFriends, { { "v", 5.131 } });
        QObject::connect(network_reply, &QNetworkReply::finished, [network_reply] {
            network_reply->deleteLater();

            QJsonDocument response = QJsonDocument::fromJson(network_reply->readAll());
            qDebug() << "All friends ids:";
            for(const auto& user_id : response["response"]["items"].toArray())
                qDebug() << '\t' << user_id.toInteger();
        });
    });
    oauth->grant();

    mainWindow.show();
    return app.exec();
}
