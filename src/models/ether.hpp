//
//  ether.h
//  Network Emulator
//
//  Created by Travis Chung on 3/8/16.
//  Copyright © 2016 jtcgen. All rights reserved.
//

#ifndef ETHER_HPP
#define ETHER_HPP

#define PEER_CLOSED 2
#define TYPE_IP_PKT 1
#define TYPE_ARP_PKT 0

#include <string>

//#include "ip.hpp"


//typedef std::string MacAddr;

///* structure of an ethernet pkt */
//typedef struct __etherpkt
//{
//    /* destination address in net order */
//    MacAddr dst;
//    
//    /* source address in net order */
//    MacAddr src;
//    
//    /************************************/
//    /* payload type in host order       */
//    /* type = 0 : ARP frame             */
//    /* type = 1 : IP  frame             */
//    /************************************/
//    short  type;
//    
////    /* size of the data in host order */
////    short   size;
////    
////    struct arp_pkt arp_;
////
////    /*      IP      */
////    IpPkt ip_;
//
//} EtherPkt;

#endif /* ETHER_H */
