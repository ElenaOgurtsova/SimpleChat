#!/bin/bash
SimpleChatClient/build/SimpleChatClient 127.0.0.1 > client1.log &
sleep 90s &
SimpleChatClient/build/SimpleChatClient 127.0.0.1 >client2.log &
sleep 120s
SimpleChatClient/build/SimpleChatClient 127.0.0.1 >client3.log &
exit

