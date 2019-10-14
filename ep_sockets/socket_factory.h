#ifndef SOCKET_FACTORY_NEW_H
#define SOCKET_FACTORY_NEW_H

/**
 * @brief A template metaprogramming cross platform socket class factory.
 * @version 0.5 (udp & tcp on linux and windows)
 *
    __linux__       Defined on Linux
    __sun           Defined on Solaris
    __FreeBSD__     Defined on FreeBSD
    __NetBSD__      Defined on NetBSD
    __OpenBSD__     Defined on OpenBSD
    __APPLE__       Defined on Mac OS X
    __hpux          Defined on HP-UX
    __osf__         Defined on Tru64 UNIX (formerly DEC OSF1)
    __sgi           Defined on Irix
    _AIX            Defined on AIX
  */

#ifdef WIN32

    #include "winsock_socket.h"

#elif __linux__

    #include "linux_socket.h"

#elif __APPLE__

    #include "darwin_socket.h"

#endif

namespace net {

    //base factory template class
    template<net::protocol_t, net::role_t, net::family_t, net::socket_t>
    struct multi_socket{};

    //------------product definitions------------
    //udp sockets
    using udp_server_socket = multi_socket<net::protocol_t::UDP, net::role_t::server, net::family_t::IPv4, net::socket_t::DGRAM>;
    using udp_client_socket = multi_socket<net::protocol_t::UDP, net::role_t::client, net::family_t::IPv4, net::socket_t::DGRAM>;
    //tcp sockets
    using tcp_server_socket = multi_socket<net::protocol_t::TCP, net::role_t::server, net::family_t::IPv4, net::socket_t::STREAM>;
    using tcp_active_socket = multi_socket<net::protocol_t::TCP, net::role_t::active, net::family_t::IPv4, net::socket_t::STREAM>;
    using tcp_client_socket = multi_socket<net::protocol_t::TCP, net::role_t::client, net::family_t::IPv4, net::socket_t::STREAM>;

    //------------udp_server_socket template------------
    template<>
    struct multi_socket<net::protocol_t::UDP, net::role_t::server, net::family_t::IPv4, net::socket_t::DGRAM>:
        private base_socket {

        multi_socket(const std::string addr, const unsigned short port);

        std::string read_from(const int flags = 0) override final;

        long write_back(const std::string& buffer, const int flags = 0) override final;

        virtual ~multi_socket() override = default;

    };

    //------------udp_client_socket template------------
    template<>
    struct multi_socket<net::protocol_t::UDP, net::role_t::client, net::family_t::IPv4, net::socket_t::DGRAM>:
        private base_socket {

        multi_socket(const std::string addr, const unsigned short port);

        std::string read(const int flags = 0) const override final;

        long write(const std::string& buffer, const int flags = 0) const override final;

        virtual ~multi_socket() override = default;

    };

    //------------tcp_active_socket template------------
    template<>
    struct multi_socket<net::protocol_t::TCP, net::role_t::active, net::family_t::IPv4, net::socket_t::STREAM>:
        private base_socket {

        explicit multi_socket(unsigned int socket);

        multi_socket (const multi_socket&) = default;

        multi_socket& operator= (const multi_socket&) = default;

        std::string read(const int flags = 0) const override final;

        long write(const std::string& buffer, const int flags = 0) const override final;

        virtual ~multi_socket() override = default;
    };

    //------------tcp_server_socket template------------
    template<>
    struct multi_socket<net::protocol_t::TCP, net::role_t::server, net::family_t::IPv4, net::socket_t::STREAM>:
        private base_socket {

        multi_socket(const std::string addr, const unsigned short port);

        tcp_active_socket accept_and_create_socket();

        void stop(action_t action) override final;

        virtual ~multi_socket() override = default;
    };

    //------------tcp_client_socket template------------
    template<>
    struct multi_socket<net::protocol_t::TCP, net::role_t::client, net::family_t::IPv4, net::socket_t::STREAM>:
        private base_socket {

        multi_socket(const std::string addr, const unsigned short port);

        std::string read(const int flags = 0) const override final;

        long write(const std::string& buffer, const int flags = 0) const override final;

        virtual ~multi_socket() override = default;

    };

}

#endif // SOCKET_FACTORY_NEW_H
