//
//  Bridge.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Bridge_hpp
#define Bridge_hpp

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>

#include "utility.hpp"
#include "Client.hpp"

class Bridge {
public:
    Bridge(char *lan_name, int num_ports, bool debug_on);
    ~Bridge();
    
    void start();
    
private:
    void setup_server_info();
    void setup_socket();
    void create_symlink();
    
    struct hostent *info_;     // Holds host info
    struct sockaddr_in addr_;  // Contains server internet address information
    unsigned short port_;
    unsigned int listen_fd_;
    std::vector<Client> clients_;
    int num_ports_;
    char *lan_name_;
    Log debug;
};

#endif /* Bridge_hpp */
