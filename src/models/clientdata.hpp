//
//  ClientData.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/10/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef ClientData_hpp
#define ClientData_hpp

#include <string>

#include "w_socket.hpp"
#include "ip.hpp"

class ClientData {
public:
    friend class Bridge;
    
    ClientData(char type, int fd, char *ip, short port) :
        type_(type), fd_(fd), port_(port), ip_(ip) { };
    
private:
    char type_;                         // Station or Router
    int fd_;
    Port port_;
    IPAddr ip_;
    
};

class BridgeData {
public:
    std::string name_;
    IPAddr addr_;
    Port port_;
    int fd_;                            // File descriptor
    socklen_t sock_size_;
    struct sockaddr_in sock_addr_;      // Contains server internet address information
};

#endif /* ClientData_h */
