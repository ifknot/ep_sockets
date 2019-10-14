#ifndef WINSOCK_SOCKET_H
#define WINSOCK_SOCKET_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN


#include <atomic>
#include <string>
#include <sstream>
#include <stdexcept>
#include <map>
#include <array>
#include "assert.h"

#include "string.h"

#include "winsock_specific.h"
#include "socketable.h"

namespace net {

    /**
     * @brief The multipurpose base_socket class provides WINDOWS OS specific *both* client and server behaviour for (all) protocols.
     * @note Only TCP & UDP implemented so far
     * @implements socketable interface
     * @version 0.5
     * Although somewhat monolithic it encapsulates all of the OS specific networking logic in one place
     * which simplifies cross-platform development and makes for a very flexible base for a factory to use.
     */
    class base_socket: public socketable {

        using sockfd_t = SOCKET;

        static const int MAX_BACKLOG = SOMAXCONN;
        //static constexpr int MAX_BACKLOG = SOMAXCONN_HINT(200);

    public:

        /**
         * @brief base_socket::base_socket - constructs an easily restartable socket from a socket file descriptor.
         * @param socket - sockfd_t socket file descriptor
         */
        explicit base_socket(const sockfd_t socket);

        /**
         * @brief base_socket
         * @param address_family
         * @param socket_type
         * @param protocol
         */
        base_socket(const short address_family, const int socket_type, const int protocol);

        base_socket (const base_socket&) = default;

        base_socket& operator= (const base_socket&) = default;

        /**
         * @brief connect_to - creates a socket file descriptor for this socket from a text format Internet address and port.
         * @note on failure throws an exception containing the system call error message.
         * @param address - the address to which datagrams are sent by default, and the only address from which datagrams are received.
         * @param port - port number
         */
        void connect_to(const std::string& address, const unsigned short port) override;

        /**
         * @brief bind_to - naming a port by creates a socket file descriptor for this socket from a text format Internet address and port.
         * It is normally necessary to assign a local address using bind before a SOCK_STREAM socket may receive connections.
         * @param address - text format Internet address
         * @param port - port number
         */
        void bind_to(const std::string& address, const unsigned short port) override;

        /**
         * @brief server_be_listening - marks this socket as a *passive socket*, that will be used to accept incoming connection requests using accept.
         */
        void be_listening() override final;

        /**
         * @baction_trief server_is_listening
         * @return true if passive socket that can accept connection(s)
         */
        bool is_listening() const override final;

        /**
         * @brief server_accept_and_create_fd - used with connection-based socket types
         * (SOCK_STREAM, SOCK_SEQPACKET) in a server role it extracts the first connection request on
         * the queue of pending connections for this first marked as listening socket and creates a
         * new connected socket, and returns a new file descriptor referring to that socket.
         * @note the newly created socket is *not* in the listening state whilst this original socket is unaffected.
         * @return int newly created socket file descriptor
         */
        unsigned int accept_and_create_sockfd() override;

        /**
         * @brief read - read a message from this socket if connected
         * @param flags - formed by ORing one or more of: MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL - defaults to none
         * @return string - the message
         */
        std::string read(const int flags = 0) const override;

        /**
         * @brief write - write a message to this socket if connected
         * @param buffer - the message string to write
         * @param flags - formed by ORing one or more of: MSG_CONFIRM, MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_MORE, MSG_NOSIGNAL, MSG_OOB - defaults to none.
         * @return long - the number of bytes written
         */
        long write(const std::string& buffer, const int flags = 0) const override;

        /**
         * @brief read_from - receive data on a socket whether or not it is connection-oriented.
         * @param flags - formed by ORing one or more of: MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL, MSG_EOR, MSG_TRUNC, MSG_CTRUNC, MSG_ERRQUEUE - defaults to none
         * @return string - the message
         */
        std::string read_from(const int flags = 0) override;

        /**
         * @brief write_back - transmit a message to back the socket that has been read/read_from
         * @param buffer - the message string to write
         * @param flags - formed by ORing one or more of: MSG_CONFIRM, MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_MORE, MSG_NOSIGNAL, MSG_OOB - defaults to none.
         * @return long - the number of bytes written
         */
        long write_back(const std::string& buffer, const int flags = 0) override;

        /**
         * @brief reset - enable kernal reuse addresses and ports that may already be active/tied
         */
        void reset() override final;

        /**
         * @brief stop - causes all or part of a full-duplex connection on this socket to be shut down.
         * @param action - SHUT_RD, further receptions will be disallowed; SHUT_WR further transmissions will be disallowed; SHUT_RDWR, further receptions and transmissions will be disallowed.
         */
        void stop(action_t action) override;

        /**
         * @brief last_error - convert the error number stored in system errno into its human readable text form
         * @return string - error description
         */
        static const std::string last_error();

        virtual ~base_socket() override;

    private:

        /**
         * @brief _assign_address - system call helper converts an Internet address in its standard text format into its numeric binary form
         * @param address - text format Internet address
         * @param port - IP port
         * @return int - Internet address in its numeric binary form
         */
        int _assign_address(const std::string& address, const unsigned short port);

        /**
         * @brief _reset_socket - system call helper to enable fast restart by enabling kernel to reuse addresses and ports that may be active.
         * @param socket - socket file descriptor
         */
        static void _reset_socket(sockfd_t socket);

        /**
         * @brief _read - static system call helper receives message from a connected socket, behaviour dictated by flag options.
         * If no messages are available at the socket, the receive calls wait for a message to arrive (unless the socket is nonblocking).
         * @note May be used to receive data on both connectionless and connection-oriented sockets.
         * @param socket - the *connected* socket file descriptor
         * @param flags - formed by ORing one or more of: MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL
         * @return string - normally returns any data available, up to the requested amount, rather than waiting for receipt of the full amount requested.
         */
        static std::string _read(sockfd_t socket, const int flags);

        /**
         * @brief _write - static system call helper transmits a message to a socket, behaviour dictated by flag options.
         * @param socket - the *connected* sending socket file descriptor
         * @param buffer - the std:string to store the message
         * @param flags - formed by ORing one or more of: MSG_CONFIRM, MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_MORE, MSG_NOSIGNAL, MSG_OOB
         * @return long - on success return the number of bytes sent, else -1
         */
        static long _write(sockfd_t socket, const std::string& buffer, const int flags);

        /**
         * @brief _read_from static system call helper receives message from specific address, behaviour dictated by flag options.
         * @param socket - the socket file descriptor
         * @param addr - target address
         * @param flags - action flags
         * @return string - normally returns any data available, up to the requested amount, rather than waiting for receipt of the full amount requested.
         */
        static std::string _read_from(sockfd_t socket, sockaddr_in& addr, const int flags);

        /**
         * @brief _write_back - static system call helper transmits a message back to socket address stored by either accept or read_from, behaviour dictated by flag options.
         * @param socket
         * @param addr
         * @param flags
         * @return long - on success return the number of bytes sent, else -1
         */
        static long _write_back(sockfd_t socket, const std::string& buffer, sockaddr_in& addr, const int flags);

        short _address_family;
        int _socket_type;
        int _protocol;
        sockfd_t _socket;
        sockaddr_in _addr;
        sockaddr_in _raddr;

    };

}

#endif // WIN32


#endif // WINSOCK_SOCKET_H
