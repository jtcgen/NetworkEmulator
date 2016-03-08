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


#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <vector>

class Bridge {
public:
    Bridge(short port);
    Bridge(char *lan_name, int num_ports);
    ~Bridge();
    void start();
    
private:
    
    
    
    struct hostent *serv_info_;     // Holds host info
    struct sockaddr_in serv_addr_;  // Contains server internet address information
    short port_;
    unsigned int listen_fd_;
    //    std::vector<Client> clients_;
    int num_ports_;
    char *lan_name_;
    
};

#endif /* Bridge_hpp */
