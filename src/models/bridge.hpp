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
#include <map>
#include <chrono>       // timer

#include "w_socket.hpp"
#include "utility.hpp"
#include "clientdata.hpp"
#include "ip.hpp"
#include "ether.hpp"

typedef std::chrono::milliseconds milliseconds;
typedef std::map<MacAddr, milliseconds> BridgeTableValues;
typedef std::map<MacAddr, milliseconds>::iterator BridgeTableValuesItr;
typedef std::map<Port, BridgeTableValues > BridgeTable;
typedef std::map<Port, BridgeTableValues >::iterator BridgeTableItr;

class Bridge {
public:
    Bridge(const char *lan_name,
           int num_ports);
    
    ~Bridge();
    
    /**
     *  Initiates bridge. Listens for open/close requests from stations and
     *  routers, and regular data packets.
     */
    void start();
    
private:
    /**
     *  Checks for inactive station ins ARP cache.
     */
    void monitor_arp_cache();
    
    /**
     *  Adds incoming port and MAC address to bridge table.
     *
     *  @param port         Incoming station port
     */
    void add_port(Port port);
    
    /**
     *  Checks bridge table for associated station/router.
     *
     *  @param port         Incoming station port
     *  @param mac          Incoming MAC address
     *  @return             True if contains, else false
     */
    int has_arp_entry(Port port, MacAddr mac);
    
    /**
     *  Adds incoming port and MAC address to bridge table.
     *
     *  @param port         Incoming station port
     *  @param station      Incoming station name
     *  @param mac          Incoming station MAC address
     */
    void add_arp_entry(Port port, MacAddr mac);
    
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

    /**
     Client disconnected, removes infor from client vector.
     
     @param cli ClientData object.
     @param index Location in vector.
     @param all_set
     @return none
     */
    void remove_client(ClientData cli, int index, fd_set &all_set);
    
    /**
     *  Outputs the contents of the self-learning table;
     */
    void print_btable();
    
    /*  CLIENT DATA     */
    const milliseconds ARP_TIME;            // Allowed period of inactivity
    const int NUM_PORTS_;                   // Max capacity of ports
    int curr_ports_;                        // Current number of conn. ports
    std::vector<ClientData> clients_;       // Corresponding station/router data
    
    /*  BRIDGE INFORMATION  */
    std::string lan_name_;
    struct hostent *info_;                  // Holds host info
    struct sockaddr_in addr_;               // Contains server internet address information
    int port_;                              // Port binded to socket
    unsigned int listen_fd_;                // Actively listening on listen socket
    
    // TODO: might have to be a key: port, value: <key=station,value=MAC_ADDR>
    BridgeTable btable_;                    // Bridge table for self-learning
    
    Log debug;
};

#endif /* Bridge_hpp */
