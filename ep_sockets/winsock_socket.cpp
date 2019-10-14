
#ifdef WIN32

#define WIN32_LEAN_AND_MEAN

#include "winsock_socket.h"

#ifdef __MINGW32__
    #include "wsa_inetpton.h"
#endif

namespace net {

    base_socket::base_socket(const sockfd_t socket) {
        assert(_socket > 0);\
        _socket = socket;
        char optval = 1;
        setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	}

    base_socket::base_socket(const short address_family, const int socket_type, const int protocol) : _address_family(address_family), _socket_type(socket_type), _protocol(protocol) {
        assert(_socket > 0);
        _socket = socket(_address_family, _socket_type, _protocol);
        char optval = 1;
        setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int));
	}

    int base_socket::_assign_address(const std::string &address, const unsigned short port) {
        //using sockaddr_in makes assigning the address easier but must then reinterpret cast to sockaddr for winsock functions
        _addr.sin_family = static_cast<unsigned short>(_address_family);
		_addr.sin_port = htons(port);
#ifdef __MINGW32__
        return inet_pton(_address_family, address.c_str(), reinterpret_cast <char*>(&(_addr.sin_addr)));
#else
        return InetPtonW(_address_family, reinterpret_cast <const wchar_t*>(address.c_str()), reinterpret_cast <char*>(&(_addr.sin_addr)));
#endif

	}

    void base_socket::_reset_socket(sockfd_t socket) {
        char optval = 1; //option data depends on command here 1 enables reuse
        auto optlen = sizeof(char); //length of the option data here a single byte field
        if (setsockopt(socket,
                  SOL_SOCKET, //manipulates options at the sockets API level
                  SO_REUSEADDR, //Enables fast restart by telling kernel to reuse even if busy
                  &optval,
                  static_cast<int>(optlen)) == -1) {
                  throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
        }
    }

    std::string base_socket::_read(sockfd_t socket, const int flags) {
		std::array<char, DEFAULT_BUFFER_SIZE>buffer;
        auto i = recv(static_cast<unsigned int>(socket), &buffer.front(), static_cast<int>(buffer.size()), flags);
		if (i <= 0) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
		}
        buffer[static_cast<size_t>(i)] = '\0';
		return std::string(buffer.data());
	}

    long base_socket::_write(sockfd_t socket, const std::string& buffer, const int flags) {
        auto i = send(socket, buffer.c_str(), static_cast<int>(buffer.size()), flags);
		if (i <= 0) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
		}
		return i;
	}

    std::string base_socket::_read_from(sockfd_t socket, sockaddr_in &addr, const int flags) {
        std::array<char, DEFAULT_BUFFER_SIZE> buffer;
        int len_raddr = sizeof(_raddr);
        auto i = recvfrom(socket,
                          &buffer.front(),
                          static_cast<int>(buffer.size()),
                          flags,
                          reinterpret_cast<struct sockaddr*>(&addr),
                          &len_raddr);
        if(i <= 0) { //return the number of bytes received, or -1 if an error occurred.
            throw std::runtime_error(last_error());
        }
        return std::string{buffer.begin(), buffer.end()};//static_cast<size_t>(i)};
    }

    long base_socket::_write_back(sockfd_t socket, const std::string& buffer, sockaddr_in& addr, const int flags) {
        //transmit message in buffer
        auto i = sendto(socket,
                        buffer.c_str(),
                        static_cast<int>(buffer.size()),
                        flags,
                        reinterpret_cast<struct sockaddr*>(&addr),
                        sizeof(_raddr));
        if(i <= 0) { //return the number of bytes sent, or -1 if an error occurred.
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
        }
        return i;
    }

    void base_socket::connect_to(const std::string& address, const unsigned short port) {
        auto e = _assign_address(address, port);
		if (e < 0) {
            throw std::runtime_error(std::to_string(e) + " assign address " + last_error());
		}
		if (e == 0) {
			throw std::runtime_error(EMSG_INET_PTON);
		}
        if (connect(_socket, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
		}
	}

    void base_socket::bind_to(const std::string& address, const unsigned short port) {
        auto e = _assign_address(address, port);
		if (e < 0) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
		}
		if (e == 0) {
			throw std::runtime_error(EMSG_INET_PTON);
		}
        if (bind(_socket, reinterpret_cast<struct sockaddr*>(&_addr), sizeof(_addr)) < 0) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
		}
	}

    void base_socket::be_listening() {
        //marks the socket referred to by _socket as a passive socket, that is,
        //as a socket that will be used to accept incoming connection requests using accept
        if(listen(_socket, MAX_BACKLOG) < 0) {
        //MAX_BACKLOG defines the maximum length to which the queue of pending connections for _socket may grow
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
        }
    }

    bool base_socket::is_listening() const {
        char val;
        int len = sizeof(val);
        if (getsockopt(_socket,
                       SOL_SOCKET, //get options at the sockets API level
                       SO_ACCEPTCONN, //can it accepts connections i.e. passive listening
                       &val, &len) == SOCKET_ERROR) {
            throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
        }
        return val;
    }

    unsigned int base_socket::accept_and_create_sockfd() {
        assert(is_listening());
        socklen_t len_raddr = sizeof(_raddr);
        auto s = accept(_socket, //this bound and listening socket's file descriptor
                            reinterpret_cast<struct sockaddr*>(&_raddr), //filled in with the remote address of this peer socket
                            &len_raddr);
        if(s == INVALID_SOCKET) {
            throw std::runtime_error(last_error());
        }
        return static_cast<unsigned int>(s); //the newly created socket using the connected file descriptor
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
		case action_t::WRITE:
			if (shutdown(_socket, SD_SEND) < 0) {
                throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
			}
			break;
		case action_t::READ:
			if (shutdown(_socket, SD_RECEIVE) < 0) {
                throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
			}
			break;
		case action_t::READ_AND_WRITE:
			if (shutdown(_socket, SD_BOTH) < 0) {
                throw std::runtime_error(std::to_string(WSAGetLastError()) + " " + last_error());
			}
			break;
		}
	}

	base_socket::~base_socket() {
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
	}

	const std::string base_socket::last_error() {
		return error_messages[WSAGetLastError()];
	}

}

#endif // WIN32
