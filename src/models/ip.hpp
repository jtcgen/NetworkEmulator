//
//  ip.h
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef IP_HPP
#define IP_HPP

#include <string.h>
#include <string>
#include "ether.hpp"

/* ARP packet types */
#define ARP_REQUEST 0
#define ARP_RESPONSE 1

/* IP protocol types */
#define PROT_TYPE_UDP 0
#define PROT_TYPE_TCP 1
#define PROT_TYPE_OSPF 2

//typedef unsigned long IPAddr;
typedef std::string IPAddr;

/* Structure to represent an interface */

typedef struct iface {
    std::string ifacename;
    IPAddr ipaddr;
    IPAddr mask;
    MacAddr macaddr;
    std::string lanname;
} Iface;

/* mapping between interface name and socket id representing link */
typedef struct itface2link {
    char ifacename[32];
    int sockfd;
} ITF2LINK;

/* Structure for a routing table entry */

typedef struct rtable {
    IPAddr dest_subnet;
    IPAddr next_hop;
    IPAddr mask;
    char ifacename[32];
} Rtable;


///* Structure for an ARP cache entry */
//
//typedef struct arpcache {
//    IPAddr ip_addr;
//    MacAddr mac_addr;
//} Arpc;

/*--------------------------------------------------------------------*/

/* The Structure of the IP datagram and ARP packets go here */

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* Structure for ARP packets */

/*Map of ARP cache, used to maintain current cache*/
typedef std::map<IPAddr, MacAddr> ArpMap

/*ARP packet format*/
typedef struct arp_pkt
{
    short op; /* op =0 : ARP request; op = 1 : ARP response */
    IPAddr src_ip;
    MacAddr src_mac;
    IPAddr dst_ip;
    MacAddr dst_mac;
} ArpPkt;

/*IP packet format*/
typedef struct ip_pkt
{
    IPAddr  dst_ip;
    IPAddr  src_ip;
    short   protocol;
    unsigned long    sequenceno;
    short   length;
    std::string data;
//    char    data[BUFSIZ];
} IpPkt;

/*queue for ip packet that has not yet sent out*/
typedef struct p_queue
{
    IPAddr next_hop_ipaddr;
    IPAddr dst_ipaddr;
    char *pending_pkt;
    struct p_queue *next;
    
} PendingQueue;

/*queue to remember the packets we have received*/
typedef struct packet_queue
{
    char *packet;
    int  length;
    short counter;
    struct packet_queue *next;
} OldPackets;

/*-------------------------------------------------------------------- */


//#define MAXHOSTS 32
//#define MAXINTER 32

typedef struct host
{
    std::string name;
    IPAddr addr;
    short port;
} Host;

typedef struct lan_rout {
    short router_attached;
    short counter;
} LanRoute;

// Bridges and stations will be added in sequential order...NOT THREAD SAFE
class IP {
public:
    /**
     *  Checks if host is already in existence.
     *
     *  @param host     Name of Host
     */
    static bool host_exists(std::string host);
    
    /**
     *  Adds host to list of existing hosts.
     *
     *  @param host     New bridge/station.
     *  @return         Success status.
     */
    static bool add_host(std::string host);
    
    
    static const int MAX_HOSTS = 32;
    static const int MAX_INTER = 32;
    
    static Host host[MAX_HOSTS];
    static int host_cnt;
    
    static Iface iface_list[MAX_INTER];
    /* if there is router on this lan, 1; else 0 */
    static LAN_ROUT lan_router[MAX_INTER];
    static ITF2LINK link_socket[MAX_INTER];
    static int intr_cnt; /* counter for interface */
    
    static Rtable rt_table[MAX_HOSTS*MAX_INTER];
    static int rt_cnt;
    
    static PENDING_QUEUE *pending_queue;
    static ARP_LIST *arp_cache;
    
    static int ROUTER;
};

#endif /* IP_HPP */
