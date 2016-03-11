
#ifndef _W_SOCKET_H
#define _W_SOCKET_H

#include <sys/socket.h>
#include "utility.hpp"

class WSocket {
public:
    /*      Wrappers        */
    static int socket(int domain, int type, int protocol);
    static int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    static int listen(int socket, int backlog);
    static int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    static int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
};

#endif


