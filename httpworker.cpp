
#include "httpworker.h"

#include <QDir>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QByteArray>

#include <atomic>
#include <string>

static std::atomic<int> COUNTER{ 0 };


HttpServerWorker::HttpServerWorker(QObject * const parent)
    :QObject{parent},
      tcp_server_{std::make_shared<QTcpServer>()}
{
    //initialize connection.
    QObject::connect(tcp_server_.get(), &QTcpServer::newConnection, this, &HttpServerWorker::readData);
    QObject::connect(tcp_server_.get(), &QTcpServer::acceptError, this, &HttpServerWorker::displayError);
}

HttpServerWorker::~HttpServerWorker()
{
    if(tcp_server_->isListening())
    {
        tcp_server_->close();
    }
}

void HttpServerWorker::readData()
{
    if(!tcp_server_->hasPendingConnections())
    {
        return;
    }

    QTcpSocket* client{tcp_server_->nextPendingConnection()};

    QObject::connect(client, &QTcpSocket::readyRead, [this, client]()
    {
        QByteArray data{ client->readAll() };

#ifdef QT_DEBUG
//        qDebug() << data;
#endif //QT_DEBUG

        this->parse_header(data, client);
    });

}

void HttpServerWorker::displayError(QAbstractSocket::SocketError err)noexcept
{
    qDebug() << "when accepting a new connection a error occured: " << err;
}

void HttpServerWorker::start(const QHostAddress &addr, const std::size_t &port)
{
    tcp_server_->listen(addr, port);
}

void HttpServerWorker::parse_header(const QByteArray &header, QTcpSocket* socket)
{

    if(header.contains(u8"GET /?query=all"))
    {
        response_file_list(socket);

    }else if(header.contains(u8"application/octet-stream")){
        qDebug() << "=====================================" << header;
        get_file_info_from_header(header);


    }else{
//        qDebug() << "=====================================" << header;
    }

}

void HttpServerWorker::response_file_list(QTcpSocket *socket)
{
    QDir current_dir{"."};
    QList<QString> files_name{ current_dir.entryList(QDir::Files) };

    QJsonObject json_object{};
    QJsonArray  json_array{};

    for(const auto& file_name : files_name)
    {
        json_array.append(file_name);
    }

    json_object.insert("files_name", json_array);
    QJsonDocument json_doc{json_object};

    qDebug() << socket->localAddress().toString() << ":  " << socket->localPort();

    QString http_header{
        "HTTP/1.1 200 OK \r\n"
        "Content-Type: application/json; charset=UTF-8\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: %1\r\n"
        "\r\n\r\n"
    };

    http_header = http_header.arg(json_doc.toJson().size()+1);

    QByteArray utf8_header{ http_header.toUtf8() };
    utf8_header += json_doc.toJson();
    utf8_header += "\r\n";

#ifdef QT_DEBUG
    qDebug() << utf8_header;
#endif //QT_DEBUG

    socket->write(utf8_header);
}

void HttpServerWorker::response_continue(QTcpSocket* socket)
{
//    QString http_header{
//        "HTTP/1.1 100 Continue \r\n"
//        "Content-Disposition: attachment;filename=Makefile"
//        "Connection: keep-alive\r\n"
//        "Access-Control-Allow-Origin: *\r\n"
//        "\r\n"
//    };

//    socket->write(http_header.toUtf8());
}



void HttpServerWorker::flush_content_to_file(const QByteArray &header)
{
//    qDebug() << "===============" << header;
}

void HttpServerWorker::get_file_info_from_header(const QByteArray &header)
{
    QByteArray::size_type pos{ header.indexOf("\r\n\r\n") };

    QByteArray protocal_header{ header.constData(), pos+1 };
    QList<QByteArray> header_contents{ protocal_header.split('\n') };

    qDebug() << header_contents;

    for(const auto& content : header_contents)
    {
        if(content.contains("name"))
        {
            QList<QByteArray> splited_content{ content.split(' ') };
        }
    }


    QByteArray content{ header.constData()+pos+4 };
}
