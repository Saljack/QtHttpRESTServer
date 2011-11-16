#include "RESTServer.h"
#include "restconnection.h"
#include <QTcpSocket>

RESTServer::RESTServer(int port, QObject* parent) : QTcpServer(parent) {

    listen(QHostAddress::Any, port);
}

void RESTServer::incomingConnection(int socketDescriptor) {
    RESTConnection *connection = new RESTConnection(socketDescriptor);
    connect(connection, SIGNAL(finished()), connection, SLOT(deleteLater()));
    connection->start();

}


RESTServer::~RESTServer() {
}


#include "RESTServer.moc"
