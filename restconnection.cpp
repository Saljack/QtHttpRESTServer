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
#include <QSslSocket>
#include <QDate>

RESTConnection::RESTConnection(int socketDescriptor, QString certificate, QSslKey* key, QObject* parent)
        :QThread(parent), m_socketDescriptor(socketDescriptor), m_cert(certificate), m_key(key) {

}


void RESTConnection::run() {
    m_socket = new QSslSocket();
    
    if (m_socket->setSocketDescriptor(m_socketDescriptor)) {
        m_socket->setLocalCertificate(m_cert);
        m_socket->setPrivateKey(*m_key);
        connect(m_socket, SIGNAL(encrypted()), this, SLOT(handshakeComplete()));
        m_socket->setPeerVerifyMode(QSslSocket::VerifyNone);
        m_socket->startServerEncryption();
    }

    exec();
}

void RESTConnection::handshakeComplete()
{
    connect(m_socket, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(receiveData()));
}

void RESTConnection::connectionClosed()
{
    m_socket->close();
    m_socket->deleteLater();
}

void RESTConnection::receiveData()
{
//      if (m_socket->canReadLine()) {
        QStringList tokens = QString(m_socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        qDebug() << m_socketDescriptor << tokens;

        if (tokens[0] == "GET") {
            QByteArray r = "HTTP/1.0 200 Ok\r\n"
                           "Content-Type: text/html; charset=\"utf-8\"\r\n"
                           "\r\n"
                           "<h1>Nothing to see here</h1>\n";
            if (m_socket->isWritable()) {
                m_socket->write(r);
                m_socket->waitForBytesWritten();
            }
            m_socket->close();

        }
//     }
}

#include "restconnection.moc"
