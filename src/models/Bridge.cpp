//
//  Bridge.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include "Bridge.hpp"
#include "utility.hpp"

Bridge::Bridge(char *lan_name, int num_ports) : num_ports_(num_ports) {
    strncpy(lan_name_, lan_name, strlen(lan_name));
    
    // Configure server information
    size_t serv_size = sizeof(serv_addr_);
    bzero(&serv_addr_, serv_size);
    serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons((short)port_);
    
}

Bridge::Bridge(short port) : port_(port) {
    // Configure internet address
    size_t serv_size = sizeof(serv_addr_);
    bzero(&serv_addr_, serv_size);
    serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons((short)port_);
    
    char temp_host[100];
    my_gethostname(temp_host, sizeof(temp_host));
    serv_info_ = gethostbyname(temp_host);
}


Bridge::~Bridge() {
    
}

void Bridge::start() {
    
}

