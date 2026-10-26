#include <iostream>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "ChatUser.h"

#define PORT 8901
#define BUFFER_SIZE 1024
#define BUFFER_MSG_SIZE 1050
#define MAX_EVENTS 100

class TCPChatServer {
    public:
    TCPChatServer(){}
    ~TCPChatServer(){}
    void start();
    void stop();
    std::vector<ChatUser> clients;

    private:
    int server_fd, new_socket;
    sockaddr_in addr{};
    int opt = 1;
    int epfd;
    struct epoll_event events[MAX_EVENTS];
    void AddUser(int fd);
    void DeleteUser(int fd);
    void SendMessage(int fd, std::string msg);
    std::string getUserByFD(int fd);
    std::string getUserFromBuffer();
    void analizeMessage(int fd, std::string msg);
    bool checkExistConnection(int fd);
    void sendUserLists(int fd);
    std::string receiveMSG(int fd);
    void ChangeUserName (int fd, std::string _uname);
};