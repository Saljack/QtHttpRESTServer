#include <QCoreApplication>
#include "RESTServer.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    RESTServer* foo = new RESTServer(8081, &app);
    return app.exec();
}
