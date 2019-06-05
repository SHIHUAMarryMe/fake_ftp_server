#ifndef HTTP_WORKER_H
#define HTTP_WORKER_H

#include <QObject>
#include <QTcpServer>

#include <memory>


class HttpServerWorker final : public QObject
{
  Q_OBJECT

public:
    explicit HttpServerWorker(QObject* const parent = nullptr);
    HttpServerWorker(const HttpServerWorker&) = delete;
    HttpServerWorker& operator=(const HttpServerWorker&) = delete;

    virtual ~HttpServerWorker();

    void readData();
    void displayError(QAbstractSocket::SocketError err)noexcept;

    void start(const QHostAddress& addr, const std::size_t& port);
private:

    void parse_header(const QByteArray& header, QTcpSocket *socket);
    void response_file_list(QTcpSocket* socket);
    void response_continue(QTcpSocket *socket);

    void flush_content_to_file(const QByteArray& header);

    void get_file_info_from_header(const QByteArray& header);

    std::shared_ptr<QTcpServer> tcp_server_;
    QByteArray client_data_{};
    std::size_t file_size_{0};
    QByteArray file_name_{};
    QByteArray boundary_{};
};



#endif // HTTP_WORKER_H
