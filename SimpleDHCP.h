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

#ifndef SIMPLE_DHCP_H
#define SIMPLE_DHCP_H

// DHCP Unit Tester
class DHCP_TESTER {
private:
    // Members
public:
    DHCP_TESTER();
    ~DHCP_TESTER();
    bool runTests();
};

#endif