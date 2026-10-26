cmake_minimum_required(VERSION 3.16)

project(SimpleChatServer
    VERSION 1.0.0
    DESCRIPTION "Concurrent multi-client TCP server"
    LANGUAGES CXX
)

add_executable(SimpleChatServer src/main.cpp src/TCPChatServer.cpp src/Logger.cpp)
target_compile_features(SimpleChatServer PRIVATE cxx_std_17)
target_compile_options(SimpleChatServer PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(SimpleChatServer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src  ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS SimpleChatServer
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
