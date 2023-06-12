#include "tcpcommunication.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    TcpCommunication listener;
    return a.exec();
}
