//
//  Bridge.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <iostream>

#include "Bridge.hpp"

#include "w_socket.hpp"
/**
    Constructor
 
    @param lan_name Name of the lan
    @param num_ports Max number of ports that bridge can handle
    @param debug_on Flag to set debug settings
*/
Bridge::Bridge(char *lan_name, int num_ports, bool debug_on) : NUM_PORTS_(num_ports),
                                                               lan_name_(lan_name),
                                                               debug("Bridge", debug_on) {
    
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
    delete addr_;
}


/**
    Creates the symbolic link to its address and port number
    so that others (stations/routers) can connect to it.
 
    @param none
    @return none
*/
void Bridge::create_symlink() {
    char content[30];                   // Msg of symlink
    char addr_buffer[INET_ADDRSTRLEN];
    
    inet_ntop(AF_INET, (void*)&addr_->addr_.sin_addr, addr_buffer, INET_ADDRSTRLEN);
    sprintf(content, "%s %d", addr_buffer, addr_->port_);
    my_symlink(content, lan_name_.c_str());
}

/**
    Establishes bridge address information.
 
    @param none
    @return none
*/
void Bridge::setup_server_info() {
    addr_ = new AddrData;
    // Configure server information
    size_t serv_size = sizeof(addr_);
    bzero(&addr_, serv_size);
    addr_->addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_->addr_.sin_family = AF_INET;
    addr_->addr_.sin_port = htons((short)0);      // Bind to random available port
    
    char temp_host[100];
    my_gethostname(temp_host, sizeof(temp_host));
    addr_->info_ = gethostbyname(temp_host);
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Configured server on: " << addr_->info_->h_name;
        debug.print(out.str());
    }
}

/**
    Establishes socket information for incoming and outgoing
    information.
 
    @param none
    @return none
 */
void Bridge::setup_socket() {
    listen_fd_ = WSocket::socket(AF_INET, SOCK_STREAM, 0);
    WSocket::bind(listen_fd_, (struct sockaddr*) &addr_, sizeof(addr_));
    WSocket::listen(listen_fd_, NUM_PORTS_);
    
    // Get Port number
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    WSocket::getsockname(listen_fd_, (struct sockaddr*)&sin, &len);
    addr_->port_ = (unsigned short)ntohs(sin.sin_port);
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Created socket: " << listen_fd_ << " on port: " << addr_->port_;
        debug.print(out.str());
    }
}

/**
    Initiates bridge. Listens for open/close requests from stations and
    routers, and regular data packets.
 
    @param none
    @return none
*/
void Bridge::start() {
    const int MAX_LINE = 100;
    char msg[MAX_LINE];
    int msg_size;
    fd_set all_set, read_set;
    struct sockaddr_in client_addr;
    unsigned int client_size = sizeof(client_addr);
    int max_fd = listen_fd_;
    
    FD_ZERO(&all_set);
    FD_SET(listen_fd_, &all_set);
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "admin: started server on '" << addr_->info_->h_name
            << "'' at '" << addr_->port_ << "'" << std::endl;
        debug.print(out.str());
    }
    
    while(1) {
        // Block until input is receieved
        read_set = all_set;
        my_select(max_fd + 1, &read_set, NULL, NULL, NULL);
        
        if (FD_ISSET(listen_fd_, &read_set)) {
            for (unsigned int i = 0; i < FD_SETSIZE; ++i) {
                // New Client requesting connection
                if (i == listen_fd_) {
                    int client_fd = WSocket::accept(listen_fd_, (struct sockaddr *)&client_addr,
                                              &client_size);
                    
                    // Get client host name
                    struct addrinfo *res=0;
                    char host[100];
                    getaddrinfo(inet_ntoa(client_addr.sin_addr),0,0,&res);
                    getnameinfo(res->ai_addr,res->ai_addrlen,host,100,0,0,0);
                    
                    
                    clients_.push_back(ClientData(client_fd, host, ntohs(client_addr.sin_port)));
                    
                    // Update file descriptor set
                    FD_SET(client_fd, &all_set);
                    
                    if (client_fd > max_fd)
                        max_fd = client_fd;
                    
                    std::cout << "admin: connect from '" << host
                    << "' at '" << ntohs(client_addr.sin_port) << "'\n";
                }
            }
        } else {
            // Check clients for data
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
                        std::cout << "admin: disconnect from '"
                        << clients_[i].host_ << "(" << ntohs(clients_[i].port_)
                        << ")'\n";
                        // Client disconnected, remove from client
                        close(clients_[i].fd_);
                        FD_CLR(clients_[i].fd_, &all_set);
                        clients_.erase(clients_.begin()+i);
                    }
                    break;
                }
            }
        }
    }

}


