#ifndef WINSOCK_SPECIFIC_H
#define WINSOCK_SPECIFIC_H

#ifdef WIN32

#include <string>
#include <sstream>

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace net {

    //Windows specific...

    enum versions {v1_0 = 0x1, v1_1 = 0x101, v2_0 = 0x2, v2_1 = 0x102, v2_2 = 0x202 };

    /**
     * @brief startup - performs Windows specific initialization
     * @return string - winsock description and version info
     */
    static std::string startup() {
        WSADATA wsa_data;
        WSAStartup(v2_2, &wsa_data);
        std::stringstream ss;
        ss << "description: " << std::string(wsa_data.szDescription) << " - " << std::string(wsa_data.szSystemStatus);
        ss << "\nversion: " << std::to_string(wsa_data.wVersion & 0xFF) << "." << std::to_string((wsa_data.wVersion >> 8) & 0xFF);
        ss << "\nrequested: " << std::to_string(v2_2 & 0xFF) << "." << std::to_string((v2_2 >> 8) & 0xFF);
        return ss.str();
    }


    /**
     * @brief cleanup - performs Windows winsock specific closedown
     */
    static void cleanup() {
        WSACleanup();
    }
}

#endif

#endif // WINSOCK_SPECIFIC_H
