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


#include "serverrequest.h"
#include <QDebug>

ServerRequest::ServerRequest(): m_method(ServerRequest::NONE)
{

}

ServerRequest::ServerRequest(const ServerRequest& other)
{
}

ServerRequest::~ServerRequest()
{

}

/**
 *
 * @param key of header
 * @return QStringList of line or empty QStringList
 */
const QStringList* ServerRequest::headerLine(const QString& key) const
{
    return m_header.value(key);
}

/**
 *
 * @return true if method is recognized false if method is not recognized
 */
bool ServerRequest::parseFirstLine()
{

//     if (m_firstLine->isEmpty()) {
    if (m_firstLine[0] == "GET") {
        setMethod(GET);
    } else if (m_firstLine[0] == "PUT") {
        setMethod(PUT);
    } else if (m_firstLine[0] == "POST") {
        setMethod(POST);
    } else {
        setMethod(NONE);
        return false;
    }

    return true;
//     }
//     return false;
}

/**
 *
 *@param authBase64 authorization encrypted base64
 */
bool ServerRequest::isAuthorized(const QByteArray& authBase64)
{
    if (!m_header.isEmpty()) {
        if (m_header.contains("Authorization")) {
            QStringList* auth = m_header.value("Authorization");//get line with authorization
	    
            if (auth->length() > 2) {
	      
                if (auth->at(1) == "Basic") {
                    if (auth->at(2).compare(authBase64,Qt::CaseInsensitive) == 0) {
                        return true;
                    }
                }

            }
        }
    }
    return false;
}

bool ServerRequest::insertRawHeaderLine(QString line)
{
    QStringList* ln = new QStringList(line.split(QRegExp("[ \r\n][ \r\n]*")));
    if (!ln->isEmpty()) {
        if (ln->at(0) == "\r\n") {//test end of header
	    delete ln;
            return false;
        }
        m_header.insert((*ln)[0].remove(":"), ln); //remove :
        return true;
    }
    return false;
}

bool ServerRequest::setFirstLine(QString firstLine)
{
    m_firstLine = firstLine.split(QRegExp("[ \r\n][ \r\n]*"));
    return parseFirstLine();
}

QString ServerRequest::path() const
{
    if(m_firstLine.length()>2){
      return m_firstLine[1];
    }
}






