//
//  Client.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Client_hpp
#define Client_hpp

#include <stdio.h>
#include <string>
#include <arpa/inet.h>

#include "AddrData.hpp"

class Client {
public:
    Client(char *iface, char *rtable, char *host_name);
    Client(char* serv_name, char* port);
    virtual ~Client() {}
    virtual void start() = 0;
protected:
    std::string iface_;     // Interface file name
    std::string rtable_;    // Routing Table  file name
    std::string hname_;     // Host file name
    
//    AddrData *
//    int serv_port_;
//    struct sockaddr_in serv_addr_;
};

#endif /* Client_hpp */
