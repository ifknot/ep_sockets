TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

#There are no porting issues with the socket library - implementation is in a file called ws2_32.dll and there are 32-bit and 64-bit versions of the DLL in 64-bit Windows
win32 {
    LIBS += -lws2_32
}

SOURCES += \
        linux_socket.cpp \
        main.cpp \
        socket_factory.cpp \
        winsock_socket.cpp

HEADERS += \
    linux_socket.h \
    socket_constants.h \
    socket_errors.h \
    socket_factory.h \
    socketable.h \
    winsock_socket.h \
    winsock_specific.h \
    wsa_inetpton.h
