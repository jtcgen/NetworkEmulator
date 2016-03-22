//
//  socket_wrapper.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/5/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <cerrno>

#include "w_socket.hpp"

int WSocket::wsocket(int domain, int type, int protocol) {
    int fd;
    if ((fd = socket(domain, type, protocol)) == -1 )
        my_error("Socket: Socket creation failed.");
    
    return fd;
}

int WSocket::waccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    int fd;
    while ((fd = accept(sockfd, addr, addrlen)) == -1) {
        if (errno == EINTR)
            continue;
        else
            my_error("Socket accept failed.");
    }
    
    return fd;
}

int WSocket::wlisten(int socket, int backlog) {
    if (listen(socket, backlog) == -1)
        my_error("Socket: Listen Failed.");
    
    return 0;
}

int WSocket::wbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) == -1)
        my_error("Socket: Bind failed");
    
    return 0;
}

int WSocket::wgetsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    if (getsockname(sockfd, addr, addrlen) == -1)
        my_error("Socket: getsockname failed.");
    
    return 0;
}

