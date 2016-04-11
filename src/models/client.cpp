//
//  Client.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <sstream>
#include <iomanip>

#include "client.hpp"

/**
 *  Constructor
 *
 *  @param iface Interface file name.
 *  @param rtable Routing table file name.
 *  @param hname Lan host file name.
 */
Client::Client(const char *iface, const char *rtable, const char *host, bool is_router) : is_router_(is_router), debug("Client", true) {
    load_interfaces(iface);
    load_hosts(host);
    load_routes(rtable);
}

Client::~Client() {
    debug.get_oss() << "Calling Client Destructor...";
    debug.print();
    
    // Clean up interfaces
    for (auto i : ifaces_) {
        delete i.first; delete i.second;
    }
    
    // Clean up  hosts
    for (auto h : hosts_) {
        delete h;
    }
    
    // Clean up routes
    for (auto r : routes_) {
        delete r;
    }
}

/**
 *  Reads and stores Host name and IP address
 *
 *  @param host        Host file name
 */
void Client::load_hosts(const char *host) {
    std::string line;
    std::ostringstream file;
    file << "./hosts";
    std::ifstream in(file.str());
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << host << ".";
        my_error(out.str());
    }
    
    // Load store hosts
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        Host *nhost = new Host;
        iss >> nhost->name >> nhost->addr;
        hosts_.push_back(nhost);
        
        debug.get_oss() << "Loading Host: " << nhost->name
                        << " Address: " << nhost->addr;
        debug.print();
    }
}

/**
 *  Reads symbolic link. Loads Bridge data into
 *  Socket/Server structure and returns.
 *
 *  @param iface      Interface data
 *  @return           Socket Data structure
 */
SocketData* Client::get_iface_socket(Iface* iface) {
    std::ifstream in;                       // Input stream to read contents of symlink
    size_t n;                               // Size of read symbolic link
    const size_t BUF_SIZE = 100;            // Size of buffer
    char buf[BUF_SIZE];                     // Buffer for symbolic link

    
    SocketData *sd = new SocketData;

    debug.get_oss() << "Reading Symbolic Link: " << iface->lan_name << " -> " << buf;
    debug.print();
    
    // Read symoblic link
    n = my_readlink((const char*) iface->lan_name.c_str(), buf, BUF_SIZE);
    
    // Process buf
    memset(buf + n, '\0', BUF_SIZE - n);
    
    // Open corresponding file
    in.open(buf);
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << iface->lan_name << ".";
        my_error(out.str());
    }
    
    // Load data into host structure
    in >> sd->ip_addr >> sd->port;
    
    debug.get_oss() << "Loading lan: " << iface->lan_name << " IP Address: "
                    << sd->ip_addr << " on Port: " << sd->port;
    debug.print();
    
    // Map socket to bridge
    socket_to_bridge(sd);
    
    in.close();
    
    return sd;
}

/**
 *  Reads and stores interface data from iface filename
 *  and stores read.
 *
 *  @param iface    Interface file name
 */
void Client::load_interfaces(const char *iface) {
    std::ostringstream file;            // Modified interface file
    std::string line;                   // Buffer contents of iface file
    
    file << "ifaces/" << iface;
    
    std::ifstream in(file.str());
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << iface << ".";
        my_error(out.str());
    }
    
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        
        Iface *iface = new Iface;
        
        iss >> iface->iface_name
           >> iface->ip_addr
           >> iface->net_mask
           >> iface->mac_addr
           >> iface->lan_name;
        
        SocketData* sd = get_iface_socket(iface);
        ifaces_.push_back(std::pair<Iface*, SocketData*>(iface, sd));
        
        debug.get_oss() << "Loading Interface: " << iface->iface_name
                        << " Network Mask: " << iface->net_mask
                        << " MAC Address: " << iface->mac_addr
                        << " Lan: " << iface->lan_name;
        debug.print();
    }
    
    in.close();
}

/**
 *  Reads and stores routes stored in Routing Table file.
 *
 *  @param rtable       Routing table file name.
 */
void Client::load_routes(const char *rtable) {
    debug.get_oss() << "Loading Routes";
    debug.print();
    
    std::ifstream in;
    std::ostringstream file;
    std::string line;
    
    file << "rtables/" << rtable;
    
    in.open(file.str());
    
    if (!in) {
        std::ostringstream out;
        out << "Failed to open " << rtable << ".";
        my_error(out.str());
    }
    
    while(std::getline(in, line)) {
        std::istringstream iss(line);
        
        Rtable *route = new Rtable;
        
        iss >> route->dest_addr
           >> route->next_hop
           >> route->net_mask
           >> route->iface_name;
        
        routes_.push_back(route);
        
        debug.get_oss() << "Loading Routing Interface: " << route->iface_name
                        << " Destination Address: " << route->dest_addr
                        << " Network Mask: " << route->net_mask
                        << " Next Hop: " << route->next_hop;
        debug.print();
    }
    
    in.close();
}


void Client::send_ether_pkt(IPAddr dst_ip, std::string data) {
    // Get first hop in path to destination
    NextHop next_hop = get_next_hop(dst_ip);
    IfaceSocketPair iface = get_iface(next_hop.iface);
    EtherPkt ether_pkt(iface.first->mac_addr);
    
    // Destination is in same LAN
    if (next_hop.ip_addr == "0.0.0.0")
        next_hop.ip_addr = dst_ip;
    
    // Check if dst MAC address is in ARP Cache
    debug.get_oss() << "Checking ARP cache for IP: " << next_hop.ip_addr;
    debug.print();
    if (arp_.has_mapping(next_hop.ip_addr)) {
        debug.get_oss() << "ARP has mapping - Next Hop IP Address: " << next_hop.ip_addr;
        debug.print();
        
        // Wrap message in IP Packet
        ether_pkt.mac_dst = arp_.get_mac_for(next_hop.ip_addr);
        ether_pkt.type = 1;
        ether_pkt.ip_pkt = IpPkt::create_pkt(iface.first->ip_addr, dst_ip, data);
    } else {
        debug.get_oss() << "ARP does not have mapping...pushing packet to Pending Queue...";
        debug.print();
        
        // Add message to pending queue
        PendingPkt pp = { next_hop.ip_addr, dst_ip, iface.first->ip_addr, next_hop.iface, data };
        pq_.push_back(pp);
        
        // Wrap ARP Request
        ether_pkt.arp_pkt = ArpPkt::create_req_pkt(*iface.first, next_hop.ip_addr);
        ether_pkt.type = 0;
    }
    
    debug.get_oss() << "Sending Ether Packet";
    debug.print();
    print_ether_pkt(ether_pkt);
    
    // Send Ether Packet
    if (my_write(iface.second->fd, &ether_pkt, sizeof(EtherPkt)) < 0)
        std::cout << "Could not send Ethernet Packet\n";
}

/**
 *  Sends user message to destination.
 *
 *  @param dst          Intended destination
 *  @param msg          Message content
 */
void Client::send_cmd(std::string dst, std::string msg) {
    bool valid_dst = false;
    
    // Check if dst host is valid
    for (auto h : hosts_) {
        if (dst.compare(h->name) == 0) {
            send_ether_pkt(h->addr, msg);
            valid_dst = true;
            break;
        }
    }
    
    if (!valid_dst) {
        std::cout << "Invalid Destination." << std::endl;
    }
}

/**
 *  Parses the IpAddr string and casts it to a long.
 *
 *  @param addr         IP Address
 *  @param arr          Container for IP address elements
 */
void Client::parse_address(IPAddr addr, int *arr) {
    size_t pos = 0;
    std::string delim = ".";
    std::string tok;
    
    unsigned int i = 0;
    while ((pos = addr.find(".")) != std::string::npos) {
        tok = addr.substr(0, pos);
        arr[i++] = std::stoi(tok);
        addr.erase(0, pos + delim.length());
    }
    
    // Parse last two bytes of IP Address
    arr[i] = std::stoi(addr);
}

/**
 *  Searches the Routing table for the next hop.
 *
 *  @param host_dst     Station destination IP Address
 *  @return             IP Address of next hop
 */
NextHop Client::get_next_hop(IPAddr host_dst) {
    debug.get_oss() << "Get Next Hop";  debug.print();
    
    NextHop next_hop;
    int dst[4];
    parse_address(host_dst, dst);
    
    for (auto r : routes_) {
        std::string delim = ".";
        std::string smask(r->net_mask);
        std::string snet_prefix(r->dest_addr);
        
        // Parse string and cast to integer format
        int mask[4], net_prefix[4];
        parse_address(smask, mask);
        parse_address(snet_prefix, net_prefix);
        
        bool match = true;
        
        // Next hop is selected if bitwise &, between mask and dst IP address matches dst
        for (unsigned int i = 0; i < 4; ++i) {
//            debug.get_oss() << dst[i] << " & " << mask[i] << " = "
//                            << (dst[i] & mask[i]) << " : Net_Prefix - " << net_prefix[i];
//            debug.print();
            
            if ((dst[i] & mask[i]) != net_prefix[i]) {
                match = false;
                break;
            }
        }
        
        if (match) {
            next_hop.ip_addr = r->next_hop;
            next_hop.iface = r->iface_name;
            break;
        } else {
            debug.get_oss() << "Invalid next hop - " << r->dest_addr << " "
                            << r->next_hop << " " << r->net_mask
                            << " " << r->iface_name;                debug.print();
        }
    }
    
    debug.get_oss() << "Found next hop for " << host_dst
                    << " through Interface " << next_hop.iface
                    << " IP address " << next_hop.ip_addr;          debug.print();
    
    return next_hop;
}

/**
 *  Retrieves the interface structure for the
 *  corresponding interface name.
 *
 *  @param name         Interface Name
 *  @return             Corresponding Interface struct
 */
IfaceSocketPair Client::get_iface(std::string name) {
    IfaceSocketPair result;
    for (auto i : ifaces_) {
        if (name.compare(i.first->iface_name) == 0) {
            result = i;
            break;
        }
    }
    return result;
}

/**
 *  Retrieves the interface for the corresponding
 *  IP address.
 *
 *  @param ip           IP address
 *  @return             Corresponding Interface Name
 */
std::string Client::get_iface_for_ip(IPAddr ip) {
    std::string host;
    for (auto h : hosts_) {
        if (ip.compare(h->addr) == 0) {
            host = h->name;
            break;
        }
    }
    return host;
}


/**
 *  Scans incoming message from stdin and executes legal
 *  commands.
 *
 *  @param quit         Reference to quit program
 */
void Client::menu_commands(char *cmd, bool &quit) {
    // Eliminate new line
    std::string buff(cmd);
    std::stringstream ss(buff);
    std::string s_cmd;
    std::getline(ss, s_cmd, '\n');
    
    if (strcmp(s_cmd.c_str(), "debug") == 0)  {
        debug.set_on(!debug.get_on());
    } else if (strcmp(s_cmd.c_str(), "off") == 0) {
        quit = true;
    } else {
        // Parse message
        std::string buf;
        std::stringstream ss2(s_cmd);
        ss2 >> buf;
        
        if (strcmp(buf.c_str(), "send") == 0) {
            std::string dst, msg;
            ss2 >> dst;
            std::getline(ss2, msg);
            send_cmd(dst, msg);
        } else if (buf.compare(std::string("show")) == 0) {
            ss2 >> buf;
            if (buf.compare(std::string("arp")) == 0) {
                print_arp();
            } else if (buf.compare(std::string("pq")) == 0) {
                print_pq();
            } else if (buf.compare(std::string("host")) == 0) {
                print_host();
            } else if (buf.compare(std::string("iface")) == 0) {
                print_iface();
            } else if (buf.compare(std::string("rtable")) == 0) {
                print_rtable();
            } else {
                std::cout << "Invalid command.\n";
            }
        } else {
            std::cout << "Invalid command.\n";
        }
    }
}

/**
 *  Outputs the contents of the Ethernet Packet.
 *
 *  @param pkt          Ethernet Packet
 */
void Client::print_ether_pkt(const EtherPkt &pkt) {
    debug.get_oss() << "Ether Pkt - MAC SRC: " << pkt.mac_src << " MAC DST: " << pkt.mac_dst;
    debug.print();
    if (pkt.type == 0) {
        debug.get_oss() << "ARP - OP: " << pkt.arp_pkt.op << " IP SRC: " << pkt.arp_pkt.src_ip
                        << " MAC SRC: " << pkt.arp_pkt.src_mac << "\n\t\tIP DST: " << pkt.arp_pkt.dst_ip
                        << " MAC DST: " << pkt.arp_pkt.dst_mac;
    } else {
        debug.get_oss() << "IP - IP SRC: " << pkt.ip_pkt.src_ip << " IP DST: " << pkt.ip_pkt.dst_ip
                        << " MSG: " << pkt.ip_pkt.data;
    }
    debug.print();
}

/**
 *  Show the ARP cache table information.
 */
void Client::print_arp() {
    std::cout << "Showing ARP Cache\n";
    std::cout << std::setw(13) << "IP Address" << std::setw(15) << "MAC Address";
    std::cout << "\n--------------------------------\n";
    for (auto a : arp_) {
        std::cout << std::left << std::setw(15) << a.first << std::setw(15) << a.second.mac << std::endl;
    }
}

/**
 *  Show the pending queue.
 */
void Client::print_pq() {
    std::cout << "Showing Pending Queue" << std::endl;
    std::cout << std::setw(14) << "Next Hop" << std::setw(12) << "Iface" << std::setw(17)
              << "Destination IP" << std::setw(8) << "Data" << std::endl;
    std::cout << "-----------------------------------------------\n";
    for (auto p: pq_) {
        std::cout << std::setw(16) << p.next_hop_ipaddr << std::setw(10) << p.iface_name
                  << std::setw(17) << p.dst_ipaddr << std::setw(100) << p.data << std::endl;
    }
}

/**
 *  Show the IP/name mapping table.
 */
void Client::print_host() {
    std::cout << "Showing Hosts\n";
    std::cout << std::left << std::setw(15) << "IP Address" << std::setw(7) << "Station" << std::endl;
    std::cout << "----------------------" << std::endl;
    for (std::vector<Host*>::iterator itr = hosts_.begin(); itr != hosts_.end(); ++itr)
        std::cout << std::setw(16) << (*itr)->addr << std::setw(7) << (*itr)->name << std::endl;
}

/**
 *  Show interface information.
 */
void Client::print_iface() {
    std::cout << "Showing Interface Information\n";
    std::cout << std::setw(7) << "Station" << std::setw(14) << "IP Address"
              << std::setw(12) << "MASK" << std::setw(18) << "MAC"
              << std::setw(18) << "Interface" << std::endl;
    std::cout << "---------------------------------------------------------------------\n";
    
    for (auto i : ifaces_)
        std::cout << std::setw(7) << i.first->iface_name
                  << std::setw(16) << i.first->ip_addr << std::setw(16)
                  << i.first->net_mask << std::setw(19) << i.first->mac_addr
                  << std::setw(8) << i.first->lan_name << std::endl;
}

/**
 *  Show the contents of the routing table.
 */
void Client::print_rtable() {
    std::cout << "Showing Routing Table Information\n";
    std::cout << std::setw(14) << "Net Prefix" << std::setw(14) << "Next Hop"
              << std::setw(12) << "Mask" << std::setw(17) << "Interface" << std::endl;
    std::cout << "    -----------------------------------------------------\n";
    
    for (std::vector<Rtable*>::iterator itr = routes_.begin(); itr != routes_.end(); ++itr)
        std::cout << std::setw(15) << (*itr)->dest_addr << std::setw(15) << (*itr)->next_hop << std::setw(15)
        << (*itr)->net_mask << std::setw(8) << (*itr)->iface_name << std::endl;
}


/**
 *  Binds socket to connected Bridge address and server.
 *
 *  @param cmd          User command
 *  @param bridge       Bridge structure connected to client
 */
void Client::socket_to_bridge(SocketData *&sock_data) {
    bzero(&sock_data->sock_addr, sizeof(sock_data->sock_addr));
    sock_data->sock_addr.sin_family = AF_INET;
    sock_data->sock_addr.sin_port = htons(sock_data->port);
    
    if (inet_pton(AF_INET, sock_data->ip_addr.c_str(), (void*) &sock_data->sock_addr.sin_addr) == 0)
        my_error("pton error");
    
    sock_data->fd = WSocket::wsocket(AF_INET, SOCK_STREAM, 0);
    
    debug.get_oss() << "Binding socket " << sock_data->fd << " to " << sock_data->ip_addr
                    << " on port " << sock_data->port;
    debug.print();
}

/**
 *  Checks if incoming IP packet is intended for self.
 *
 *  @param ip       IP Address of intended recipient.
 *  @return         True or False.
 */
bool Client::is_intended_ip_destination(IPAddr ip) {
    for (auto i : ifaces_) {
        if (ip.compare(i.first->ip_addr) == 0) {
            return true;
        }
    }
    return false;
}

void Client::start() {
    int max_fd;
    fd_set read_set;
    const unsigned int MSG_SIZE = 100;
    char msg[MSG_SIZE];
    bool quit = false;
    
    FD_ZERO(&read_set);
    FD_SET(0, &read_set);

    // int *flags = new int[lans_.size()];
    
    // Connect to bridges
    for (unsigned int i = 0; i < ifaces_.size(); ++i) {
        // Set read flags
//        FD_SET(ifaces_[i].second->fd, &read_set);
        
        connect(ifaces_[i].second->fd, (struct sockaddr*)&ifaces_[i].second->sock_addr, sizeof(ifaces_[i].second->sock_addr));
        // Set non-blocking flag for bridge descriptors
        // flags[ix] = fcntl(i.second->fd, F_GETFL, 0);
        // fcntl(i.second->fd, F_SETFL, flags[i] | O_NONBLOCK);

        bool accepted = false;
        // Wait preset number of times, before station gives up
        for (unsigned int j = 0; j < 1; ++j) {
            if (read(ifaces_[i].second->fd, msg, MSG_SIZE) > 0) {
                if (strcmp(msg, "Accepted") == 0) {
                    // Reset to blocking on socket file descriptor
                    // fcntl(i.second->fd, F_SETFL, flags[idx];
                    accepted = true;
                }
            } else sleep(2);
        }
        
        if (!accepted) {
            // Erase bridge from data struct
            delete ifaces_[i].second;
            ifaces_.erase(ifaces_.begin() + i);
        }
        
        std::cout << ifaces_[i].first->iface_name << " " <<  msg << std::endl;
    }

    memset(msg, '\0', MSG_SIZE);
    
    while (!quit) {
        FD_SET(0, &read_set);
        
        // Update readset
        for (auto i : ifaces_)
            FD_SET(i.second->fd, &read_set);
        
        max_fd = ifaces_.back().second->fd;
        
        arp_.monitor_cache();
        
        my_select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (FD_ISSET(0, &read_set)) {
            if (read(STDIN_FILENO, msg, MSG_SIZE) > 0) {
                menu_commands(msg, quit);
            }
            memset(msg, '\0', MSG_SIZE);
        } else {
            EtherPkt recv_ether_pkt;
            for (auto i : ifaces_) {
                if (FD_ISSET(i.second->fd, &read_set)) {
                    if (read(i.second->fd, &recv_ether_pkt, sizeof(recv_ether_pkt)) < 0) {
                        debug.get_oss() << "Could not read incoming Ethernet Packet.";
                        debug.print();
                    } else {
                        if (recv_ether_pkt.type == 0) {      /*  ARP PACKET  */
                            if (recv_ether_pkt.arp_pkt.op == 0) {    /*  REQUEST  */
                                debug.get_oss() << "Receiving incoming Arp Request from " << i.first->iface_name;
                                debug.print();
                                print_ether_pkt(recv_ether_pkt);
                                for (auto i : ifaces_) {
                                    if (recv_ether_pkt.arp_pkt.dst_ip.compare(i.first->ip_addr) == 0) {
                                        debug.get_oss() << "Adding IP: " << recv_ether_pkt.arp_pkt.src_ip << " MAC: "
                                                        << recv_ether_pkt.arp_pkt.src_mac << " to Arp Cache.";
                                        debug.print();
                                        
                                        arp_.add_mapping(recv_ether_pkt.arp_pkt.src_ip, recv_ether_pkt.arp_pkt.src_mac);
                                        
                                        // ARP response
                                        EtherPkt resp_ether_pkt;
                                        resp_ether_pkt.mac_dst = recv_ether_pkt.mac_src;
                                        resp_ether_pkt.mac_src = i.first->mac_addr;
                                        resp_ether_pkt.type = 0;
                                        resp_ether_pkt.arp_pkt = ArpPkt::create_resp_pkt(recv_ether_pkt.arp_pkt,
                                                                                         i.first->ip_addr,
                                                                                         i.first->mac_addr);
                                        
                                        write(i.second->fd, &resp_ether_pkt, sizeof(resp_ether_pkt));
                                        
                                        debug.get_oss() << "Sending Arp Response packet.";
                                        debug.print();
                                        print_ether_pkt(resp_ether_pkt);
                                    }
                                }
                            } else {                            /*  RESPONSE  */
                                debug.get_oss() << "Receiving incoming Arp Response from " << i.first->iface_name;
                                debug.print();
                                print_ether_pkt(recv_ether_pkt);
                                
                                if (is_intended_ip_destination(recv_ether_pkt.arp_pkt.dst_ip)) {
                                    PendingQueueItr itr = pq_.begin();
                                    while (itr != pq_.end()) {
                                        if (itr->next_hop_ipaddr.compare(recv_ether_pkt.arp_pkt.src_ip) == 0) {
                                            debug.get_oss() << "Adding IP: " << recv_ether_pkt.arp_pkt.src_ip << " MAC: "
                                                            << recv_ether_pkt.arp_pkt.src_mac << " to Arp Cache.";
                                            debug.print();
                                            
                                            arp_.add_mapping(recv_ether_pkt.arp_pkt.src_ip, recv_ether_pkt.arp_pkt.src_mac);
                                            
                                            IfaceSocketPair iface = get_iface(itr->iface_name);
                                            EtherPkt resp_ether_pkt;
                                            resp_ether_pkt.mac_dst = recv_ether_pkt.arp_pkt.src_mac;
                                            resp_ether_pkt.mac_src = iface.first->mac_addr;
                                            resp_ether_pkt.type = 1;
                                            resp_ether_pkt.ip_pkt = IpPkt::create_pkt(itr->src_ipaddr, itr->dst_ipaddr, itr->data);
                                            
                                            write(i.second->fd, &resp_ether_pkt, sizeof(resp_ether_pkt));
                                            
                                            debug.get_oss() << "Sending IP packet.";
                                            debug.print();
                                            
                                            
                                            PendingQueueItr erased = itr++;
                                            pq_.erase(erased);
                                            break;
                                        } else ++itr;
                                    }
                                }
                                
                            }
                        } else {                        /*  IP PACKET   */
                            debug.get_oss() << "Receiving incoming IP Packet from " << i.first->iface_name;
                            debug.print();
                            print_ether_pkt(recv_ether_pkt);
                            
                            if (is_intended_ip_destination(recv_ether_pkt.ip_pkt.dst_ip)) {
                                std::string sender_iface = get_iface_for_ip(recv_ether_pkt.ip_pkt.src_ip);
                                std::cout << "From " << sender_iface << ": " << recv_ether_pkt.ip_pkt.data << std::endl;
                            } else {
                                if (is_router_) {
                                    debug.get_oss() << "IP Packet not intended for self...forwarding...";
                                    debug.print();
                                    // TODO: Fix original sender
                                    // Forward packet to next hop
                                    send_ether_pkt(recv_ether_pkt.ip_pkt.dst_ip, recv_ether_pkt.ip_pkt.data);
                                }
                            }
                        }
                    }
                }
            }
            memset(&recv_ether_pkt, 0, sizeof(recv_ether_pkt));
        }
    }
}




















