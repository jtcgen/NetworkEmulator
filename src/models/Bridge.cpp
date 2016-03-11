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
#include "utility.hpp"

/**
    Constructor
 
    @param lan_name Name of the lan
    @param num_ports Max number of ports that bridge can handle
    @param debug_on Flag to set debug settings
*/
Bridge::Bridge(char *lan_name, int num_ports, bool debug_on) : num_ports_(num_ports),
                                                               debug("Bridge", debug_on) {
    lan_name_ = new char[strlen(lan_name)];
    strncpy(lan_name_, lan_name, strlen(lan_name));

    
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
    delete [] lan_name_;
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
    
    inet_ntop(AF_INET, (void*)&addr_.sin_addr, addr_buffer, INET_ADDRSTRLEN);
    sprintf(content, "%s %hu", addr_buffer, port_);
    my_symlink(content, lan_name_);
}

/**
    Establishes bridge address information.
 
    @param none
    @return none
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
    Establishes socket information for incoming and outgoing
    information.
 
    @param none
    @return none
 */
void Bridge::setup_socket() {
    listen_fd_ = WSocket::socket(AF_INET, SOCK_STREAM, 0);
    WSocket::bind(listen_fd_, (struct sockaddr*) &addr_, sizeof(addr_));
    WSocket::listen(listen_fd_, num_ports_);
    
    // Get Port number
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    WSocket::getsockname(listen_fd_, (struct sockaddr*)&sin, &len);
    port_ = (unsigned short)ntohs(sin.sin_port);
    
    if (debug.get_on()) {
        std::ostringstream out;
        out << "Created socket: " << listen_fd_ << " on port: " << port_;
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
    fd_set all_set, read_set;
    struct sockaddr_in client_addr;
    unsigned int client_size = sizeof(client_addr);
    int max_fd = listen_fd_;
    
    FD_ZERO(&all_set);
    FD_SET(listen_fd_, &all_set);
    
    if (debug.get_on())
        debug.print("Started");
    
    std::cout << "admin: started server on '" << info_->h_name
    << "'' at '" << port_ << "'" << std::endl;

}


