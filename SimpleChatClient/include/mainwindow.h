#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "TCPClient.h"
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    int TCPconnect(std::string _host, quint16 port);

private slots:
    void onExitClicked(); 
    void onSendClicked();
    void onReadyRead();
    void onTCPConnected();
    void onTCPDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onTextEditChanged();
    void onReconnect();

    private:
    QString myUserName;
    bool needExit = false;
    int maxTextSymbol = 500;
    std::string tcpHost;
    quint16 tcpPort;
    Ui::MainWindow *ui;
    TCPClient *m_client = nullptr;
    void messageAnalyze(const QString &text);
    QString getUserName();
    void sendUserName();
};

#endif // MAINWINDOW_H