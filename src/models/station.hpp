//
//  Station.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Station_hpp
#define Station_hpp

#include <stdio.h>
#include <stdlib.h>         // sleep
#include <string.h>          // strtok
#include <string>
#include <vector>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>          // fcntl blocking

#include "clientdata.hpp"
#include "w_socket.hpp"
#include "utility.hpp"
#include "ip.hpp"

typedef struct next_hop_data {
    IPAddr ip_addr;
    std::string iface;
} NextHop;

typedef std::pair<Iface*, SocketData*> IfaceSocketPair;
typedef std::vector< IfaceSocketPair > IfaceSocketVector;

class Station {
public:
    Station(const char *iface,
           const char *rtable,
           const char *hname,
           bool is_router);
    ~Station();
    void start();
    
    
protected:
    /*  CONFIGURATION FUNC  */
    
    /**
     *  Reads symbolic link and loads Lan IP address
     *  and lan name.
     *
     *  @param hname        Symbolic link host name.
     */
    void load_lans();
    
    /**
     *  Reads and stores interface data from iface filename
     *  and stores read.
     *
     *  @param iface        Interface file name
     */
    void load_interfaces(const char *iface);
    
    /**
     *  Reads and stores Host name and IP address
     *
     *  @param host        Host file name
     */
    void load_hosts(const char *host);
    
    /**
     *  Reads and stores routes stored in Routing Table file.
     *
     *  @param rtable       Routing table file name.
     */
    void load_routes(const char *rtable);
    
    /**
     *  Binds socket to connected Bridge address and server.
     *
     *  @param bridge       Bridge structure connected to Station
     */
    void socket_to_bridge(SocketData *&bridge);
    
    /**
     *  Packages and sends Ethernet Packet.
     *
     *  @param ip           
     */
    void send_ether_pkt(IPAddr src_ip, IPAddr dst_ip, std::string data);
    
    void send_ether_pkt(IPAddr dst_ip, std::string data);
    
    /**
     *  Sends user message to destination.
     *
     *  @param dst          Intended destination
     *  @param msg          Message content
     */
    void send_cmd(std::string dst, std::string msg);
    
    /**
     *  Scans incoming message from stdin and executes legal
     *  commands.
     *
     *  @param cmd          User command
     *  @param quit         Reference to quit program
     */
    void menu_commands(char *cmd, bool &quit);
    
    /**
     *  Show the ARP cache table information.
     */
    void print_arp();
    
    /**
     *  Show the pending queue.
     */
    void print_pq();
    
    /**
     *  Show the IP/name mapping table.
     */
    void print_host();
    
    /**
     *  Show interface information.
     */
    void print_iface();
    
    /**
     *  Show the contents of the routing table.
     */
    void print_rtable();

    /**
     *  Outputs the contents of the Ethernet Packet.
     *
     *  @param pkt          Ethernet Packet
     */
    void print_ether_pkt(const EtherPkt &pkt);
    
    /**
     *  Parses the IpAddr string and casts it to a long.
     *
     *  @param addr         IP Address
     *  @param arr          Container for IP address elements
     */
    void parse_address(IPAddr addr, int *arr);
    
    /**
     *  Searches the Routing table for the next hop.
     *
     *  @param host_dst     Station destination IP Address
     *  @return             IP Address of next hop
     */
    NextHop get_next_hop(IPAddr host_dst);

    /**
     *  Retrieves the interface structure for the
     *  corresponding interface name.
     *
     *  @param name         Interface Name
     *  @return             Corresponding Interface struct
     */
    IfaceSocketPair get_iface(std::string name);
    
    /**
     *  Retrieves the interface for the corresponding
     *  IP address.
     *
     *  @param ip           IP address
     *  @return             Corresponding Interface Name
     */
    std::string get_iface_for_ip(IPAddr ip);
    
    /**
     *  Reads symbolic link. Loads Bridge data into
     *  Socket/Server structure and returns.
     *
     *  @param iface      Interface data
     *  @return           Socket Data structure
     */
    SocketData* get_iface_socket(Iface* iface);
    
    /**
     *  Checks if incoming IP packet is intended for self.
     *
     *  @param ip       IP Address of intended recipient.
     *  @return         True or False.
     */
    bool is_intended_ip_destination(IPAddr ip);
    

    
    ArpCache arp_;                      // Arp Cache
    PendingQueue pq_;                   // Queue of old packets
    
    IfaceSocketVector ifaces_;          // Interface and Socket connection data
    std::vector<Rtable*> routes_;       // Routing table information
    std::vector<Host*> hosts_;          // All active stations and routers
    
    bool is_router_;
    Log debug;
};

#endif /* station.cpp */
