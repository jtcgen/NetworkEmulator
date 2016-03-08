//
//  Bridge.hpp
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef Bridge_hpp
#define Bridge_hpp

#include <stdio.h>

#include "Server.hpp"


class Bridge: public Server {
public:
    Bridge(short port);
    ~Bridge();
    
    void start();
    
private:
    
};

#endif /* Bridge_hpp */
