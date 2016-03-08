//
//  Server.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Server_hpp
#define Server_hpp

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>

class Server {
public:
    Server(short port);
    virtual ~Server();
    virtual void start() = 0;
    
protected:
    struct hostent *serv_info_;     // Holds host info
    struct sockaddr_in serv_addr_;  // Contains server internet address information
    short port_;
    unsigned int listen_fd_;
//    std::vector<Client> clients_;

    
};

#endif /* Server_hpp */
