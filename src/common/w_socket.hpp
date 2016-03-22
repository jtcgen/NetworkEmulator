
#ifndef _W_SOCKET_H
#define _W_SOCKET_H

#include <sys/socket.h>
#include "utility.hpp"

class WSocket {
  public:
    /*      Wrappers        */
    static int wsocket(int domain, int type, int protocol);
    static int waccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    static int wlisten(int socket, int backlog);
    static int wbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    static int wgetsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
};

class SocketData {
  public:
    int fd;                                 // File descriptor
    struct hostent *info_;                  // Holds host info
    struct sockaddr_in addr_;               // Contains server internet address information
    
};


#endif


