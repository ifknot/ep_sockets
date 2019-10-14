#ifndef SOCKETABLE_H
#define SOCKETABLE_H

#include <string>

#include "socket_errors.h"
#include "socket_constants.h"

namespace net {

    struct socketable {

        /**
         * @brief connect_to - creates a socket file descriptor for this socket from a text format Internet address and port.
         * @version 0.5
         * @note on failure throws an exception containing the system call error message.
         * @param address - the address to which datagrams are sent by default, and the only address from which datagrams are received.
         * @param port - port number
         */
        virtual void connect_to(const std::string& address, const unsigned short port) = 0;

        /**
         * @brief bind_to - naming a port by creates a socket file descriptor for this socket from a text format Internet address and port.
         * It is normally necessary to assign a local address using bind before a SOCK_STREAM socket may receive connections.
         * @param address - text format Internet address
         * @param port - port number
         */
        virtual void bind_to(const std::string& address, const unsigned short port) = 0;

        /**
         * @brief server_be_listening - marks this socket as a *passive socket*, that will be used to accept incoming connection requests using accept.
         */
        virtual void be_listening() = 0;

        /**
         * @brief server_is_listening
         * @return true if passive socket that can accept connection(s)
         */
        virtual bool is_listening() const  = 0;

        /**
         * @brief server_accept_and_create_fd - used with connection-based socket types
         * (SOCK_STREAM, SOCK_SEQPACKET) in a server role it extracts the first connection request on
         * the queue of pending connections for this first marked as listening socket and creates a
         * new connected socket, and returns a new file descriptor referring to that socket.
         * @note the newly created socket is *not* in the listening state whilst this original socket is unaffected.
         * @return int newly created socket file descriptor
         */
        virtual unsigned int accept_and_create_sockfd() = 0;

        /**
         * @brief read - read a message from this socket if connected
         * @param flags - formed by ORing one or more of: MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL - defaults to none
         * @return string - the message
         */
        virtual std::string read(const int flags = 0) const = 0;

        /**
         * @brief write - write a message to this socket if connected
         * @param buffer - the message string to write
         * @param flags - formed by ORing one or more of: MSG_CONFIRM, MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_MORE, MSG_NOSIGNAL, MSG_OOB - defaults to none.
         * @return long - the number of bytes written
         */
        virtual long write(const std::string& buffer, const int flags = 0) const = 0;

        /**
         * @brief read_from - receive data on a socket whether or not it is connection-oriented.
         * @param flags - formed by ORing one or more of: MSG_CMSG_CLOEXEC, MSG_DONTWAIT, MSG_ERRQUEUE, MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL, MSG_EOR, MSG_TRUNC, MSG_CTRUNC, MSG_ERRQUEUE - defaults to none
         * @return string - the message
         */
        virtual std::string read_from(const int flags = 0) = 0;

        /**
         * @brief write_back - transmit a message to back the socket that has been read/read_from
         * @param buffer - the message string to write
         * @param flags - formed by ORing one or more of: MSG_CONFIRM, MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_MORE, MSG_NOSIGNAL, MSG_OOB - defaults to none.
         * @return long - the number of bytes written
         */
        virtual long write_back(const std::string& buffer, const int flags = 0) = 0;

        /**
         * @brief reset - enable kernal reuse addresses and ports that may already be active/tied
         */
        virtual void reset() = 0;

        /**
         * @brief stop - causes all or part of a full-duplex connection on this socket to be shut down.
         * @param action - SHUT_RD, further receptions will be disallowed; SHUT_WR further transmissions will be disallowed; SHUT_RDWR, further receptions and transmissions will be disallowed.
         */
        virtual void stop(action_t action) = 0;

        virtual ~socketable() = default;

    };

}

#endif // SOCKETABLE_H
