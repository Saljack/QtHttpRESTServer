
#include "restserver.h"
#include <QString>
#include <QFile>
#include <QStringList>
#include <QHash>
#include <QFileInfo>
#include "serverrequest.h"

RESTServer::RESTServer(int port, QObject* parent) : QTcpServer(parent) {
    m_certPath = "cert/server.crt";
    QFile fkey("cert/server.key");
    fkey.open(QIODevice::ReadOnly | QIODevice::Text);
    m_key = new QSslKey(&fkey, QSsl::Rsa);
    fkey.close();
    listen(QHostAddress::Any, port);


    m_notFoundFileHtmlPath = "404.html";
}

/**
 * Incoming connection on server
 */
void RESTServer::incomingConnection(int socketDescriptor) {
//     qDebug() << "nove spojeni " << socketDescriptor;
    nextPendingConnection();
    QSslSocket* socket = new QSslSocket();
    qDebug() << "Prichazi spojeni "<< socketDescriptor;
    QIODevice* f; 


    if (socket->setSocketDescriptor(socketDescriptor)) {
        if (m_cert == 0) {
            socket->setLocalCertificate(m_certPath);
            m_cert = socket->localCertificate();
        } else {
            socket->setLocalCertificate(m_cert);
        }
        QMap<QSsl::AlternateNameEntryType, QString> a=socket->localCertificate().alternateSubjectNames();
        QMapIterator<QSsl::AlternateNameEntryType, QString> it(a);
        while (it.hasNext()) {
            qDebug() << it.next().value();
        }



        socket->setPrivateKey(*m_key);
        connect(socket, SIGNAL(encrypted()), this, SLOT(handshakeComplete()));
        connect(socket, SIGNAL(sslErrors(QList<QSslError>)), SLOT(errorySSL(QList<QSslError>)));
        socket->startServerEncryption();
    }

}


RESTServer::~RESTServer() {
}

void RESTServer::connectionClosed()
{
    QSslSocket* socket = dynamic_cast<QSslSocket *>(sender());
    connect(socket, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void RESTServer::handshakeComplete()
{
    QSslSocket* socket = dynamic_cast<QSslSocket *>(sender());
//     qDebug() << "sifrovano " << socket->socketDescriptor();
    connect(socket, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void RESTServer::receiveData()
{
    QSslSocket* socket = dynamic_cast<QSslSocket *>(sender());

    if (socket->canReadLine()) {
        qDebug()<<"################################INCOMING####################";

        ServerRequest* req = receiveRequestHeader(socket);

        //DBG
        QByteArray pass = "tomas:superman";
        pass = pass.toBase64();
        //END DBG
        if (req->isAuthorized(pass)) {
            switch (req->method()) {
            case ServerRequest::GET:
                sendOnGETReply(socket, req);
		break;
	    case ServerRequest::PUT:
		sendOnPUTReply(socket, req);
            }
        } else {
            socket->readAll();
            QByteArray ok = "HTTP/1.1 401 Unauthorized\r\n"
                            "WWW-Authenticate: Basic realm=\"tajne\"\r\n"
                            "\r\n";
            socket->write(ok);
            socket->waitForBytesWritten();
            socket->close();
        }
        socket->close();
	
        delete req;

    }
}


void RESTServer::sendNoContent(QSslSocket* socket)
{
    sendMSG(socket, "204 No Content");
}

void RESTServer::sendMSG(QSslSocket* socket, QString msg)
{

    QByteArray send = "HTTP/1.1 "+msg.toUtf8()+"\r\n"
                      "\r\n";
    if (socket->isWritable()) {
        socket->write(send);
    } else {
        qDebug() << "cannot send msg:"+msg+" socket is not writable";
    }
}


void RESTServer::errorySSL(QList< QSslError > errs)
{
    foreach(QSslError er, errs) {
        qDebug() << er.errorString();
    }
}

bool RESTServer::isBasicAuthorizated(const QStringList& authlist) const
{
//     if (authlist[1].compare("Basic", Qt::CaseInsensitive) == 0) {//zda je to basic
//         QString base = authlist[2];
//         qDebug() << QByteArray::fromBase64(base.toAscii());
//         QString heslo = QByteArray("tomas:superman").toBase64();
//         qDebug() << base<< " moje kodovani " << heslo;
//         if (heslo.compare(base, Qt::CaseInsensitive) == 0) {
//             return true;
//         }
//     }
//     return false;
}

ServerRequest* RESTServer::receiveRequestHeader(QTcpSocket* socket)
{
    ServerRequest* req = new ServerRequest();

//     qDebug() << socket->readAll();
    if (socket->canReadLine()) {
        bool isSet = req->setFirstLine(socket->readLine());//set first line
        if (isSet) {
// 	  qDebug() << "Vypisuji hlavicku";
            while (socket->canReadLine()) {

                QByteArray tmp = socket->readLine();
//                 qDebug() << "Hlavicka"<< tmp;
                if (!req->insertRawHeaderLine(tmp)) {
                    break;
                }

            }
        }
    }
    return req;
}

void RESTServer::receivedPUT(QSslSocket* socket, const QStringList& firstLine, const QHash< QString, QStringList >& header)
{
    while (socket->canReadLine()) {
        QString tmp = socket->readLine();
        qDebug() << tmp;
        if (tmp == "\r\n") {//testovani konce hlavicky
            break;
        }
    }

    qDebug() << "jsem v PUT";
    if (firstLine[1].endsWith(".xml")) { //je to soubor s priponou xml
        //Vytvoreni souboru
        qDebug() << "vytvarim sob";
        QFileInfo info("xml/"+firstLine[1]);
        if (info.exists()) {//soubor existuje
            QFile file(info.filePath());
            if (file.open(QIODevice::WriteOnly)) {
                file.write(socket->readAll());
                file.flush();
                file.close();
                sendMSG(socket, "200 Ok");
            } else {
                sendNoContent(socket);
            }
        } else {//pokusim se ho vytvorit
            qDebug() << "jeste jednou vytv";
            QFile file(info.filePath());
            if (file.open(QIODevice::WriteOnly)) {
                QByteArray v = socket->readAll();
                qDebug() << v;
                file.write(v);
                file.flush();
                file.close();
                sendMSG(socket, "201 Created");
                //TODO poslat jeste adresu souboru

            } else {
                sendNoContent(socket);
            }
        }
    } else {
        sendNoContent(socket);
    }
}

void RESTServer::sendOnPUTReply(QTcpSocket* socket, const ServerRequest* req)
{

}


void RESTServer::sendOnGETReply(QTcpSocket* socket, const ServerRequest* req)
{
    QString fl = req->path();
    QFileInfo path("xml/"+fl);
    if (path.isFile() && path.isReadable()) {//FILE EXIST

        if (socket->isWritable()) {
            QByteArray ok = "HTTP/1.1 200 Ok\r\n"
                            "Content-Type: text/xml; charset=\"utf-8\"\r\n"
                            "\r\n";
            socket->write(ok);
            qDebug() << fl+": existuje";
            QFile f(path.filePath());
            if (f.open(QIODevice::ReadOnly)) {
                socket->write(f.readAll());
            }
            socket->waitForBytesWritten();
        }
    } else {//FILE not exist
        qDebug() << "File not found";
        if (socket->isWritable()) {
            QByteArray nof = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Type: text/html; charset=\"utf-8\"\r\n"
                             "\r\n";
            socket->write(nof);
            QFile nof404(m_notFoundFileHtmlPath);
            QFileInfo infonof(nof404);
            if (infonof.isFile() && infonof.isReadable()) {
                if (nof404.open(QIODevice::ReadOnly)) {
                    socket->write(nof404.readAll());
                }
            }
            socket->waitForBytesWritten();
        }
    }
}

#include "restserver.moc"
