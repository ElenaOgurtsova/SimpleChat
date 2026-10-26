#include <iostream>
#include "TCPChatServer.h"

int main() {
    TCPChatServer tserver;
    tserver.start();
    tserver.stop();

    return 0;
}