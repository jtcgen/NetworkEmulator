//
//  Client.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include "client.hpp"

/**
 *  Constructor
 *
 *  @param iface Interface file name.
 *  @param rtable Routing table file name.
 *  @param hname Lan host file name.
 */
Client::Client(char *iface, char *rtable, char *host) {
    /* initialization of hosts, interface, and routing tables */
    
    /* hook to the lans that the station should connected to
     * note that a station may need to be connected to multilple lans
     */
    
    /* monitoring input from users and bridges
     * 1. from user: analyze the user input and send to the destination if necessary
     * 2. from bridge: check if it is for the station. Note two types of data
     * in the ethernet frame: ARP packet and IP packet.
     *
     * for a router, it may need to forward the IP packet
     */

    load_interfaces(iface);
    load_hosts(host);
    load_lans();
    load_routes(rtable);
}

/**
 *  Reads and stores Host name and IP address
 *
 *  @param host        Host file name
 */
void Client::load_hosts(char *host) {
    std::ifstream in(host);
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << host << ".";
        my_error(out.str());
    }
    
    // Load store hosts
    while (in.good()) {
        Host *nhost = new Host;
        in >> nhost->name >> nhost->addr;
        hosts_.push_back(nhost);
    }
}


/**
 *  Reads symbolic link and loads Lan IP address
 *  and lan name.
 *
 *  @param lan      Symbolic link host name.
 */
void Client::load_lans() {
    std::ifstream in;


//    for (std::vector<Iface*>::iterator itr = ifaces_.begin(); itr != ifaces_.end(); ++itr) {
//        Host *n_host = new Host;
//        
//        n_host->name = itr->ifacename;
//    }
    
    for (int i = 0; i < ifaces_.size(); ++i) {
        Host *n_host = new Host;
        n_host->name = ifaces_[i]->ifacename;
        
        // Read Symlink
        
        // Open corresponding file
        in.open(n_host->name);
        if (!in) {
            std::ostringstream out;
            out << "Failed to open " << n_host->name << ".";
            my_error(out.str());
        }
        
        // Load data into host structure
        in >> n_host->addr >> n_host->port;
        lans_.push_back(n_host);
        
        in.close();
    }
}

/**
 *  Reads and stores interface data from iface filename
 *  and stores read.
 *  
 *  @param iface    Interface file name
 */
void Client::load_interfaces(char *iface) {
    std::ifstream in(iface);
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << iface << ".";
        my_error(out.str());
    }
    
    while (in.good()) {
        std::string ip_addr;        // Interface IP address
        std::string net_mask;       // Network mask
        std::string mac_addr;       // MAC address
        Iface *iface = new Iface;
        
        in >> iface->ifacename
           >> ip_addr
           >> net_mask
           >> mac_addr
           >> iface->lanname;
        
        // TODO: Parse addresses
        
        ifaces_.push_back(iface);
    }
    
    in.close();
}

/**
 *  Reads and stores routes stored in Routing Table file.
 *
 *  @param rtable       Routing table file name.
 */
void Client::load_routes(char *rtable) {
    std::ifstream in;
    
    in.open(rtable);
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << rtable << ".";
        my_error(out.str());
    }
    
    while(in.good()) {
        std::string dest_subnet;    // Destination network address
        std::string next_hop;       // Next-hop router address
        std::string mask;           // Network mask
        
        Rtable *route = new Rtable;
        
        in >> dest_subnet
           >> next_hop
           >> mask
           >> route->ifacename;
        
        // TODO: Parse addresses
        
        routes_.push_back(route);
    }
    
    in.close();
}


/**
 *  Binds socket to connected Bridge address and server.
 *
 *  @param bridge       Bridge structure connected to client
 */
void socket_to_host(Host *bridge, SocketData *sd) {
    bzero(&sd->addr_, sizeof(sd->addr_));
    sd->addr_.sin_family = AF_INET;
    sd->addr_.sin_port = htons(bridge->port);
    
    if (inet_pton(AF_INET, bridge->addr.c_str(), (void*) &sd->addr_.sin_addr) == 0)
        my_error("pton error");
    
//    if (inet_aton(bridge->addr.c_str(), (struct in_addr *)&sd->addr_.sin_addr.s_addr) == 0)
//        my_error("aton error.");
    
    sd->fd = WSocket::wsocket(AF_INET, SOCK_STREAM, 0);
}




















