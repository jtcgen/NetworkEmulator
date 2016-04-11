//
//  ip.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/17/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <stdio.h>
#include <iostream>

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

//typedef struct arp_pkt
//{
//    short op; /* op =0 : ARP request; op = 1 : ARP response */
//    IPAddr src_ip;
//    MacAddr src_mac;
//    IPAddr dst_ip;
//    MacAddr dst_mac;
//} ArpPkt;
//
///*IP packet format*/
//typedef struct ip_pkt
//{
//    IPAddr  dst_ip;
//    IPAddr  src_ip;
//    //    short   protocol;
//    //    unsigned long    sequenceno;
//    std::string data;
//}

/************************************************************************/
/*                              EtherPkt                                */
/************************************************************************/
void __etherpkt::print(struct __etherpkt &pkt) {
    std::cout << "Ether Pkt - MAC SRC: " << pkt.mac_src << " MAC DST: " << pkt.mac_dst << "\n";
    if (pkt.type == 0) {
        std::cout << "ARP - OP: " << pkt.arp_pkt.op << " IP SRC: " << pkt.arp_pkt.src_ip
        << " MAC SRC: " << pkt.arp_pkt.src_mac << " IP DST: " << pkt.arp_pkt.dst_ip
        << " MAC DST: " << pkt.arp_pkt.dst_mac << std::endl;
    } else {
        std::cout << "IP - IP SRC: " << pkt.ip_pkt.src_ip << " IP DST: " << pkt.ip_pkt.dst_ip
        << " MSG: " << pkt.ip_pkt.data << std::endl;
    }
}

/************************************************************************/
/*                               ArpPkt                                 */
/************************************************************************/
ArpPkt arp_pkt::create_req_pkt(Iface i, IPAddr dst_ip) {
    ArpPkt pkt;
    
    pkt.src_ip = i.ip_addr;
    pkt.src_mac = i.mac_addr;
    pkt.dst_ip = dst_ip;
    pkt.op = 0;
    
    return pkt;
}

ArpPkt arp_pkt::create_resp_pkt(struct arp_pkt pkt, IPAddr src_ip, MacAddr src_mac) {
    ArpPkt new_pkt;
    
    new_pkt.src_ip = src_ip;
    new_pkt.src_mac = src_mac;
    new_pkt.dst_ip = pkt.src_ip;
    new_pkt.dst_mac = pkt.src_mac;
    new_pkt.op = 1;
    
    return new_pkt;
}

/************************************************************************/
/*                               IpPkt                                  */
/************************************************************************/
struct ip_pkt ip_pkt::create_pkt(IPAddr src_ip, IPAddr dst_ip, std::string data) {
    IpPkt pkt;
    
    pkt.src_ip = src_ip;
    pkt.dst_ip = dst_ip;
    pkt.data = data;
    
    return pkt;
}


/************************************************************************/
/*                              ArpCache                                */
/************************************************************************/

/*
 *  Searches ARP cache for IP address.
 *
 */
bool ArpCache::has_mapping(IPAddr ip) {
    ArpCacheMapItr itr = cache_.find(ip);
    if (itr != cache_.end())
        return true;
    else
        return false;
}

/*
 *  Adds IP/MAC pair to ARP cache.
 *
 *  @param ip                   IP address
 *  @param mac                  MAC address
 */
void ArpCache::add_mapping(IPAddr ip, MacAddr mac) {
    milliseconds time =  std::chrono::duration_cast< milliseconds >
                        (std::chrono::system_clock::now().time_since_epoch());
    cache_.insert(std::pair<IPAddr, CacheContent>(ip, CacheContent(mac, time)));
}

/**
 *  Checks for inactive station in ARP cache.
 */
void ArpCache::monitor_cache() {
    milliseconds time = std::chrono::duration_cast< milliseconds >
                       (std::chrono::system_clock::now().time_since_epoch());
    
    ArpCacheMapItr itr = cache_.begin();
    while (itr != cache_.end()) {
        if (time.count() - itr->second.time.count() >= MAX_TIME.count()) {
            ArpCacheMapItr erased = itr++;
            cache_.erase(erased);
        } else ++itr;
    }
}

/**
 *  Retrieves the corresponding MAC address.
 *
 *  @param ip                   IP Address
 *  @return                     Corresponding MAC address
 */
MacAddr ArpCache::get_mac_for(IPAddr ip) {
    return cache_.find(ip)->second.mac;
}

/*
 *  Returns iterator to first element of ArpCache.
 *
 *  @return result              Iterator to first element
 */
ArpCacheMapItr ArpCache::begin() {
    return cache_.begin();
}

/*
 *  Returns iterator to the last element of ArpCache.
 *
 *  @return result              Iterator to last element
 */
ArpCacheMapItr ArpCache::end() {
    return cache_.end();
}





