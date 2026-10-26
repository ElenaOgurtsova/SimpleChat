#include <QString>
#include "TCPClient.h"

TCPClient::TCPClient(QObject *parent)
    : QObject(parent)
{
   
    m_socket = new QTcpSocket(this);
}

bool TCPClient::connectToHost(const QString &host, quint16 port) {
    m_socket->connectToHost(host, port);
    return true;
}

void TCPClient::disconnectFromHost() {
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->close();
    }
}

QByteArray TCPClient::serializeMessage(const QString &msg) {
    QByteArray data = msg.toUtf8();
    QByteArray packet;
    //packet.append(reinterpret_cast<const char*>(&data.size()), sizeof(quint32));
    packet.append(data);
    return packet;
}

void TCPClient::sendMessage(const QString &msg) {
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        emit errorOccurred(tr("Не подключено"));
        return;
    }

    QByteArray packet = serializeMessage(msg);
    qint64 written = m_socket->write(packet);
    if (written == -1) {
        emit errorOccurred(m_socket->errorString());
    }
}
