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
#include <vector>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>

#include "utility.hpp"
#include "ip.hpp"

class Client {
public:
    Client(char *iface, char *rtable, char *hname);
    virtual ~Client() {}
    virtual void start() = 0;

protected:
    void load_lans(char *lan);
    void load_interfaces(char *iface);
    void load_routes(char *rtable);
    
    std::vector<Iface*> ifaces_;
    std::vector<Host*> lans_;
    std::vector<Rtable*> routes_;
    
    /*  FILE NAMES */
//    std::string iface_;         // Interface file name
//    std::string rtable_;        // Routing Table  file name
};

#endif /* Client_hpp */
