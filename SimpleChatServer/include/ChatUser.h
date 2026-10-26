#include <iostream>

class ChatUser {
    private:
    int socket_fd;
    std::string username = "";

    public:
    ChatUser(int _socket_fd, std::string _username) {
        socket_fd = _socket_fd;
        username = _username;
    };
    
    ~ChatUser(){};

    std::string getUser() {
        return username;
    };

    int getSocket_id() {
        return socket_fd;
    };

    void setName(std::string _username) {
        username = _username;
    }
};