//
//  utility.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/5/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <cerrno>

#include "utility.hpp"
#include "ip.hpp"


/**
 *  error wrapper function
 *
 *  @param msg      Specific message to be outputted.
 */
void my_error(std::string msg) {
    if (errno)
        std::cerr << strerror(errno) << ": " << msg << std::endl;
    else
        std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}

/**
 *  logging function
 *
 *  @param msg      Message to be logged.
 */
void Log::print(std::string msg) {
    std::cout << "Logging - " << type_ << ": " << msg << std::endl;
}

/**
 *  read wrapper function
 *
 *  @param fd       File descriptor to read from
 *  @param vptr     Message container
 *  @param n        Max size to be read
 *  @return         Number of bytes read
 */
ssize_t my_read(int fd, void *vptr, size_t n) {
    ssize_t nleft;
    ssize_t nread;
    char *ptr;
    
    ptr = (char*)vptr;
    nleft = n;
    while (nleft >0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR) {
                nread = 0; // Call read() again
            } else my_error("Read Error");
        } else if (nread == 0) break;   // EOF
        
        nleft -= nread;
        ptr += nread;
        
        if (*(ptr-2) == '\r') { // Read appends \r\n
            *(ptr-2) = '\0';
            break;
        }
        
    }
    return n - nleft;
}

/**
 *  write wrapper function
 *
 *  @param fd       File descriptor to write to
 *  @param vptr     Message to be sent
 *  @param n        Size of message
 *  @return         Number of bytes written
 */
ssize_t my_write(int fd, const void *vptr, size_t n) {
    ssize_t nleft;
    ssize_t nwritten;
    const char *ptr;
    
    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;   // call write again
            else my_error("Write Error");
        }
        
        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}

/**
    select function wrapper
 
    @param nfds     Socket file descriptor
    @param rfds     Read fds ready to be read
    @param wfds     Write fds ready to write
    @param efds     Error fds checked for error
    @param to       Max time to wait
    @return         Active file descriptor
 */
int my_select(int nfds, fd_set *rfds, fd_set *wfds,
              fd_set *efds, struct timeval *to) {
    int num_fd;
    if ((num_fd = select(nfds, rfds, wfds, efds, to)) == -1)
        my_error("Select failed");
    
    return num_fd;
}

/**
 *
 *  hostbyname wrapper
 *
 *   @param name    Host name
 *   @return        Host structure
 */
struct hostent* my_gethostbyname(char *name) {
    struct hostent *server = gethostbyname(name);
    
    if (server == 0)
        my_error("Error: no such host");
    
    return server;
}

/**
 *
 *  gethostname wrapper
 *
 *  @param name     cString to contain name of host
 *  @param len      Max len of cString
 */
void my_gethostname(char *name, size_t len) {
    if (gethostname(name, len) == -1)
        my_error("Error: Could not get host name.");
}

/**
 *
 *  File close wrapper
 *
 *  @param fd       File descriptor to be closed
 *  @return         Status of close return
 */
int my_close(int fd) {
    if (close(fd) == -1) my_error("Close failed.");
    return 0;
}

/**
 *  symlink wrapper
 *
 *  @param content      String contained in symlink
 *  @param fname        Name of symlinnk
 */
void my_symlink(const char *path1, const char *path2) {
    if (symlink(path1, path2) == -1)
        my_error("Error: could not create symlink.");
}


/**
 *  Checks if host is already in existence.
 *
 *  @param host     Name of Host
 */
bool host_exists(std::string h) {
    bool result = false;
    
    for (int i = 0; i < IP::hostcnt; ++i) {
        if (strcmp(h.c_str(), IP::host[i].name.c_str()) == 0) {
            result = true;
            break;
        }
    }
    return result;
}



