//
//  Station.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/11/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include "Station.hpp"

Station::Station(char *iface, char *rtable, char *hname) : Client(iface, rtable, hname){
    


}

Station::~Station() {
    
    // Clean up interfaces
    for (std::vector<Iface*>::iterator itr = ifaces_.begin(); itr != ifaces_.end(); ++itr)
        delete *itr;
    
    // Clean up lans
    for (int i = 0; i < lans_.size(); ++i) {
        delete lans_[i];
        delete sd_[i];
    }
    for (std::vector<Host*>::iterator itr = hosts_.begin(); itr != hosts_.end(); ++itr)
        delete *itr;
    
    // Clean up routes
    for (std::vector<Rtable*>::iterator itr = routes_.begin(); itr != routes_.end(); ++itr)
        delete *itr;
    
}

///**
//    Reads symbolic link and extracts connected bridge's
//    IP address and port number.
// 
//    @param link Name of symbolic link.
//    @return
// */
//void load_bridge_data(std::string link, AddrData *bridge) {
//    
//}


// TODO: Question, are stations that connect to multiple lans supposed to have individual station?
void Station::start() {
    int max_fd;
    fd_set read_set;
    struct sockaddr_in sock_addr;
    socklen_t sock_size;
    int sock_port;
    
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);
//    FD_SET(
}