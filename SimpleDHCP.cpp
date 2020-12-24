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

// ********** DHCP SERVER TESTER **********

DHCP_TESTER::DHCP_TESTER() {
    uint8_t test_client_mac[] = {0x01, 0xEA, 0xFD, 0xBC, 0x05, 0x06};
    _dhcp_server = new DHCP_SERVER(IPAddress(10, 0, 0, 1), 32);
    _dhcp_client = new DHCP_CLIENT(test_client_mac, 6);
}

DHCP_TESTER::~DHCP_TESTER() {
    Serial.println("Destroying DHCP Tester");
    delete _dhcp_server;
    delete _dhcp_client;
}

bool DHCP_TESTER::runTests() {
    Serial.println("Running DHCP tests");
    Serial.println();
    Serial.println("**************************************************");
    Serial.println("  - Test 1: DHCP Client Discovery");
    Serial.println();
    DHCP_MESSAGE test_message = _dhcp_client->createDHCPMessage(DHCP_DISCOVER, 0);
    _dhcp_server->printDHCPMessage(test_message);
    _dhcp_server->printRawUDPPayload((uint8_t *)&test_message, sizeof((uint8_t *)&test_message));
    if (test_message.op != DHCP_BOOTREQUEST) return false;
    if (test_message.xid != 0) return false;
    return true; // If we reached here then all the tests passed
}