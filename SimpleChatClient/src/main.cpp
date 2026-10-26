#include <iostream>
#include <cstring>
#include <exception>
#include <QApplication>
#include <QLocale>
#include <QObject>
#include "mainwindow.h"
#include "TCPClient.h"
#include "Logger.h"

using namespace std;

int main(int argc, char* argv[]) {
    quint16 port = 8901;
    try {
        std::locale russian("ru_RU.UTF-8");
        std::locale::global(russian);
        std::cout.imbue(russian);
    } catch (const std::runtime_error& e) {
        string msg = e.what();
        msg = "Local ru_RU.UTF-8 not found: " + msg + ". Use default.\n";
        logWarn(msg);
        std::locale::global(std::locale(""));
        std::cout.imbue(std::locale());
    }
    
    if (argc != 2) {
        logError("Incorrect arguments. Must be 1 arguments : <ip address>");
        return 1;
    }

    string ipaddress = argv[1];
    QApplication a(argc, argv);
    MainWindow w;

    if (w.TCPconnect(ipaddress, port) != 0) {
        logError("Error connection to server.");
        return 1;
    }
    w.show();

    return QCoreApplication::exec();

    //return 0;
}
