//
//  Client.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <string.h>
#include <cstdlib>
#include <unistd.h>

#include "Client.hpp"
#include "utility.hpp"

Client::Client(char *iface, char *rtable, char *host_name) {
    
}

Client::Client(char *serv_name, char *serv_port) {
    serv_port_ = atoi(serv_port);
    strncpy(serv_name_, serv_name, strlen(serv_name));
    struct hostent *server = my_gethostbyname(serv_name);
    
    // Configure server address info
    bzero(&serv_addr_, sizeof(serv_addr_));
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(serv_port_);
    bcopy((char*)server->h_addr, (char*)&serv_addr_.sin_addr.s_addr, server->h_length);
    //inet_pton(AF_INET, serv_name, &serv_addr_.sin_addr);
    
    // Create socket to host communication
//    serv_fd_ = my_socket(AF_INET, SOCK_STREAM, 0);
}
