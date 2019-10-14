#include "linux_socket.h"

#ifdef __linux__

namespace net {

    base_socket::base_socket(sockfd_t socket) {
        assert(socket >= 0);
        _socket = socket; //socket file descriptor
    }

    base_socket::base_socket(sa_family_t address_family, int socket_type, int protocol):
        _address_family(address_family), _socket_type(socket_type), _protocol(protocol) {
        _socket = static_cast<sockfd_t>(socket(_address_family, _socket_type, _protocol));
        assert(_socket >= 0);
    }

    int base_socket::_assign_address(const std::string &address, const unsigned short port) {
        _addr.sin_family = _address_family;
        _addr.sin_port = htons(port); //ensure numbers are stored in memory in network byte order (bigendian) as opposed to machine order (eg little endian if intel)
        return inet_pton(_address_family, address.c_str(), &(_addr.sin_addr)); //convert the Internet address in its standard text format into its numeric binary form
    }

    void base_socket::_reset_socket(sockfd_t socket) {
        char optval = 1; //option data depends on command here 1 enables reuse
        socklen_t optlen = sizeof(char); //length of the option data here a single byte field
        if (setsockopt(static_cast<int>(socket),
                  SOL_SOCKET, //manipulates options at the sockets API level
                  (SO_REUSEPORT | SO_REUSEADDR), //Enables fast restart by telling kernel to reuse even if busy
                  &optval, optlen) == -1) {
                  throw std::runtime_error(last_error());
        }
    }

    std::string base_socket::_read(sockfd_t socket, const int flags) {
        std::array<char, DEFAULT_BUFFER_SIZE> buffer;
        auto i = recv(static_cast<int>(socket),
                      &buffer.front(),
                      buffer.size(),
                      flags); //place message into buffer
        if(i <= 0) { //return the number of bytes received, or -1 if an error occurred.
            throw std::runtime_error(last_error());
        }
        return std::string(buffer.begin(), static_cast<size_t>(i));
    }

    long base_socket::_write(sockfd_t socket, const std::string& buffer, const int flags) {
        //transmit message in buffer
        auto i = send(static_cast<int>(socket),
                      buffer.c_str(),
                      buffer.size(),
                      flags);
        if(i <= 0) { //return the number of bytes sent, or -1 if an error occurred.
            throw std::runtime_error(last_error());
        }
        return i;
    }

    std::string base_socket::_read_from(sockfd_t socket, sockaddr_in &addr, const int flags) {
        std::array<char, DEFAULT_BUFFER_SIZE> buffer;
        socklen_t len_raddr = sizeof(_raddr);
        auto i = recvfrom(static_cast<int>(socket),
                          &buffer.front(),
                          buffer.size(),
                          flags,
                          reinterpret_cast<struct sockaddr*>(&addr),
                          &len_raddr);
        if(i <= 0) { //return the number of bytes received, or -1 if an error occurred.
            throw std::runtime_error(last_error());
        }
        return std::string(buffer.begin(), static_cast<size_t>(i));
    }

    long base_socket::_write_back(sockfd_t socket, const std::string& buffer, sockaddr_in& addr, const int flags) {
        //transmit message in buffer
        auto i = sendto(static_cast<int>(socket),
                        buffer.c_str(),
                        buffer.size(),
                        flags,
                        reinterpret_cast<struct sockaddr*>(&addr),
                        sizeof(_raddr));
        if(i <= 0) { //return the number of bytes sent, or -1 if an error occurred.
            throw std::runtime_error(last_error());
        }
        return i;
    }

    void base_socket::connect_to(const std::string& address, const unsigned short port) {
        auto e = _assign_address(address, port); //use helper to convert text address and port to its numeric form
        if(e < 0) {
            throw std::runtime_error(last_error());
        }
        if(e == 0) {
            throw std::runtime_error(EMSG_INET_PTON);
        }
        //connects the socket referred to by the file descriptor _socket to the address specified by _addr.
        //the format of the address in _addr is determined by the address space of the socket
        if(connect(static_cast<int>(_socket), reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
            throw std::runtime_error(last_error());
        }
    }

    void base_socket::bind_to(const std::string& address, const unsigned short port) {
        auto e = _assign_address(address, port); //use helper to convert text address and port to its numeric form
        if(e < 0) {
            throw std::runtime_error(last_error());
        }
        if(e == 0) {
            throw std::runtime_error(EMSG_INET_PTON);
        }
        //bind assigns the address specified by _addr to the socket referred to by the file descriptor _socket.
        if(bind(static_cast<int>(_socket), reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
            throw std::runtime_error(last_error());
        }
    }

    unsigned int base_socket::accept_and_create_sockfd() {
        assert(is_listening());
        socklen_t len_raddr = sizeof(_raddr);
        auto s = accept(static_cast<int>(_socket), //this bound and listening socket's file descriptor
                            reinterpret_cast<struct sockaddr*>(&_raddr), //filled in with the remote address of this peer socket
                            &len_raddr);
        if(s < 0) {
            throw std::runtime_error(last_error());
        }
        return static_cast<sockfd_t>(s); //the newly created socket using the connected file descriptor
    }

    void base_socket::be_listening() {
        //marks the socket referred to by _socket as a passive socket, that is,
        //as a socket that will be used to accept incoming connection requests using accept
        if(listen(static_cast<int>(_socket), MAX_BACKLOG) < 0) {
        //MAX_BACKLOG defines the maximum length to which the queue of pending connections for _socket may grow
            throw std::runtime_error(last_error());
        }
    }

    bool base_socket::is_listening() const {
        int val;
        socklen_t len = sizeof(val);
        if (getsockopt(static_cast<int>(_socket),
                       SOL_SOCKET, //get options at the sockets API level
                       SO_ACCEPTCONN, //can it accepts connections i.e. passive listening
                       &val, &len) == -1) {
            throw std::runtime_error(last_error());
        } else if (val) { //non-zero socket is listening
            return true;
        } else {
            return false;
        }
    }

    std::string base_socket::read(const int flags) const {
        return _read(_socket, flags);
    }

    long base_socket::write(const std::string& buffer, const int flags) const {
        return _write(_socket, buffer, flags);
    }

    std::string base_socket::read_from(const int flags) {
        return _read_from(_socket, _raddr, flags);
    }

    long base_socket::write_back(const std::string& buffer, const int flags) {
        return _write_back(_socket, buffer, _raddr, flags);
    }

    void base_socket::reset() {
        _reset_socket(_socket);
    }

    void base_socket::stop(action_t action) {
        switch (action) {
        case action_t::WRITE: //further receptions will be disallowed
            if(shutdown(static_cast<int>(_socket), SHUT_WR) < 0) {
                throw std::runtime_error(last_error());
            }
            break;
        case action_t::READ: //further transmissions will be disallowed
            if(shutdown(static_cast<int>(_socket), SHUT_RD) < 0) {
                throw std::runtime_error(last_error());
            }
            break;
        case action_t::READ_AND_WRITE: //further receptions and transmissions will be disallowed
            if(shutdown(static_cast<int>(_socket), SHUT_RDWR) < 0) { //
                throw std::runtime_error(last_error());
            }
            break;
        }
    }

    const std::string base_socket::last_error() {
        std::array<char, DEFAULT_BUFFER_SIZE> msg;
        if (strerror_r(errno, &msg.front(), msg.size()) == 0) { //The XSI-compliant version strerror
            return std::to_string(errno) + " " + std::string(msg.begin(), msg.end());
        } else {
            return EMSG_UNKNOWN;
        }
    }

    base_socket::~base_socket() {
        shutdown(static_cast<int>(_socket), SHUT_RDWR);
        close(static_cast<int>(_socket));
    }

}

#endif
