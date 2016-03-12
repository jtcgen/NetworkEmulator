//
//  Station.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/11/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Station_hpp
#define Station_hpp

#include <stdio.h>
#include <string>

#include "Client.hpp"

class Station: Client {
public:
    Station(char *iface, char *rtable, char *hname);
    ~Station();
    
    void start();
    
private:
//    void read_symbolic_link(std::string link, AddrData *bridge);
    
    
    
};

#endif /* Station_hpp */
