//
//  Station.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/11/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include "station.hpp"

Station::Station(const char *iface, const char *rtable, const char *hname, bool is_router) : Client(iface, rtable, hname, is_router){

}

Station::~Station() {
    debug.get_oss() << "Calling Station Destructor...";
    debug.print();
//    if (debug.get_on())
//        debug.print("Calling Station Destructor...");
}


// TODO: Question, are stations that connect to multiple lans supposed to have individual station?
void Station::start() {
//    int max_fd;
//    fd_set read_set;
//    struct sockaddr_in sock_addr;
//    socklen_t sock_size;
//    int sock_port;
//    const unsigned int MSG_SIZE = 100;
//    char msg[MSG_SIZE];
//    bool quit = false;
//    
//    FD_ZERO(&read_set);
//    FD_SET(0, &read_set);
//
//    for (std::vector<BridgeData*>::iterator itr = lans_.begin(); itr != lans_.end(); ++itr)
//        FD_SET((*itr)->fd_, &read_set);
//    
//    int *flags = new int[lans_.size()];
//    
//    // Connect to bridges
//    for (unsigned int i = 0; i < lans_.size(); ++i) {
//        std::cout << "Station before connect\n";
//        connect(lans_[i]->fd_, (struct sockaddr*)&lans_[i]->sock_addr_, sizeof(lans_[i]->sock_addr_));
//        std::cout << "Station after connect\n";
//        // Set non-blocking flag for bridge descriptors
//        flags[i] = fcntl(lans_[i]->fd_, F_GETFL, 0);
//        fcntl(lans_[i]->fd_, F_SETFL, flags[i] | O_NONBLOCK);
//        
//        bool accepted = false;
//        // Wait for preset number of times
//        for (unsigned int j = 0; j < 1; ++j) {
//            std::cout << "Trying to connect\n";
//            if (my_read(lans_[i]->fd_, msg, MSG_SIZE) > 0) {
//                if (strcmp(msg, "Accepted") == 0) {
//                    std::cout << "ACcepted\n";
//                    // Reset to blocking on socket file descriptor
//                    fcntl(lans_[i]->fd_, F_SETFL, flags[i]);
//                    // Add socket fd to read set for multiplexing
//                    FD_SET(lans_[i]->fd_, &read_set);
//                    accepted = true;
//                }
//            } else {
//                sleep(2);
//            }
//        }
//        
//        if (!accepted) {
//            std::cout << "Erasing\n";
//            // Erase bridge from data struct
//            delete lans_[i];
//            lans_.erase(lans_.begin() + i);
//        }
//    }
//    
//    std::cout << "before connect.\n";
//    memset(msg, '\0', MSG_SIZE);
//    
//    while (!quit) {
//        std::cout << lans_.size() << std::endl;
//        max_fd = lans_.back()->fd_;
//        my_select(max_fd + 1, &read_set, NULL, NULL, NULL);
//        if (FD_ISSET(0, &read_set)) {
//            // Grab message from stdin and send
//            std::cin.getline(msg, MSG_SIZE);
//            
//            EtherPkt pkt;
//            pkt.src = "mac src";
//            pkt.dst = "mac dst";
//            ArpPkt apkt;
//            apkt.op = 0;
//            apkt.src_ip = "arp src ip";
//            apkt.dst_ip = "arp dst ip";
//            pkt.arp_ = apkt;
//            
//            //
//            my_write(lans_.front()->fd_, &pkt, sizeof(EtherPkt));
//            
//        } else {
//            for (unsigned int i = 0; i < lans_.size(); ++i) {
//                if (FD_ISSET(lans_[i]->fd_, &read_set)) {
//                    
//                }
//            }
//        }
//        
//    }
}