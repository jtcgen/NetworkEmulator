//
//  ClientData.h
//  Network Emulator
//
//  Created by Travis Chung on 3/10/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef ClientData_h
#define ClientData_h

class ClientData {
    friend class Bridge;
    
private:
    char type_;     // Station or Bridge
    int fd_;
    
};

#endif /* ClientData_h */
