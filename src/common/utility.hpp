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
#include <sstream>
#include <netdb.h>

#define EXIT_FAILURE 1

void my_error(std::string err);


void my_log(std::string msg);

int my_close(int fd);
ssize_t my_read(int fd, void *buff, size_t nbytes);
ssize_t my_write(int fd, const void *buff, size_t nbytes);
struct hostent* my_gethostbyname(char *name);
void my_gethostname(char *name, size_t len);
int my_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
void my_symlink(char *content, char *fname);


class Log {
public:
    Log(std::string type, bool on) : type_(type), on_(on) { };
    void print(std::string msg);
    
    /*  Accessors   */
    bool get_on() { return on_; }

private:
    std::string type_;
    bool on_;
};

#endif /* utility_h */
