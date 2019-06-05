
#include "httpworker.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpServerWorker worker{};
    worker.start(QHostAddress::AnyIPv4, 8888);

    return a.exec();
}
