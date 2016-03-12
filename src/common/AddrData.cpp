//
//  AddrData.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/10/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <stdlib.h>
#include <string.h>

#include "AddrData.hpp"
#include "utility.hpp"

/**
    Initializes address information for Server type classes.
 
    @param none.
    @return none.
 */
AddrData::AddrData() {
    // Configure Server information
    size_t serv_size = sizeof(addr_);
    bzero(&addr_, serv_size);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons((short)0);      // Bind to random available port
    
    char temp_host[100];
    my_gethostname(temp_host, sizeof(temp_host));
    info_ = gethostbyname(temp_host);
}

/**
    Initializes address information for Client type classes.
 
    @param serv_name ASCII representation of lan name.
    @param port Port of server.
    @return none.
 */
AddrData::AddrData(char *serv_name, char *port) {
    // Configure Client information
    port_ = atoi(port);
    struct hostent *info_ = my_gethostbyname(serv_name);
    
    // Configure server address info
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port_);
    bcopy((char*)info_->h_addr, (char*)&addr_.sin_addr.s_addr, info_->h_length);
}
