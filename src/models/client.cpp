//
//  Client.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include "client.hpp"

/**
    Constructor
 
    @param iface Interface file name.
    @param rtable Routing table file name.
    @param hname Lan host file name.
    @return none
 */
Client::Client(char *iface, char *rtable, char *hname) {
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
    load_lans(hname);
    load_routes(rtable);
}

/**
    Reads symbolic link and loads Lan IP address
    and lan name.
 
    @param hname Symbolic link host name.
    @return none.
 */
void Client::load_lans(char *lan) {
    // Read symbolic link
    std::ifstream in;
    
    in.open(lan);
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << lan << ".";
        my_error(out.str());
    }
    
    while(in.good()) {
        
    }
    
}

/**
    Reads and stores interface data from iface filename
    and stores read.

    @param none.
@return none
 */
void Client::load_interfaces(char *iface) {
    std::ifstream in;
    
    in.open(iface);
    
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
    Reads and stores routes stored in Routing Table file.
 
    @param rtable Routing table file name.
    @return none.
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

//Client::Client(char *serv_name, char *serv_port) {
//    serv_port_ = atoi(serv_port);
//    strncpy(serv_name_, serv_name, strlen(serv_name));
//    struct hostent *server = my_gethostbyname(serv_name);
//    
//    // Configure server address info
//    bzero(&serv_addr_, sizeof(serv_addr_));
//    serv_addr_.sin_family = AF_INET;
//    serv_addr_.sin_port = htons(serv_port_);
//    bcopy((char*)server->h_addr, (char*)&serv_addr_.sin_addr.s_addr, server->h_length);
//    //inet_pton(AF_INET, serv_name, &serv_addr_.sin_addr);
//    
//    // Create socket to host communication
////    serv_fd_ = my_socket(AF_INET, SOCK_STREAM, 0);
//}
