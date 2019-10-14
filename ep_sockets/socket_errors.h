#ifndef SOCKET_ERRORS_H
#define SOCKET_ERRORS_H

#include <string>
#include <map>
#ifdef WIN32
    #include <winsock2.h>
#endif

namespace net {

    static const std::string EMSG_SUCCESS = "Success";
    static const std::string EMSG_UNKNOWN = "Unrecognised error number.";
    static const std::string EMSG_INET_PTON = "src does not contain a character string representing a valid network address in the specified address family.";

#ifdef WIN32

    static std::map<int, std::string> error_messages = {
             {0, "No error."},
             {SOCKET_ERROR, "Generic socket error."},
             {WSAECONNREFUSED, "Connection refused. No connection could be made because the target computer actively refused it. This usually results from trying to connect to a service that is inactive on the foreign host—that is, one with no server application running."},
             {WSASYSNOTREADY, "The underlying network subsystem is not ready for network communication."},
             {WSAVERNOTSUPPORTED, "The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation."},
             {WSAEINPROGRESS, "A blocking Windows Sockets 1.1 operation is in progress."},
             {WSAEPROCLIM, "A limit on the number of tasks supported by the Windows Sockets implementation has been reached."},
             {WSAEFAULT, "The lpWSAData parameter is not a valid pointer."},
             {WSANOTINITIALISED, "A successful WSAStartup call must occur before using this function."},
             {WSAENETDOWN, "The network subsystem has failed."},
             {WSAEINPROGRESS, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."},
             {WSAEACCES, "An attempt was made to access a socket in a way forbidden by its access permissions."},
             {WSAEADDRINUSE, "Only one usage of each socket address (protocol/network address/port) is normally permitted."},
             {WSAEADDRNOTAVAIL, "The requested address is not valid in its context."},
             {WSAEFAULT, "The system detected an invalid pointer address in attempting to use a pointer argument in a call."},
             {WSAEINVAL, "An invalid argument was supplied."},
             {WSAENOBUFS, "An operation on a socket could not be performed because the system lacked sufficient buffer space or because a queue was full."},
             {WSAENOTSOCK, "An operation was attempted on something that is not a socket."},
             {WSAENOTCONN, "The socket is not connected."},
             {WSAEINTR, "The (blocking) call was canceled through WSACancelBlockingCall."},
             {WSAENETRESET, "For a connection-oriented socket, this error indicates that the connection has been broken due to keep-alive activity that detected a failure while the operation was in progress. For a datagram socket, this error indicates that the time to live has expired."},
             {WSAENOTSOCK, "The descriptor is not a socket."},
             {WSAEOPNOTSUPP, "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations."},
             {WSAESHUTDOWN, "The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_WRITE, SD_RECEIVE or SD_BOTH."},
             {WSAEWOULDBLOCK, "The socket is marked as nonblocking and the receive operation would block."},
             {WSAEMSGSIZE, "The message was too large to fit into the specified buffer and was truncated"},
             {WSAECONNABORTED, "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable."},
             {WSAETIMEDOUT, "The connection has been dropped because of a network failure or because the peer system failed to respond."},
             {WSAECONNRESET, "The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UDP-datagram socket, this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message."},
             {WSAEHOSTUNREACH, "A socket operation was attempted to an unreachable host."},
             {WSAENETUNREACH, "The network cannot be reached from this host at this time."},
             {WSAEDESTADDRREQ, "A destination address is required."},
             {WSAEAFNOSUPPORT, "Addresses in the specified family cannot be used with this socket."}
         };

#endif

}

#endif // SOCKET_ERRORS_H
