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
Bridge::Bridge(const char *lan_name, int num_ports, bool debug_on) :
    NUM_PORTS_(num_ports), curr_ports_(0), lan_name_(lan_name), debug("Bridge", debug_on) {
    
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
    content << ip_addr << port_;
    
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
        std::ostringstream ss3;
        ss3 << "Created symlink " << lan_name_ << " to file "
            << fname.str() << std::endl
            << "Contents: " << content.str();
        debug.print(ss3.str());
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
 *  @param mac          Incoming station MAC address
 *  @return             True if contains, else false
 */
bool Bridge::has_mapping(Port port, MacAddr mac) {
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
 *  @param mac          Incoming station MAC address
 */
void Bridge::add_mapping(Port port, MacAddr mac) {
    btable_.insert(std::pair<Port, MacAddr>(port, mac));
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
                    
                    std::cout << lan_name_ << ": connect from '" << clients_.back().host_
                    << "' at '" << ntohs(cli_addr.sin_port) << "'\n";
                    
                    ++curr_ports_;
                    
                    // If num_ports is at capacity, send Reject msg to client and close conn.
                    if (curr_ports_ > NUM_PORTS_) {
                        my_write(clients_.back().fd_, "Rejected", sizeof("Rejected"));
                        remove_client(clients_.back(), (int)(clients_.size() - 1), all_set);
                    } else {
                        my_write(clients_.back().fd_, "Accepted", sizeof("Accepted"));
                    }
                }
            }
        } else {
            if (FD_ISSET(STDIN_FILENO, &read_set)) {
                stop = true;
            } else {
                // TODO!!
                // Incoming data frame packets
                for (unsigned int i = 0; i < clients_.size(); ++i) {
                    if (FD_ISSET(clients_[i].fd_, &read_set)) {
                        if ((msg_size = my_read(clients_[i].fd_, msg, MAX_LINE)) > 0) {
                            // Output to server console
                            std::cout << clients_[i].host_ << "(" <<ntohs(clients_[i].port_)
                            << "): " << msg << std::endl;
                            for (unsigned int j = 0; j < clients_.size(); ++j) {
                                if (i != j) {
                                    my_write(clients_[j].fd_, msg, sizeof(msg));
                                }
                            }
                        } else {
                            std::cout << lan_name_ << ": disconnect from '"
                                      << clients_[i].host_ << "(" << ntohs(clients_[i].port_)
                                      << ")'\n";
                            remove_client(clients_[i], i, all_set);
                        }
                        break;
                    }
                }
            }
        }
    }

}


