#include "mainwindow.h"
#include <iostream>
#include <cstring>
#include <QString>

#include <QInputDialog>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    m_client = new TCPClient(this);
    QString ltext = QStringLiteral("Введите сообщение (максимум %1 символов) и нажмите Enter").arg(maxTextSymbol);
    ui->label_2->setText(ltext);
    connect(ui->pbExit, &QPushButton::clicked, this, &MainWindow::onExitClicked);
    connect(ui->pbSend, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(ui->pbConnect, &QPushButton::clicked, this, &MainWindow::onReconnect);
    connect(ui->textEdit, &QTextEdit::textChanged, this, &MainWindow::onTextEditChanged);

    connect(m_client->m_socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(m_client->m_socket, &QTcpSocket::connected, this, &MainWindow::onTCPConnected);
    connect(m_client->m_socket, &QTcpSocket::disconnected, this, &MainWindow::onTCPDisconnected);
    connect(m_client->m_socket, &QAbstractSocket::errorOccurred, this, &MainWindow::onErrorOccurred);
    
    myUserName = getUserName();
}

MainWindow::~MainWindow() {
    delete ui;
}

int MainWindow::TCPconnect(std::string _host, quint16 port) {

    const QString host = _host.c_str();
    if (tcpHost.empty()) {
        tcpHost = _host;
        tcpPort = port;
    }
    if (m_client->connectToHost(host, port)) {
        return 0;
    } else {
        return 1;
    }
}

QString MainWindow::getUserName() {
    QString header = "";
    QString text;
    bool ok;
  
    while (text.isEmpty()) {
        text = QInputDialog::getText(nullptr, header, "Введите ваше имя (20 символов):", QLineEdit::Normal, QString(), &ok);
        if (!ok) {
            m_client->disconnectFromHost();
        }
        else if (text.isEmpty()) {
            header = "Имя не может быть пустым. Введите ваше имя (20 символов):";
        }
        else if (text.length() > 20) {
            header = "Имя превышает 20 символов. Введите ваше имя (20 символов):";
            text.clear();
        }

    }
    return text;   
}

void MainWindow::sendUserName() {
    QString msg = "NewUser:" + myUserName + "\0";
    m_client->sendMessage(msg); 
}

void MainWindow::onReconnect () {
    if (MainWindow::TCPconnect(tcpHost, tcpPort) == 1) {
        ui->textBrowser->append("Ошибка соединения.");
    }
}

void MainWindow::onTCPConnected() {
    sendUserName();
    ui->pbConnect->setEnabled(false);
    ui->pbSend->setEnabled(true);
    ui->listWidget->clear();
    ui->listWidget->addItem(myUserName);
}

void MainWindow::onTCPDisconnected() {
    if (needExit) {
        QApplication::quit();
    }
    ui->textBrowser->append("Сервер оборвал соединение.");
    ui->pbConnect->setEnabled(true);
    ui->pbSend->setEnabled(false);
}

void MainWindow::onReadyRead() {
    QByteArray data = m_client->m_socket->readAll();
    QString msg  = QString::fromUtf8(data, data.size());
    messageAnalyze(msg);
}

void MainWindow::messageAnalyze(const QString &msg) {
    if (msg.startsWith("AddUser:")) {  // add new user
            QString username = msg.mid(8, msg.size() -9);
            ui->listWidget->addItem(username);
    }
    else if (msg.startsWith("DeleteUser:")) { // delete user
        QString username = msg.mid(11,msg.size() -12);
        for (int row = 0; row < ui->listWidget->count(); row++) {
            if (ui->listWidget->item(row)->text() ==  username){
                QListWidgetItem *item = ui->listWidget->takeItem(row);
                QWidget* lwidget = ui->listWidget->itemWidget(item);
                delete lwidget;
                if (item) delete item;
                break;
            }
        }
    }
    else if (msg.startsWith("ListUser:")) { // list users
        QString listuser = msg.mid(9, msg.size() -11);
        QStringList users = listuser.split(";");
        if (users.size() > 0) {
            for (const QString& us : users) {
                if (!us.isEmpty()) ui->listWidget->addItem(us);
            }
        }
    }
    else { // show message
        QString text = msg.mid(0, msg.size()-1);
        ui->textBrowser->append(text);
    }
}

void MainWindow::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    QString str;
    switch (socketError) {
        case QAbstractSocket::HostNotFoundError:
            str = "Хост не найден: проверьте имя или IP-адрес.";
            break;
        case QAbstractSocket::ConnectionRefusedError:
            str = "Сервер отклонил соединение: возможно, он не запущен или порт занят.";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            str = "Соединение закрыто удалённой стороной (часто это штатное завершение).";
            break;
        case QAbstractSocket::UnknownSocketError:
            str = "Неизвестная ошибка сокета.";
            break;
        default:
            str = "Неизвестная ошибка сокета.";
    }

    QString err = "Error : " + str + m_client->m_socket->errorString();
    ui->textBrowser->append(err);
    ui->pbConnect->setEnabled(true);
    ui->pbSend->setEnabled(false);
}

void MainWindow::onExitClicked() {
    QString msg = "DeleteUser:" + myUserName + "\0";
    m_client->sendMessage(msg);
    needExit = true;
}


void MainWindow::onTextEditChanged() {
    QString str = ui->textEdit->toPlainText();
    int strLen = str.length();
    if (str.length() > maxTextSymbol) {
        str = str.mid(0,maxTextSymbol);
        ui->textEdit->setText(str);
        QTextCursor cursor = ui->textEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        ui->textEdit->setTextCursor(cursor);
        strLen = maxTextSymbol;
    }
    ui->lblCountSymbol->setNum(strLen);
}

void MainWindow::onSendClicked() {
    QString text = ui->textEdit->toPlainText();
    m_client->sendMessage(text + "\0");
    ui->textBrowser->append("I> " + text);
    ui->textEdit->clear();
    ui->lblCountSymbol->setNum(0);
}