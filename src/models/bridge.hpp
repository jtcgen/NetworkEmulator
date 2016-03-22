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
#include <iostream>
#include <arpa/inet.h>  // inet_ntop
#include <string.h>
#include <vector>

#include "w_socket.hpp"
#include "utility.hpp"
#include "clientdata.hpp"
#include "ip.hpp"
#include "ether.hpp"

class Bridge {
public:
    Bridge(const char *lan_name,
           int num_ports,
           bool debug_on);
    
    ~Bridge();
    
    /**
     *  Initiates bridge. Listens for open/close requests from stations and
     *  routers, and regular data packets.
     */
    void start();
    
private:
    /**
     *  Collects requested client address information.
     *
     *  @param cli_fd       Client File descriptor.
     *  @param cli_addr     Client address information.
     *  @param msg          Packet frame sent from client.   TODO: ADD parameters
     *  @return ClientData
     */
    ClientData package_client_data(int cli_fd, struct sockaddr_in cli_addr);
    
    /**
     *  Establishes bridge address information.
     */
    void setup_server_info();
    
    /**
     *  Establishes socket information for incoming and outgoing
     *  information.
     */
    void setup_socket();
    
    /**
     *  Creates the symbolic link to its address and port number
     *  so that others (stations/routers) can connect to it.
     */
    void create_symlink();
    void remove_client(ClientData cli, int index, fd_set &all_set);
    
    /*  CLIENT DATA     */
    const int NUM_PORTS_;                   // Max capacity of ports
    int curr_ports_;                        // Current number of conn. ports
    std::vector<ClientData> clients_;       // Corresponding station/router data
    
    /*  BRIDGE INFORMATION  */
    std::string lan_name_;
    struct hostent *info_;                  // Holds host info
    struct sockaddr_in addr_;               // Contains server internet address information
    int port_;                              // Port binded to socket
    unsigned int listen_fd_;                // Actively listening on listen socket
    
    Log debug;
};

#endif /* Bridge_hpp */
