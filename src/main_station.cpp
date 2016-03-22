//
//  main_station.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/17/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <iostream>

#include "client.hpp"




int main(int argc, const char * argv[]) {
    
    if (argc != 6)
        my_error(" R/S INTERFACE ROUTE_TABLE HOST_NAME DEBUG");
    
    if (IP::host_exists(argv[1]) == true)
        my_error("Host already exists.");
    
    if (IP::add_host(argv[1]) == false)
        my_error("Exceed host capacity.");
    
//    int ports = (int)atoi(argv[2]);
//    bool debug = (bool)atoi(argv[3]);
    
    
    
    return 0;
}