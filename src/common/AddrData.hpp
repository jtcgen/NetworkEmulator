//
//  AddrData.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/10/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef AddrData_hpp
#define AddrData_hpp

#include <stdio.h>
#include <arpa/inet.h>

class AddrData {
public:
    AddrData();
    AddrData(char* serv_name, char* port);

    struct hostent *info_;     // Holds host info
    struct sockaddr_in addr_;  // Contains server internet address information
    int port_;
};

#endif /* AddrData_hpp */
