/* 
 * SimpleDHCP: Library for simple DHCP client and server functionality
 * Version: v0.0.5
 * 
 * File: SimpleDHCP.h
 * Author: Derek M. Blue
 * Contact: derekmblue4011@gmail.com
 * Copyright (c) 2020
 * 
 * License: GNU LESSER GENERAL PUBLIC LICENSE Version 3
 *          See included LICENSE file
 * 
 * Commercial Use: Contact me to negotiate a license for commercial use
 */

#include "SimpleDHCP.h"

// ********** DHCP CLIENT **********
// TODO: Implement this class

DHCP_CLIENT::DHCP_CLIENT(uint8_t chaddr[], uint8_t hlen) {
    for (int i = 0; i < 16; i++) {
        if (i < hlen) {
            H_ADDRESS[i] = chaddr[i];
        } else {
            H_ADDRESS[i] = 0;
        }
    }
}

DHCP_CLIENT::~DHCP_CLIENT() {
    ;
}

DHCP_MESSAGE DHCP_CLIENT::createDHCPMessage(uint8_t message_type, uint32_t xid) {
    DHCP_MESSAGE message;
    message.op = DHCP_BOOTREQUEST;
    message.htype = DHCP_ETHERNET;
    message.hlen = DHCP_MAC_ADDRESS_LENGTH;
    message.hops = 0;
    message.xid = xid;
    message.secs = 0;
    message.flags = DHCP_BROADCAST_FLAG;
    for (int i = 0; i < 4; i++) {
        message.ciaddr[i] = 0;
        message.yiaddr[i] = 0;
        message.siaddr[i] = 0;
        message.giaddr[i] = 0;
    }
    for (int i = 0; i < 16; i++) {
        message.chaddr[i] = 0;
    }
    for (int i = 0; i < message.hlen; i++) {
        message.chaddr[i] = H_ADDRESS[i];
    }
    return message;
}