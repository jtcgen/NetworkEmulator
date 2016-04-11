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


/******************************************************************/
/*                          LOG                                   */
/******************************************************************/

std::ostringstream& Log::get_oss() {
    oss_.str(std::string(""));
    oss_.clear();
    return oss_;
}

/**
 *  Outputs to stdout contents of oss.
 *
 */
void Log::print() {
    if (on_) {
        std::cout << "Logging - " << type_ << ": " << oss_.str() << std::endl;
        clear();
    }
}

/**
 *  Clears out out stringstream object.
 */
void Log::clear() {
    oss_.str("");
    oss_.clear();
}

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
        
        if (*(ptr-1) == '\n') { // Read appends \r\n
            *(ptr-1) = '\0';
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
 *  readlink wrapper
 *
 *  @param path         Filename path
 *  @param buf          Contents of the symbolic link
 *  @param bufsiz       Size of the buffer
 *  @return             Number of bytes placed in buf
 */
ssize_t my_readlink(const char *path, char *buf, size_t bufsiz) {
    ssize_t n;
    if ((n = readlink(path, buf, bufsiz)) == -1)
        my_error("Error: could not read symlink.");
    return n;
}





