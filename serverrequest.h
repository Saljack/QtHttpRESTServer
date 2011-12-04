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


#ifndef SERVERREQUEST_H
#define SERVERREQUEST_H
#include <QHash>
#include <QString>
#include <QStringList>
#include <QByteArray>



/**
 * request incoming from client
 */
class ServerRequest
{

public:
    ServerRequest();
    ServerRequest(const ServerRequest& other);
    virtual ~ServerRequest();

    enum RequestMethod {

        GET=0,
        PUT,
        POST,
        NONE=10
    };

    inline RequestMethod method() const {
        return m_method;
    };

    inline void setMethod(RequestMethod method) {
        m_method = method;
    };

//     inline void setHeader(QHash<QString, QStringList> header) {
//         m_header = header;
//     };

    inline bool hasHeaderLine(const QString& key) const {
        return m_header.contains(key);
    };
    const QStringList* headerLine (const QString& key) const;

    bool setFirstLine(QString firstLine);

    inline const QByteArray* content() const {
        return &m_content;
    };
    inline void setContent(QByteArray content) {
        m_content = content;
    };
    
    bool isAuthorized(const QByteArray& authBase64);
    bool insertRawHeaderLine(QString line);
    QString path() const;





private:
    QStringList m_firstLine;
    RequestMethod m_method;
    QHash<QString, QStringList*> m_header;
    QByteArray m_content;
    bool parseFirstLine();


};



#endif // SERVERREQUEST_H
