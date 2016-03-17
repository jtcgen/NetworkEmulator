//
//  main.cpp
//  Network Emulator
//
//  Created by Travis Chung on 3/5/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#include <iostream>

#include "bridge.hpp"
#include "ip.hpp"



int main(int argc, const char * argv[]) {
    // insert code here...
    
    char lan[2] = "A";
    int ports = 3;
    bool debug = true;
    
    Bridge(lan, ports, debug);
    
    return 0;
}
