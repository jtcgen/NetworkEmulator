
#ifndef _W_SOCKET_H
#define _W_SOCKET_H

#include <sys/socket.h>
#include "utility.hpp"
#include "ip.hpp"

class WSocket {
  public:
    /*      Wrappers        */
    static int wsocket(int domain, int type, int protocol);
    static int waccept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    static int wlisten(int socket, int backlog);
    static int wbind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    static int wgetsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
};

typedef struct socket_data {
    IPAddr ip_addr;
    Port port;
    int fd;                                // File descriptor
    struct hostent *info;                  // Holds host info
    socklen_t sock_size;
    struct sockaddr_in sock_addr;          // Contains server internet address information
    
} SocketData;


#endif


