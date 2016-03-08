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
#include <arpa/inet.h>

class Client {
public:
    Client(char *iface, char *rtable, char *host_name);
    Client(char* serv_name, char* port);
    virtual ~Client() {}
    virtual void start() = 0;
protected:
    char iface_;
    int serv_fd_;
    char serv_name_[100];
    int serv_port_;
    struct sockaddr_in serv_addr_;
};

#endif /* Client_hpp */
