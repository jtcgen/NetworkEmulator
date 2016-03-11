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
#include "AddrData.hpp"
#include "ClientData.hpp"

class Bridge {
public:
    Bridge(char *lan_name, int num_ports, bool debug_on);
    ~Bridge();
    
    void start();
    
private:
    void setup_server_info();
    void setup_socket();
    void create_symlink();
    
    
    const int NUM_PORTS_;
    
    std::vector<ClientData> clients_;
    std::string lan_name_;
    AddrData *addr_;
    unsigned int listen_fd_;
    Log debug;
};

#endif /* Bridge_hpp */
