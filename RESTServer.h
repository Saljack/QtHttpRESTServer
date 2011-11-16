#ifndef RESTServer_H
#define RESTServer_H

#include <QTcpServer>


class RESTServer : public QTcpServer {
        Q_OBJECT
    public:
        RESTServer(int port, QObject* parent = 0);
        virtual ~RESTServer();
    protected slots:
        virtual void incomingConnection(int socketDescriptor);

};

#endif // RESTServer_H
