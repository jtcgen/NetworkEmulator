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
#include <list>
#include <chrono>
#include <map>

/* ETHER PKT */
#define PEER_CLOSED 2
#define TYPE_IP_PKT 1
#define TYPE_ARP_PKT 0

typedef std::string MacAddr;

/* ARP packet types */
#define ARP_REQUEST 0
#define ARP_RESPONSE 1

/* IP protocol types */
#define PROT_TYPE_UDP 0
#define PROT_TYPE_TCP 1
#define PROT_TYPE_OSPF 2

typedef std::string IPAddr;
typedef int Port;

/* Structure to represent an interface */

typedef struct iface {
    std::string iface_name;
    IPAddr ip_addr;                 // Interface IP address
    IPAddr net_mask;                // Network mask
    MacAddr mac_addr;               // MAC address
    std::string lan_name;
} Iface;

/* mapping between interface name and socket id representing link */
typedef struct itface2link {
    char ifacename[32];
    int sockfd;
} ITF2LINK;

/* Structure for a routing table entry */

typedef struct rtable {
    IPAddr dest_addr;               // Destination network prefix address
    IPAddr next_hop;                // Next-hop router address
    IPAddr net_mask;                // Network mask
    std::string iface_name;
} Rtable;

/*--------------------------------------------------------------------*/

/* The Structure of the IP datagram and ARP packets go here */

/*--------------------------------------------------------------------*/

/*--------------------------------------------------------------------*/
/* Structure for ARP packets */

typedef std::chrono::milliseconds milliseconds;

typedef struct CacheContent {
    CacheContent(MacAddr m, milliseconds t) : mac(m), time(t) { }
    MacAddr mac;
    milliseconds time;
} CacheContent;

typedef std::map<IPAddr, CacheContent> ArpCacheMap;
typedef std::map<IPAddr, CacheContent>::iterator ArpCacheMapItr;

class ArpCache {
public:
    
    /*                                                                 */
    /*  ARP cache, used to maintain current active IP/MAC connections. */
    /*                                                                 */
    
    ArpCache() : MAX_TIME(10000) { }
    
    /*
     *  Searches ARP cache for IP address.
     *
     *  @return result              IP address existence
     */
    bool has_mapping(IPAddr ip);
    
    /*
     *  Adds IP/MAC pair to ARP cache.
     *
     *  @param ip                   IP address
     *  @param mac                  MAC address
     */
    void add_mapping(IPAddr ip, MacAddr mac);
    
    /**
     *  Checks for inactive station in ARP cache.
     */
    void monitor_cache();
    
    //      Accessors       //

    /**
     *  Retrieves the corresponding MAC address.
     *
     *  @param ip                   IP Address
     *  @return                     Corresponding MAC address
     */
    MacAddr get_mac_for(IPAddr ip);
    
    /*
     *  Returns iterator to first element of ArpCache.
     *
     *  @return result              Iterator to first element
     */
    ArpCacheMapItr begin();
    
    /*
     *  Returns iterator to the last element of ArpCache.
     *
     *  @return result              Iterator to last element
     */
    ArpCacheMapItr end();
    
private:
    const milliseconds MAX_TIME;
    ArpCacheMap cache_;
    
};

/*ARP packet format*/
typedef struct arp_pkt
{
    static struct arp_pkt create_req_pkt(Iface i, IPAddr dst_ip);
    static struct arp_pkt create_resp_pkt(struct arp_pkt pkt, IPAddr src_ip, MacAddr src_mac);
    
    short op; /* op =0 : ARP request; op = 1 : ARP response */
    IPAddr src_ip;
    MacAddr src_mac;
    IPAddr dst_ip;
    MacAddr dst_mac;
} ArpPkt;

/*IP packet format*/
typedef struct ip_pkt
{
    static struct ip_pkt create_pkt(IPAddr src_ip, IPAddr dst_ip, std::string data);
    
    IPAddr  dst_ip;
    IPAddr  src_ip;
//    short   protocol;
//    unsigned long    sequenceno;
    std::string data;
} IpPkt;

/* structure of an ethernet pkt */
typedef struct __etherpkt
{
    __etherpkt() { }
    __etherpkt(MacAddr mac_src) { this->mac_src = mac_src; }
    
    static void print(struct __etherpkt &pkt);
    
    /* destination address in net order */
    MacAddr mac_dst;
    
    /* source address in net order */
    MacAddr mac_src;
    
    /************************************/
    /* payload type in host order       */
    /* type = 0 : ARP frame             */
    /* type = 1 : IP  frame             */
    /************************************/
    short  type;
    
    /* size of the data in host order */
    short   size;
    
    ArpPkt arp_pkt;
    IpPkt ip_pkt;
    
} EtherPkt;


/*queue for ip packet that has not yet sent out*/
typedef struct pending_pkt {
    IPAddr next_hop_ipaddr;
    IPAddr dst_ipaddr;
    IPAddr src_ipaddr;
    std::string iface_name;
    std::string data;
} PendingPkt;

/*queue to remember the packets we have received*/
typedef std::list<PendingPkt> PendingQueue;
typedef std::list<PendingPkt>::iterator PendingQueueItr;



/*-------------------------------------------------------------------- */


//#define MAXHOSTS 32
//#define MAXINTER 32

typedef struct host
{
    std::string name;
    IPAddr addr;
//    Port port;
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
    static LanRoute lan_router[MAX_INTER];
    static ITF2LINK link_socket[MAX_INTER];
    static int intr_cnt; /* counter for interface */
    
    static Rtable rt_table[MAX_HOSTS*MAX_INTER];
    static int rt_cnt;
    
    static PendingQueue *pending_queue;
//    static ArpList *arp_cache;
    
    static int ROUTER;
};


#endif /* IP_HPP */
