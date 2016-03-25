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
public:
    friend class Bridge;
    
    ClientData(char type, int fd, char *host, short port) :
        type_(type), fd_(fd), port_(port), host_(host) { };
    
private:
    char type_;     // Station or Router
    int fd_;
    int port_;
    std::string host_;
    
};

#endif /* ClientData_h */
