//
//  main_station.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/17/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <iostream>

#include "station.hpp"

int main(int argc, const char **argv) {
    
    if (argc != 5)
        my_error(" R/S INTERFACE ROUTE_TABLE HOST_NAME");

    bool is_router = false;
    if (std::string(argv[1]).compare(std::string("-route")) == 0)
        is_router = true;
    
    Station *s = new Station(argv[2], argv[3], argv[4], is_router);
    s->start();
    
    delete s;
    
    return 0;
}
