#include "TCPChatServer.h"
#include <vector>

using namespace std;

void TCPChatServer::start()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (sockaddr*)&addr, sizeof(addr));

    listen(server_fd, SOMAXCONN);

    epfd = epoll_create1(0);
    if (epfd == -1) {
        cout << "Ошибка  epoil create : " << strerror(errno) << "\n";
    }

    struct  epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        cout << "Ошибка epoil_ctl server : " << strerror(errno) << "\n";
    };
    
    while (true) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (n == -1) {
            if (errno == EINTR) continue; // прервано сигналом
            cout << "Ошибка epoil_wait server : " << strerror(errno) << "\n";
            break;
        }
        for (int i = 0; i < n; ++i) {
            int fd = events[i].data.fd;

            if (!checkExistConnection(fd))
            {
                
                struct sockaddr_in client_addr{};
                socklen_t addr_len = sizeof(client_addr);

                int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &addr_len);
                if (client_fd == -1) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) continue;
                    cout << "Ошибка accept : " << strerror(errno) << "\n";
                    continue;
                }
                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

                // Регистрируем клиентский сокет в epoll для read/write
                struct epoll_event client_ev{};
                client_ev.events = EPOLLIN | EPOLLET; // EPOLLET — edge-triggered, если нужен
                client_ev.data.fd = client_fd;

                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev) == -1) {
                    cout << "Ошибка epoll_ctl client : " << strerror(errno) << "\n";
                    close(client_fd);
                    continue;
                }
            // сохранить client_fd в список активных клиентов
                AddUser(client_fd);
                sendUserLists(client_fd);
            }
            else {
                string msg = receiveMSG(fd);
                if (msg != "") {
                    analizeMessage(fd, msg);
                }
                
            }
        }
    }

}

void TCPChatServer::stop()
{
    close(epfd);
    for (ChatUser u : clients) {
        int cfd = u.getSocket_id();
        if (cfd != server_fd) {
           close(cfd);
        }
    }
    clients.clear();
    close(server_fd);
}

string TCPChatServer::receiveMSG(int fd) {
    string str ="";
    char ch;
    ssize_t n;
    int c = 0;
    while ((n = recv(fd, &ch, 1, 0)) == 1) {
        ++c;
        if (ch == '\0') {
            break;          // конец сообщения
        }
        if (ch == '\r') {
            cout << "r";
            continue;       // опционально: игнорировать \r
        }
        str.push_back(ch);
    }
    if ((c == 0 || (n == -1 && errno != EAGAIN && errno != EWOULDBLOCK)) && errno != 0) {
        cout << "Receive error:" << errno << " : " << strerror(errno) <<  "\n";
        return ""; 
    }
    return str;
}

void TCPChatServer::SendMessage(int fd, string msg) {
    const int sizeMsg = msg.size() + 1;
    char bufferMsg[BUFFER_MSG_SIZE];
    strncpy(bufferMsg, msg.c_str(), sizeMsg -1);
    bufferMsg[sizeMsg] = '\0'; 
    for (ChatUser usr : clients) {
        int cfd = usr.getSocket_id();
        string uname = usr.getUser();
        if (cfd != fd && uname != "" ) {
            send(cfd, bufferMsg, sizeMsg, 0);
        }
    }
}



void TCPChatServer::AddUser(int fd) {
    ChatUser user(fd, "");
    clients.push_back(user);
}

void TCPChatServer::DeleteUser(int fd) {
    int i = 0;
    for (ChatUser usr : clients) {
        if (usr.getSocket_id() == fd) {
            clients.erase(clients.begin() +i);
            break;
        }
        i++;
    }

    close(fd);
}

void TCPChatServer::ChangeUserName (int fd, string _uname) {
    int i = 0;
    for (ChatUser usr : clients) {
        if (usr.getSocket_id() == fd) {
            clients[i].setName(_uname);
            break;
        }
        i++;
    }
}

string TCPChatServer::getUserByFD(int fd) {
    string uname = "";
    for (ChatUser usr : clients) {
        if (usr.getSocket_id() == fd) {
            uname = usr.getUser();
            break;
        }
    }
    return uname;
}

void TCPChatServer::analizeMessage(int fd, string str) {
    if (str.find("NewUser:") == 0) {
        string usr = str.substr(8, str.size() - 8);
        ChangeUserName(fd, usr);
        string msg = "AddUser:" + usr + "\0";
        SendMessage(fd, msg);
    }
    else if (str.find("DeleteUser:") == 0) {
        DeleteUser(fd);
        SendMessage(fd, str);
    }
    else {
        string uname = getUserByFD(fd);
        string msg = uname + "> " + str;        
        SendMessage(fd, msg);
    }    
}

bool TCPChatServer::checkExistConnection(int fd) {
    bool connExist = false;
    for (ChatUser usr : clients) {
        if (usr.getSocket_id() == fd) {
            connExist = true;
            break;
        }
    }
    return connExist;
}

void TCPChatServer::sendUserLists(int fd) {
    char bufferMsg[BUFFER_MSG_SIZE];
    int count = clients.size();
    int h = count/48 +1;
    if (count > 0) {
        for (int i = 0; i <h ; i++) {
            string msg = "ListUser:";
            int begin = i*48;
            int end = begin+48; 
            if (end > count) end = count;   
            for (int j = begin; j<end; j++) {
                ChatUser usr = clients.at(j);
                msg += usr.getUser() + ";";
            }
            if (!msg.empty()) {
                int len = msg.length();
                strncpy(bufferMsg, msg.c_str(), len);
                bufferMsg[len+1] = '\0';
                send(fd, bufferMsg, len+1, 0);
            }      
        }
    }
}