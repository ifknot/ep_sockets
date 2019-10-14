#include <iostream>

#include "socket_factory.h"

int main() {
    std::cout << "Easy Peasy Sockets" << std::endl;

#ifdef WIN32
    net::startup();
#endif

    std::string name(' ', 256);
    gethostname(&name[0], name.size());
    std::cout << name << std::endl;
    std::string HPZ600 = "192.168.1.201";
    std::string HPZ620 = "192.168.1.237";
    std::string THINKPAD = "192.168.1.225";
    std::string NEO4 = "192.168.1.182";


    std::cout << "udp client" << std::endl;
    net::udp_client_socket c{HPZ600, net::DEFAULT_PORT};
    c.write(" Hello from client " + name);
    std::cout << c.read() << std::endl;

/*
    std::cout << "udp server" << std::endl;
    net::udp_server_socket s{HPZ620, net::DEFAULT_PORT};
    std::cout << s.read_from() << std::endl;
    s.write_back("Hello from server " + name);
*/
/*
    std::cout << "tcp server" << std::endl;
    net::tcp_server_socket s(NEO4, net::DEFAULT_PORT);
    //net::tcp_server_socket s(net::LOOPBACK_ADDR, net::DEFAULT_PORT);
    auto c = s.accept_and_create_socket();
    std::cout << c.read() << std::endl;
    c.write("Hello from server " + name);
*/
/*
    std::cout << "tcp client" << std::endl;
    //net::tcp_client_socket c(net::LOOPBACK_ADDR, net::DEFAULT_PORT);
    net::tcp_client_socket c(NEO4, net::DEFAULT_PORT);
    c.write("Hello from client " + name);
    std::cout << c.read() << std::endl;
*/

    std::cout << "Any Key to Continue";
    std::cin.ignore();

#ifdef WIN32
    net::cleanup();
#endif

    return 0;

}
