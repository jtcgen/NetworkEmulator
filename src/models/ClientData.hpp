//
//  ClientData.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/10/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef ClientData_hpp
#define ClientData_hpp

#include <string>

class ClientData {
    friend class Bridge;
    
    ClientData(int fd, char *host, short port) :
        fd_(fd), host_(host), port_(port) { };
    
private:
    char type_;     // Station or Bridge
    int fd_;
    int port_;
    std::string host_;
    
};

#endif /* ClientData_h */
