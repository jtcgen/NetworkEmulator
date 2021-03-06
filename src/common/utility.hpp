//
//  utility.h
//  Network Emulator
//
//  Created by Travis Chung on 3/5/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef utility_h
#define utility_h

#include <sys/select.h>
#include <string>
#include <cstring>
#include <sstream>
#include <netdb.h>


#define EXIT_FAILURE 1

/**
 *  error wrapper function
 *
 *  @param msg      Specific message to be outputted.
 */
void my_error(std::string err);

/**
 *  logging function
 *
 *  @param msg      Message to be logged.
 */
void my_log(std::string msg);

/**
 *
 *  File close wrapper
 *
 *  @param fd       File descriptor to be closed
 *  @return         Status of close return
 */
int my_close(int fd);

/**
 *  read wrapper function
 *
 *  @param fd       File descriptor to read from
 *  @param vptr     Message container
 *  @param n        Max size to be read
 *  @return         Number of bytes read
 */
ssize_t my_read(int fd, void *buff, size_t nbytes);

/**
 *  write wrapper function
 *
 *  @param fd       File descriptor to write to
 *  @param vptr     Message to be sent
 *  @param n        Size of message
 *  @return         Number of bytes written
 */
ssize_t my_write(int fd, const void *buff, size_t nbytes);

/**
 *
 *  hostbyname wrapper
 *
 *  @param name     Host name
 *  @return         Host structure
 */
struct hostent* my_gethostbyname(char *name);

/**
 *
 *  gethostname wrapper
 *
 *  @param name     cString to contain name of host
 *  @param len      Max len of cString
 */
void my_gethostname(char *name, size_t len);

/**
 *  select function wrapper
 *
 *  @param nfds        Socket file descriptor
 *  @param rfds        Read fds ready to be read
 *  @param wfds        Write fds ready to write
 *  @param efds        Error fds checked for error
 *  @param timeout     Max time to wait
 *  @return            Active file descriptor
 */
int my_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

/**
 *  symlink wrapper
 *
 *  @param content String contained in symlink
 *  @param fname Name of symlinnk
 */
void my_symlink(const char *content, const char *fname);

/**
 *  readlink wrapper
 *
 *  @param path         Filename path
 *  @param buf          Contents of the symbolic link
 *  @param bufsiz       Size of the buffer
 *  @return             Number of bytes placed in buf
 */
ssize_t my_readlink(const char *path, char *buf, size_t bufsiz);

class Log {
public:
    Log(std::string type,
        bool on) :
        type_(type),
        on_(on) { clear(); };
    
    void print();
    
    /*  Accessors   */
    bool get_on() { return on_; }
    void set_on(bool on) { on_ = on; }
    std::ostringstream& get_oss();
    std::ostringstream out;
    
private:
    void clear();
    
    std::ostringstream oss_;
    std::string type_;
    bool on_;
};

#endif /* utility_h */
