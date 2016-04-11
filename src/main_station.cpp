//
//  main_station.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/17/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <iostream>

#include "client.hpp"

int main(int argc, const char **argv) {
    
    if (argc != 5)
        my_error(" R/S INTERFACE ROUTE_TABLE HOST_NAME");

    bool is_router = false;
    if (std::string(argv[1]).compare(std::string("-route")) == 0)
        is_router = true;
//    if (IP::host_exists(argv[1]) == true)
//        my_error("Host already exists.");
//    
//    if (IP::add_host(argv[1]) == false)
//        my_error("Exceed host capacity.");
    
    Client *c = new Client(argv[2], argv[3], argv[4], is_router);
    c->start();
    
    delete c;
    
    return 0;
}
