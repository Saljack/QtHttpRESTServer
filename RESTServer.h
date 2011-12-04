#ifndef RESTServer_H
#define RESTServer_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QStringList>
#include <QString>
#include <QStringList>
#include <QHash>
#include "serverrequest.h"


class RESTServer : public QTcpServer {
    Q_OBJECT
public:
    RESTServer(int port, QObject* parent = 0);
    virtual ~RESTServer();
protected slots:
    virtual void incomingConnection(int socketDescriptor);
    void handshakeComplete();
    void connectionClosed();
    void receiveData();
private:
    QString m_certPath;
    QSslKey* m_key;
    QSslCertificate m_cert;
    QString m_notFoundFileHtmlPath;
    void sendNoContent(QSslSocket* socket);
    void sendMSG(QSslSocket* socket, const QString msg);
    bool isBasicAuthorizated(const QStringList& authlist) const;
    
    void receivedPUT(QSslSocket* socket, const QStringList& firstLine, const QHash<QString, QStringList>& header);
    void sendOnGETReply(QTcpSocket* socket, const ServerRequest* req);
    void sendOnPUTReply(QTcpSocket* socket, const ServerRequest* req);
    
    ServerRequest* receiveRequestHeader(QTcpSocket* socket);

public slots:
    void errorySSL(QList<QSslError>);
};

#endif // RESTServer_H
