//
//  main.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/5/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//
//
#include <stdlib.h>
#include <iostream>

#include "bridge.hpp"
#include "utility.hpp"
#include "ip.hpp"

int main(int argc, const char * argv[]) {
    
    if (argc != 3)
        my_error("LAN_NAME NUM_PORTS");
    
    if (IP::host_exists(argv[1]) == true)
        my_error("Host already exists.");
    
    if (IP::add_host(argv[1]) == false)
        my_error("Exceed host capacity.");
    
    int ports = (int)atoi(argv[2]);

    Bridge *b = new Bridge(argv[1], ports);
    
    b->start();
    
    delete b;
    
    return 0;
}
