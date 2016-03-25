//
//  Client.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <sstream>

#include "client.hpp"

/**
 *  Constructor
 *
 *  @param iface Interface file name.
 *  @param rtable Routing table file name.
 *  @param hname Lan host file name.
 */
Client::Client(char *iface, char *rtable, char *host, bool debug_on) : debug("Client", debug_on) {
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
    
    if (debug.get_on())
        debug.print("Loading Hosts");
    
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
        
        if (debug.get_on()) {
            std::ostringstream ss;
            ss << "Loading Host: " << nhost->name
            << " Address: " << nhost->addr;
            debug.print(ss.str());
        }
    }
}


/**
 *  Reads symbolic link and loads Lan IP address
 *  and lan name.
 *
 *  @param lan      Symbolic link host name.
 */
void Client::load_lans() {
    
    if (debug.get_on())
        debug.print("Loading LANs");
    
    std::ifstream in;
//    const int BUF_SIZE = 100;
//    char buf[BUF_SIZE];

//    for (std::vector<Iface*>::iterator itr = ifaces_.begin(); itr != ifaces_.end(); ++itr) {
//        Host *n_host = new Host;
//        
//        n_host->name = (*itr)->iface_name;
//    }
    
    for (int i = 0; i < ifaces_.size(); ++i) {
        Host *n_host = new Host;
        n_host->name = ifaces_[i]->iface_name;
        
//        // Read Symlink
//        memset(buf, '\0', sizeof(char)*BUF_SIZE);
//        if (readlink(n_host->name.c_str(), buf, BUF_SIZE) == -1) {
//            my_error("Read symbolic link failed.");
//        }
        
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
        
        if (debug.get_on()) {
            std::ostringstream ss;
            ss << "Loading lan: " << n_host->name << " IP Address: "
            << n_host->addr << " on Port: " << n_host->port;
            debug.print(ss.str());
        }
        
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
    
    if (debug.get_on())
        debug.print("Loading Interfaces");
    
    std::ifstream in(iface);
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << iface << ".";
        my_error(out.str());
    }
    
    while (in.good()) {
        Iface *iface = new Iface;
        
        in >> iface->iface_name
           >> iface->ip_addr
           >> iface->net_mask
           >> iface->mac_addr
           >> iface->lan_name;
        
        // TODO: Parse addresses
        
        ifaces_.push_back(iface);
        
        if (debug.get_on()) {
            std::ostringstream ss;
            ss << "Loading Interface: " << iface->iface_name
            << " Network Mask: " << iface->net_mask
            << " MAC Address: " << iface->mac_addr
            << " Lan: " << iface->lan_name;
            debug.print(ss.str());
        }

    }
    
    in.close();
}

/**
 *  Reads and stores routes stored in Routing Table file.
 *
 *  @param rtable       Routing table file name.
 */
void Client::load_routes(char *rtable) {
    
    if (debug.get_on())
        debug.print("Loading Routes");
    
    std::ifstream in;
    
    in.open(rtable);
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << rtable << ".";
        my_error(out.str());
    }
    
    while(in.good()) {
        Rtable *route = new Rtable;
        
        in >> route->dest_addr
           >> route->next_hop
           >> route->net_mask
           >> route->iface_name;
        
        routes_.push_back(route);
        
        if (debug.get_on()) {
            std::ostringstream ss;
            ss << "Loading Routing Interface: " << route->iface_name
            << " Destination Address: " << route->dest_addr
            << " Network Mask: " << route->net_mask
            << " Next Hop: " << route->next_hop;
            debug.print(ss.str());
        }
    }
    
    in.close();
}


/**
 *  Binds socket to connected Bridge address and server.
 *
 *  @param bridge       Bridge structure connected to client
 */
void Client::socket_to_host(Host *bridge, SocketData *sd) {
    bzero(&sd->addr_, sizeof(sd->addr_));
    sd->addr_.sin_family = AF_INET;
    sd->addr_.sin_port = htons(bridge->port);
    
    if (inet_pton(AF_INET, bridge->addr.c_str(), (void*) &sd->addr_.sin_addr) == 0)
        my_error("pton error");
    
//    if (inet_aton(bridge->addr.c_str(), (struct in_addr *)&sd->addr_.sin_addr.s_addr) == 0)
//        my_error("aton error.");
    
    sd->fd = WSocket::wsocket(AF_INET, SOCK_STREAM, 0);
    
    if (debug.get_on()) {
        std::ostringstream ss;
        ss << "Binding socket " << sd->fd << " to " << bridge->addr.c_str();
        debug.print(ss.str());
    }
}




















