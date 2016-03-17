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
int IP::hostcnt = 0;
int IP::intr_cnt; /* counter for interface */
int IP::rt_cnt = 0;
int IP::ROUTER = 0;

Iface IP::iface_list[MAX_INTER];
/* if there is router on this lan, 1; else 0 */
LAN_ROUT IP::lan_router[MAX_INTER];
ITF2LINK IP::link_socket[MAX_INTER];
Rtable IP::rt_table[MAX_HOSTS * MAX_INTER];

PENDING_QUEUE* IP::pending_queue;
ARP_LIST* IP::arp_cache;