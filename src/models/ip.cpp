//
//  ip.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/17/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <stdio.h>

#include "ip.hpp"

Host IP::host[MAX_HOSTS];
int IP::host_cnt = 0;
int IP::intr_cnt; /* counter for interface */
int IP::rt_cnt = 0;
int IP::ROUTER = 0;

Iface IP::iface_list[MAX_INTER];
/* if there is router on this lan, 1; else 0 */
LanRoute IP::lan_router[MAX_INTER];
ITF2LINK IP::link_socket[MAX_INTER];
Rtable IP::rt_table[MAX_HOSTS * MAX_INTER];

PendingQueue* IP::pending_queue;
//ARP_LIST* IP::arp_cache;

/**
 *  Checks if host is already in existence.
 *
 *  @param host     Name of Host
 */
bool IP::host_exists(std::string h) {
    bool result = false;
    
    for (int i = 0; i < host_cnt; ++i) {
        if (strcmp(h.c_str(), host[i].name.c_str()) == 0) {
            result = true;
            break;
        }
    }
    return result;
}

/**
 *  Adds host to list of existing hosts.
 *
 *  @param host     New bridge/station.
 *  @return         Status if successfully added
 */
bool IP::add_host(std::string hname) {
    bool result = false;
    
    if (host_cnt < MAX_HOSTS) {
        host[host_cnt++].name = hname;
        result = true;
    }
    
    return result;
}
