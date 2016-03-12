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
    for (std::vector<Host*>::iterator itr = lans_.begin(); itr != lans_.end(); ++itr)
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



void Station::start() {
    
}