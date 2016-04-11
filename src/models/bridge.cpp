//
//  Bridge.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <netinet/in.h>

#include "bridge.hpp"


/**
    Constructor
 
    @param lan_name Name of the lan
    @param num_ports Max number of ports that bridge can handle
    @param debug_on Flag to set debug settings
*/
Bridge::Bridge(const char *lan_name, int num_ports) :
    ARP_TIME(10000),
    NUM_PORTS_(num_ports),
    curr_ports_(0),
    lan_name_(lan_name),
    debug("Bridge", true) {
    
    // Setup Bridge information
    setup_server_info();
    setup_socket();
    create_symlink();
}

/**
    Destructor
 
    @return none
*/
Bridge::~Bridge() {
}


/**
 *  Creates the symbolic link to its address and port number
 *  so that others (stations/routers) can connect to it.
 */
void Bridge::create_symlink() {
    std::ostringstream content;      // File content
    std::string ip_addr;                // Bridge IP address
    
    in_addr *addr = (in_addr*) info_->h_addr;
    ip_addr = inet_ntoa(*addr);
    content << ip_addr << " " << port_;
    
    // Create File
    std::ofstream out;                  // Write to file
    std::ostringstream fname;           // Format file name
    
    // Format file name
    fname << "./tmp/" << lan_name_;
    
    out.open(fname.str().c_str());
    
    if (!out) {
        debug.get_oss() << "Failed to open " << fname.str().c_str() << ".";
        debug.print();
        my_error(debug.get_oss().str());
        my_error(debug.get_oss().str());
    }
    
    // Write to file
    out << content.str();
    
    // Create symlink to file
    my_symlink(fname.str().c_str(), lan_name_.c_str());


    debug.get_oss() << "Created symlink " << lan_name_ << " to file "
                    << fname.str() << std::endl << "\t- Contents: " << content.str();
    debug.print();
}

/**
 *  Establishes bridge address information.
 */
void Bridge::setup_server_info() {
    // Configure server information
    size_t serv_size = sizeof(addr_);
    bzero(&addr_, serv_size);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons((short)0);      // Bind to random available port
    
    char temp_host[100];
    my_gethostname(temp_host, sizeof(temp_host));
    info_ = gethostbyname(temp_host);

    debug.get_oss() << "Configured server on: " << info_->h_name;
    debug.print();
}

/**
 *  Establishes socket information for incoming and outgoing
 *  information.
 */
void Bridge::setup_socket() {
    listen_fd_ = WSocket::wsocket(AF_INET, SOCK_STREAM, 0);
    WSocket::wbind(listen_fd_, (struct sockaddr*) &addr_, sizeof(addr_));
    WSocket::wlisten(listen_fd_, NUM_PORTS_);
    
    // Get Port number
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    WSocket::wgetsockname(listen_fd_, (struct sockaddr*)&sin, &len);
    port_ = (unsigned short)ntohs(sin.sin_port);

    debug.get_oss() << "Created socket: " << listen_fd_ << " on port: " << port_;
    debug.print();
}

/**
    Client disconnected, removes infor from client vector.
 
    @param cli ClientData object.
    @param index Location in vector.
    @param all_set
    @return none
 */
void Bridge::remove_client(ClientData cli, int index, fd_set &all_set) {
    my_close(cli.fd_);
    FD_CLR(cli.fd_, &all_set);
    clients_.erase(clients_.begin()+index);
    --curr_ports_;
}

/**
 *  Collects requested client address information.
 *
 *  @param cli_fd       Client File descriptor.
 *  @param cli_addr     Client address information.
 *  @param msg          Packet frame sent from client.   TODO: ADD parameters
 *  @return ClientData
 */
ClientData Bridge::package_client_data(int cli_fd, struct sockaddr_in cli_addr) {
    char type = 's';              // Station or Router
    struct addrinfo *res=0;
    char host[100];
    
    // Gets client host name
    getaddrinfo(inet_ntoa(cli_addr.sin_addr),0,0,&res);
    getnameinfo(res->ai_addr,res->ai_addrlen,host,100,0,0,0);
    
    return ClientData(type, cli_fd, host, ntohs(cli_addr.sin_port));
}

/**
 *  Adds incoming port and MAC address to bridge table.
 *
 *  @param port         Incoming station port
 */
void Bridge::add_port(Port port) {
    btable_.insert(std::pair<Port, BridgeTableValues>(port, BridgeTableValues()));
}

/**
 *  Checks bridge table for associated station/router.
 *
 *  @param port         Incoming station port
 *  @param mac          Incoming MAC address
 *  @return             FD to send to, -1 if ARP entry not found
 */
int Bridge::has_arp_entry(Port port, MacAddr mac) {
    int result = -1;
    
    // Search through entire ARP cache
    if (port < 0) {
        for (BridgeTableItr itr = btable_.begin(); itr != btable_.end(); ++itr) {
            for (BridgeTableValuesItr itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
                if (mac.compare(itr2->first) == 0) {
                    result = itr->first;
                    break;
                }
            }
            if (result) break;
        }

        if (result != -1)
            debug.get_oss() << "ARP cache contains MAC Address: " << mac;
        else
            debug.get_oss() << "ARP Table does not contain MAC Address: " << mac;
        
        debug.print();
    } else {
        BridgeTableItr itr = btable_.find(port);
        if (itr != btable_.end()) {
            BridgeTableValuesItr itr2 = itr->second.find(mac);
            if (itr2 != itr->second.end())
                result = 1;
        }
    }
    
    return result;
}


/**
 *  Adds incoming port and MAC address to bridge table.
 *
 *  @param port         Incoming station port
 *  @param mac          Incoming station MAC address
 */
void Bridge::add_arp_entry(Port port, MacAddr mac) {
    milliseconds time =  std::chrono::duration_cast< milliseconds >
                            (std::chrono::system_clock::now().time_since_epoch());
    BridgeTableItr itr = btable_.find(port);
    itr->second.insert(std::pair<MacAddr, milliseconds>(mac, time));

    
    debug.get_oss() << "Adding MAC address: " << mac << " on port: " << port_;
    debug.print();
}

/**
 *  Checks for inactive station in ARP cache.
 */
void Bridge::monitor_arp_cache() {
    
    milliseconds time = std::chrono::duration_cast< milliseconds >
                            (std::chrono::system_clock::now().time_since_epoch());
    
    for (BridgeTableItr itr = btable_.begin(); itr != btable_.end(); ++itr) {
        for (BridgeTableValuesItr itr2 = itr->second.begin(); itr2 != itr->second.end();) {
            if ((time.count() - itr2->second.count()) > ARP_TIME.count()) {
                debug.get_oss() << "MAC Address: " << itr2->first << " timed out. Erasing... ";
                debug.print();
                
                itr->second.erase(itr2++);
            } else {
                ++itr2;
            }
        }
    }
}

/**
 *  Outputs the contents of the self-learning table;
 */
void Bridge::print_btable() {
    std::cout << "Showing Bridge Table" << std::endl;
    std::cout << std::left << std::setw(11) << "Port" << "MAC Address" << std::endl;
    for (auto p: btable_) {
        for (auto m: p.second) {
            std::cout << std::left << std::setw(8) << p.first << m.first << std::endl;
        }
    }
}

/**
 *  Initiates bridge. Listens for open/close requests from stations and
 *  routers, and regular data packets.
 */
void Bridge::start() {
    const int MAX_LINE = 100;
    char msg[MAX_LINE];
    size_t msg_size;
    bool stop = false;
    fd_set all_set, read_set;
    struct sockaddr_in cli_addr;
    unsigned int client_size = sizeof(cli_addr);
    int max_fd = listen_fd_;
    int nhop_fd;                                    // Next hop File descriptor
    EtherPkt pkt;
    
    FD_ZERO(&all_set);
    FD_SET(STDIN_FILENO, &all_set);
    FD_SET(listen_fd_, &all_set);

    debug.get_oss() << lan_name_ << ": started server on '" << info_->h_name
                    << "'' at '" << port_ << "'";
    debug.print();
    
    while(!stop) {
        // Block until input is receieved
        read_set = all_set;
        
        // Actively listens for connection setup request and
        // data frame arrival on established ports
        my_select(max_fd + 1, &read_set, NULL, NULL, NULL);
        
        monitor_arp_cache();
        
        if (FD_ISSET(listen_fd_, &read_set)) {
            for (unsigned int i = 0; i < FD_SETSIZE; ++i) {
                // New client (station/router) requesting connection
                if (i == listen_fd_) {
                    int cli_fd = WSocket::waccept(listen_fd_, (struct sockaddr *)&cli_addr,
                                              &client_size);
                    
                    clients_.push_back(package_client_data(cli_fd, cli_addr));
                    
                    // Update file descriptor set
                    FD_SET(cli_fd, &all_set);
                    
                    if (cli_fd > max_fd)
                        max_fd = cli_fd;
                    
                    debug.get_oss() << lan_name_ << ": connect from '" << clients_.back().ip_
                                    << "' at '" << ntohs(cli_addr.sin_port);
                    debug.print();
                    
                    ++curr_ports_;
                    
                    // If num_ports is at capacity, send Reject msg to client and close conn.
                    if (curr_ports_ > NUM_PORTS_) {
                        debug.get_oss() << "Rejected connection.";
                        debug.print();

                        my_write(cli_fd, "Rejected", sizeof("Rejected"));
                        remove_client(clients_.back(), (int)(clients_.size() - 1), all_set);
                    } else {
                        debug.get_oss() << "Accpeted connection.";
                        debug.print();
                        
                        my_write(cli_fd, "Accepted", sizeof("Accepted"));
                        
                        // Add port mapping to arp cache
                        add_port(cli_fd);
                    }
                }
            }
        } else {
            if (FD_ISSET(STDIN_FILENO, &read_set)) {
                if ((msg_size = my_read(STDIN_FILENO, msg, MAX_LINE))) {
                    if (strncmp(msg, "debug", sizeof(char)*5) == 0) {
                        debug.set_on(!debug.get_on());
                    } else if (strncmp(msg, "off", sizeof(char)*3) == 0){
                        stop = true;
                    } else if (strncmp(msg, "show sl", sizeof(char)*7) == 0) {
                        print_btable();
                    }
                }
                // Clear buffer
                memset(msg, '\0', sizeof(char) * MAX_LINE);
            } else {
                // Incoming ether frame packets
                for (unsigned int i = 0; i < clients_.size(); ++i) {
                    if (FD_ISSET(clients_[i].fd_, &read_set)) {
                        if (read(clients_[i].fd_, &pkt, sizeof(EtherPkt)) > 0) {
                            debug.get_oss() << "Received Ether Pkt";  debug.print(); EtherPkt::print(pkt);
                            // Check if dest MAC address is known
                            if ((nhop_fd = has_arp_entry(-1, pkt.mac_dst)) < 0) {
                                // Flood to all connected stations
                                for (unsigned int j = 0; j < clients_.size(); ++j) {
                                    if (i != j) {
                                        debug.get_oss() << "Destination MAC not known - Flooding to : "
                                                        << clients_[j].ip_;
                                        debug.print();
                                        
                                        if (write(clients_[j].fd_, &pkt, sizeof(pkt)) < 0)
                                            my_error("Bridge: sending packet to next hop.");
                                    }
                                }
                            } else {
                                debug.get_oss() << "Destination MAC known - sending to " << pkt.mac_dst;
                                debug.print();
                                //  Send to next hop
                                if (write(nhop_fd, &pkt, sizeof(pkt)) < 0)
                                    my_error("Bridge: sending packet to next hop.");
                            }
                            
                            debug.get_oss() << "Searching Bridge Table for SRC MAC.";
                            debug.print();
                            // Check if src MAC address is known
                            if (has_arp_entry(clients_[i].fd_, pkt.mac_src) < 0)
                                add_arp_entry(clients_[i].fd_, pkt.mac_src);
                        } else {
                            debug.get_oss()<< lan_name_ << ": disconnect from '"
                                << clients_[i].ip_ << "(" << ntohs(clients_[i].port_) << ")'";
                            debug.print();

                            remove_client(clients_[i], i, all_set);
                        }
                        break;
                    }
                }
                
                memset(&pkt, 0, sizeof(EtherPkt));
            }
            
            
        }
    }

}


