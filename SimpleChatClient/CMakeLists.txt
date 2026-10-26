cmake_minimum_required(VERSION 3.16)
project(SimpleChatClient
    VERSION 1.0.0
    DESCRIPTION "TCP client"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set( CMAKE_INCLUDE_CURRENT_DIR ON )

find_package(Qt5 COMPONENTS Core Widgets  Network REQUIRED)
## message(STATUS "Qt5 found: ${Qt5_FOUND}")
## message(STATUS "Qt5 version: ${Qt5_VERSION}")
## message(STATUS "Qt5 dir: ${Qt5_DIR}")

## set(CMAKE_AUTOUIC_SEARCH_PATHS "/src")

add_executable(SimpleChatClient src/main.cpp include/TCPClient.h src/TCPClient.cpp include/Logger.h src/Logger.cpp include/mainwindow.h src/mainwindow.cpp src/mainwindow.ui)
target_compile_features(SimpleChatClient PRIVATE cxx_std_17)
target_compile_options(SimpleChatClient PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(SimpleChatClient Qt${Qt_VERSION}::Core Qt${Qt_VERSION}::Gui Qt${Qt_VERSION}::Widgets Qt${Qt_VERSION}::Network)

target_include_directories(SimpleChatClient PUBLIC ${Qt_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/src  ${CMAKE_CURRENT_SOURCE_DIR}/include )

install(TARGETS SimpleChatClient
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
