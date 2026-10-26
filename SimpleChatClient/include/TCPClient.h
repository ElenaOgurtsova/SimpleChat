#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QString>

class TCPClient : public QObject {
    Q_OBJECT
public:

    explicit TCPClient(QObject *parent = nullptr);
    QTcpSocket *m_socket = nullptr;

    bool connectToHost(const QString &host, quint16 port);
    void disconnectFromHost();
    void sendMessage(const QString &msg);
    QByteArray serializeMessage(const QString &msg);

signals:
    void errorOccurred(const QString &message);

};

#endif // TCPCLIENT_H