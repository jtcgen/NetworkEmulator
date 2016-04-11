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

#include "client.hpp"

class Station: public Client {
public:
    Station(const char *iface,
            const char *rtable,
            const char *hname,
            bool is_router);
    ~Station();
    
    void start();
    
private:
    
};

#endif /* Station_hpp */
