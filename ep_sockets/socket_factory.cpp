#include "socket_factory.h"

namespace net {

    //------------udp_server_socket implementation------------
    udp_server_socket::multi_socket(const std::string addr, const unsigned short port):
        base_socket(AF_INET, SOCK_DGRAM, 0) {
        bind_to(addr, port);
    }

    std::string udp_server_socket::read_from(const int flags) {
        return  base_socket::read_from(flags);
    }

    long udp_server_socket::write_back(const std::string& buffer, const int flags) {
        return base_socket::write_back(buffer, flags);
    }

    //------------udp_client_socket implementation------------
    udp_client_socket::multi_socket(const std::string addr, const unsigned short port):
        base_socket(AF_INET, SOCK_DGRAM, 0) {
        connect_to(addr, port);
    }

    std::string udp_client_socket::read(const int flags) const  {
        return base_socket::read(flags);
    }

    long udp_client_socket::write(const std::string& buffer, const int flags) const {
        return  base_socket::write(buffer, flags);
    }

    //------------tcp_active_socket implementation------------
    tcp_active_socket::multi_socket(unsigned int socket): base_socket(socket) {}

    std::string tcp_active_socket::read(const int flags) const  {
        return base_socket::read(flags);
    }

    long tcp_active_socket::write(const std::string& buffer, const int flags) const {
        return  base_socket::write(buffer, flags);
    }

    //------------tcp_server_socket implementation------------
    tcp_server_socket::multi_socket(const std::string addr, const unsigned short port):
        base_socket(AF_INET, SOCK_STREAM, 0) {
        bind_to(addr, port);
        be_listening();
    }

    tcp_active_socket tcp_server_socket::accept_and_create_socket()  {
        return tcp_active_socket(base_socket::accept_and_create_sockfd());
    }

    void tcp_server_socket::stop(action_t action) {
        base_socket::stop(action);
    }

    //------------tcp_client_socket implementation------------
    tcp_client_socket::multi_socket(const std::string addr, const unsigned short port):
        base_socket(AF_INET, SOCK_STREAM, 0) {
        connect_to(addr, port);
    }

    std::string tcp_client_socket::read(const int flags) const  {
        return base_socket::read(flags);
    }

    long tcp_client_socket::write(const std::string& buffer, const int flags) const {
        return  base_socket::write(buffer, flags);
    }

}
