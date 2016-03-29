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
    ARP_TIME(20000),
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
    fname << "./tmp/" << lan_name_ << ".txt";
    
    out.open(fname.str().c_str());
    
    if (!out) {
        std::ostringstream ss2;
        ss2 << "Failed to open " << fname.str().c_str() << ".";
        if (debug.get_on())
            debug.print(ss2.str());
        
        my_error(ss2.str());
    }
    
    // Write to file
    out << content.str();
    
    // Create symlink to file
    my_symlink(fname.str().c_str(), lan_name_.c_str());
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Created symlink " << lan_name_ << " to file "
            << fname.str() << std::endl
            << "Contents: " << content.str();
        debug.print(out.str());
    }
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
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Configured server on: " << info_->h_name;
        debug.print(out.str());
    }
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
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Created socket: " << listen_fd_ << " on port: " << port_;
        debug.print(out.str());
    }
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
    
    // TODO: Parse message and retrieve sender device type.
    
    return ClientData(type, cli_fd, host, ntohs(cli_addr.sin_port));
}

/**
 *  Checks bridge table for associated station/router.
 *
 *  @param port         Incoming station port
 *  @return             True if contains, else false
 */
bool Bridge::has_port(Port port) {
    bool result = false;
    BridgeTableItr itr = btable_.find(port);
    
    if (itr != btable_.end())
        result = true;
    
    return result;
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
 *  @param station      Incoming station name
 *  @return             True if contains, else false
 */
bool Bridge::has_station(Port port, std::string station) {
//    for (BridgeTableItr itr = btable_.begin(); itr != btable_.end(); ++itr) {
//        if (itr->first == port) {
//            for (BridgeTableValuesItr itr2 = itr->second.begin(); itr2 != itr->second.end(); ++itr2) {
//                if (itr2->second == station) {
//                    return true;
//                }
//            }
//        }
//    }
    
    bool result = false;
    BridgeTableItr itr = btable_.find(port);
    if (itr != btable_.end()) {
        result = true;
    } else {
        BridgeTableValuesItr itr2 = itr->second.find(station);
        if (itr2 != itr->second.end())
            result = true;
    }
    
    return result;
}


/**
 *  Adds incoming port and MAC address to bridge table.
 *
 *  @param port         Incoming station port
 *  @param mac          Incoming station MAC address
 */
void Bridge::add_station(Port port, std::string station, MacAddr mac) {
//    for (BridgeTableItr itr = btable_.begin(); itr != btable_.end(); ++itr) {
//        if (itr->first == port) {
//            itr->second.insert(std::pair<std::string, MacAddr>(station, mac));
//        }
//    }
    
    ArpEntry *entry = new ArpEntry(station, mac, std::chrono::duration_cast< milliseconds >
                                   (std::chrono::system_clock::now().time_since_epoch()));
    
    BridgeTableItr itr = btable_.find(port);
    itr->second.insert(std::pair<std::string, ArpEntry*>(station, entry));
}

/**
 *  Checks for inactive station in ARP cache.
 */
void Bridge::monitor_arp_cache() {
    
    milliseconds time = std::chrono::duration_cast< milliseconds >
                            (std::chrono::system_clock::now().time_since_epoch());
    
    for (BridgeTableItr itr = btable_.begin(); itr != btable_.end(); ++itr) {
        for (BridgeTableValuesItr itr2 = itr->second.begin(); itr2 != itr->second.end();) {
            if ((time - itr2->second->init_time_) > ARP_TIME)
                itr->second.erase(itr2++);
            else
                ++itr2;
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
    
    FD_ZERO(&all_set);
    FD_SET(STDIN_FILENO, &all_set);
    FD_SET(listen_fd_, &all_set);
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << lan_name_ << ": started server on '" << info_->h_name
            << "'' at '" << port_ << "'" << std::endl;
        debug.print(out.str());
    }
    
    while(1 && !stop) {
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
                    
                    
                    if (debug.get_on()) {
                        std::ostringstream out;
                        out << lan_name_ << ": connect from '" << clients_.back().host_
                        << "' at '" << ntohs(cli_addr.sin_port);
                        debug.print(out.str());
                    }
                    
                    ++curr_ports_;
                    
                    // If num_ports is at capacity, send Reject msg to client and close conn.
                    if (curr_ports_ > NUM_PORTS_) {
                        if (debug.get_on()) {
                            std::ostringstream out;
                            out << "Rejected connection.";
                            debug.print(out.str());
                        }
                        
                        my_write(clients_.back().fd_, "Rejected", sizeof("Rejected"));
                        remove_client(clients_.back(), (int)(clients_.size() - 1), all_set);
                    } else {
                        if (debug.get_on()) {
                            std::ostringstream out;
                            out << "Accpeted connection.";
                            debug.print(out.str());
                        }
    
                        my_write(clients_.back().fd_, "Accepted", sizeof("Accepted"));
                        
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
                    }
                }
            } else {
                // TODO!!
                // Incoming data frame packets
                for (unsigned int i = 0; i < clients_.size(); ++i) {
                    if (FD_ISSET(clients_[i].fd_, &read_set)) {
                        if ((msg_size = my_read(clients_[i].fd_, msg, MAX_LINE)) > 0) {
                            // Check if dest MAC address is known
                            
                            // Check if src MAC address is known
//                            if (has_station(clients_[i].fd_, STATION_NAME) == false)
//                                add_station(clients_[i].fd_, STATION_NAME, SRC_MAC_ADDR);


                            
                        } else {
                            if (debug.get_on()) {
                                std::ostringstream out;
                                out << lan_name_ << ": disconnect from '"
                                      << clients_[i].host_ << "(" << ntohs(clients_[i].port_)
                                      << ")'\n";
                                debug.print(out.str());
                            }
                            remove_client(clients_[i], i, all_set);
                        }
                        break;
                    }
                }
            }
            
            // Clear buffer
            memset(msg, '\0', sizeof(char) * MAX_LINE);
        }
    }

}


