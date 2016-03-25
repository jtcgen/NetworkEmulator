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
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>

#include "w_socket.hpp"
#include "utility.hpp"
#include "ip.hpp"

class Client {
public:
    Client(char *iface,
           char *rtable,
           char *hname,
           bool debug_on);
    virtual ~Client() {}
    virtual void start() = 0;

protected:
    /*  CONFIGURATION FUNC  */
    
    /**
     *  Reads symbolic link and loads Lan IP address
     *  and lan name.
     *
     *  @param hname        Symbolic link host name.
     */
    void load_lans();
    
    /**
     *  Reads and stores interface data from iface filename
     *  and stores read.
     *
     *  @param iface        Interface file name
     */
    void load_interfaces(char *iface);
    
    /**
     *  Reads and stores Host name and IP address
     *
     *  @param host        Host file name
     */
    void load_hosts(char *host);
    
    /**
     *  Reads and stores routes stored in Routing Table file.
     *
     *  @param rtable       Routing table file name.
     */
    void load_routes(char *rtable);
    
    /**
     *  Binds socket to connected Bridge address and server.
     *
     *  @param bridge       Bridge structure connected to client
     */
    void socket_to_host(Host *bridge, SocketData *sd);
    
    /**
     *
     */
    
    std::vector<Iface*> ifaces_;            // Interface information
    std::vector<Host*> lans_;               // Connected bridges
    std::vector<Rtable*> routes_;           // Routing table information
    std::vector<SocketData*> sd_;           // Socket Data
    std::vector<Host*> hosts_;              // All active stations and routers
    
    Log debug;
};

#endif /* Client_hpp */
