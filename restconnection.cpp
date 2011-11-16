/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  TomÃ¡Å¡ PolednÃ½ <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "restconnection.h"
#include <qhostaddress.h>

RESTConnection::RESTConnection(int socketDescriptor, QObject* parent): QThread(parent), m_socketDescriptor(socketDescriptor) {

}


void RESTConnection::run() {
    QTcpSocket socket;
    if (!socket.setSocketDescriptor(m_socketDescriptor)) {
        qDebug() << socket.error();
        return;
    }

    socket.waitForReadyRead();

    if (socket.canReadLine()) {
        QStringList tokens = QString(socket.readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
	qDebug() << m_socketDescriptor << tokens;
	
        if (tokens[0] == "GET") {
            QByteArray r = "HTTP/1.0 200 Ok\r\n"
                           "Content-Type: text/html; charset=\"utf-8\"\r\n"
                           "\r\n"
                           "<h1>Nothing to see here</h1>\n";
            if (socket.isWritable()) {
                socket.write(r);
		socket.waitForBytesWritten();
            }
            socket.close();
	  
        }
    }
}

void RESTConnection::readyToRead() {

}

