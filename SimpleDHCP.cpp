/* 
 * SimpleDHCP: Library for simple DHCP client and server functionality
 * Version: v0.0.17
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

// ********** DHCP SERVER **********

// DHCP_SERVER Default constructor, this constructor should be avoided
DHCP_SERVER::DHCP_SERVER() {
    SERVER_ADDRESS = IPAddress(10,0,0,1);
    assignAddressPool(SERVER_ADDRESS, 255);
    _verbose = false;
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
}

// DHCP_SERVER Intended Constructor, sets the address pool and server IP
DHCP_SERVER::DHCP_SERVER(IPAddress server_address, uint8_t range) {
    SERVER_ADDRESS = server_address;
    assignAddressPool(SERVER_ADDRESS, range);
    _verbose = false;
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
}

// DHCP_SERVER Intended Constructor, sets the address pool, server IP, and verbosity
DHCP_SERVER::DHCP_SERVER(IPAddress server_address, uint8_t range, bool verbose) {
    SERVER_ADDRESS = server_address;
    assignAddressPool(SERVER_ADDRESS, range);
    _verbose = verbose;
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
    if (_verbose) Serial.println("DHCP UDP Socket opened");
}

// DHCP Server Destructor
DHCP_SERVER::~DHCP_SERVER() {
    delete [] _addresses;
}

// Set the DHCP Address Pool
void DHCP_SERVER::assignAddressPool(IPAddress server_address, uint8_t address_range) {
    address_pool.firstOctet  = server_address[0];
    address_pool.secondOctet = server_address[1];
    address_pool.thirdOctet  = server_address[2];
    if (address_range > 255) {
        address_pool.range = 255;
    } else {
        address_pool.range = address_range;
    }
    _addresses = new bool[address_pool.range];
    for (int i = 0; i < address_pool.range; i++) {
        _addresses[i] = true;
    }
}

// Assign network address from available addresses in the pool
IPAddress DHCP_SERVER::assignAddress(IPAddress requested_ip) {
    IPAddress address;
    if (isAddressAvailable(requested_ip)) {
        address = requested_ip;
    } else {
        address = getAddressFromPool();
    }
    if (isAddressAvailable(address)) _addresses[address[3] - 2] = false;
    return address;
}

// Get a valid network address from the address pool
IPAddress DHCP_SERVER::getAddressFromPool() {
    uint8_t fourthOctet;
    for (int i = 0; i < address_pool.range; i++) {
        if (_addresses[i]) {
            fourthOctet = i + 2;
            break;
        }
    }
    IPAddress address(address_pool.firstOctet, address_pool.secondOctet, address_pool.thirdOctet, fourthOctet);
    if (isAddressAvailable(address)) {
        return address;
    } else {
        return IPAddress(0, 0, 0, 0);
    }
}

// Check if a network address is in the pool and available
bool DHCP_SERVER::isAddressAvailable(IPAddress address) {
    if (address[0] != address_pool.firstOctet) return false;
    if (address[1] != address_pool.secondOctet) return false;
    if (address[2] != address_pool.thirdOctet) return false;
    if (address[3] > address_pool.range) return false;
    return _addresses[address[3] - 2];
}

// Release assigned address
void DHCP_SERVER::releaseAddress(IPAddress address) {
    if (!_addresses[address[3] - 2]) {
        _addresses[address[3] - 2] = true;
    }
}

// Parse DHCP Messages: If received from client then will allocate an address as required
DHCP_MESSAGE DHCP_SERVER::parseDHCPRequest(DHCP_MESSAGE message) {
    // Simple check to make sure request is from a client
    if (message.op != DHCP_BOOTREQUEST) return createDHCPReply(DHCP_NAK, DHCP_CLIENT_ADDRESS, message.xid);
    int opt_index = 0;
    uint8_t opt_len = 0;
    uint8_t message_type = 0;
    IPAddress client_ip = {0, 0, 0, 0};
    // Parse the relevant DHCP options
    while (opt_index < DHCP_DEFAULT_OPTIONS_SIZE) {
        switch (message.options[opt_index]) {
        case DHCP_END:                              // End of the options list
            opt_index = DHCP_DEFAULT_OPTIONS_SIZE;
            break;
        case DHCP_PAD:                              // Has no data
            opt_index++;
            break;
        case DHCP_REQUESTED_IP:                     // Client requested IP Address
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            for (int i = 0; i < opt_len; i++) {
                client_ip[i] = message.options[opt_index + i];
            }
            opt_index += opt_len;
            break;
        case DHCP_IP_LEASE_TIME:                    // Client requested specific lease time
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_OPTION_OVERLOAD:                  // Client option overload of file or sname
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_MESSAGE_TYPE:                     // DHCP Message Type
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            message_type = message.options[opt_index];
            opt_index += opt_len;
            break;
        case DHCP_CLIENT_IDENTIFIER:                // Client provided unique identifier
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_VENDOR_CLASS_IDENTIFIER:          // Client provided class identifier
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_SERVER_IDENTIFIER:                // Client specified DHCP Server
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_PARAMETER_REQUEST_LIST:           // Client included a parameter list
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_MAX_MESSAGE_SIZE:                 // Client specified a maximum DHCP message size
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        case DHCP_MESSAGE_OPTION:                   // Client included a message
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        default:                                    // All other options are not relevant to clients
            opt_index++;
            opt_len = message.options[opt_index];
            opt_index++;
            opt_index += opt_len;
            break;
        }
    }
    // Send back the appropriate DHCP Reply
    switch (message_type) {
    case DHCP_DISCOVER:
        assignAddress(client_ip);
        return createDHCPReply(DHCP_OFFER, client_ip, message.xid);
    case DHCP_REQUEST:
        return createDHCPReply(DHCP_ACK, DHCP_CLIENT_ADDRESS, message.xid);
    case DHCP_DECLINE:
        return createDHCPReply(DHCP_NAK, DHCP_CLIENT_ADDRESS, message.xid);
    case DHCP_RELEASE:
        return createDHCPReply(DHCP_NAK, DHCP_CLIENT_ADDRESS, message.xid);
    default:
        return createDHCPReply(DHCP_NAK, DHCP_CLIENT_ADDRESS, message.xid);
    }
}

// Create DHCP Reply based on the received DHCP Request
DHCP_MESSAGE DHCP_SERVER::createDHCPReply(uint8_t message_type, IPAddress client_ip, uint32_t xid) {
    DHCP_MESSAGE reply;
    reply.op = DHCP_BOOTREPLY;
    reply.htype = DHCP_ETHERNET;
    reply.hlen = DHCP_MAC_ADDRESS_LENGTH;
    reply.hops = 0;
    reply.xid = xid;
    reply.secs = 0;
    reply.flags = DHCP_BROADCAST_FLAG;
    reply.ciaddr[0] = 0;
    reply.ciaddr[1] = 0;
    reply.ciaddr[2] = 0;
    reply.ciaddr[3] = 0;
    switch (message_type) {
    case DHCP_OFFER:
        break;
    case DHCP_ACK:
    
        break;
    case DHCP_NAK:
        break;
    default:
        break;
    }
    return reply;
}

// DHCP Server Check for Requests
uint8_t DHCP_SERVER::checkForRequests() {
    uint16_t packet_size = DHCP_SOCKET.parsePacket();
    uint8_t packet_buffer[DHCP_MESSAGE_SIZE];
    if (packet_size > 0) {
        DHCP_MESSAGE *request, reply;
        DHCP_SOCKET.read((unsigned char *)&packet_buffer, DHCP_MESSAGE_SIZE);
        if (_verbose) printRawUDPPayload(packet_buffer, packet_size);
        request = (DHCP_MESSAGE*)packet_buffer;
        if (_verbose) printDHCPMessage(*request);
        reply = parseDHCPRequest(*request);
        if (_verbose) printDHCPMessage(reply);
        DHCP_SOCKET.beginPacket(DHCP_BROADCAST, DHCP_CLIENT_PORT);
        DHCP_SOCKET.write((uint8_t *)&reply, sizeof(reply));
        DHCP_SOCKET.endPacket();
        delete request;
    }
}

// Print a DHCP Message
void DHCP_SERVER::printDHCPMessage(DHCP_MESSAGE message) {
    Serial.println("DHCP Message");
    Serial.print("    op: ");
    switch (message.op) {
    case DHCP_BOOTREQUEST:
        Serial.print("BOOTREQUEST");
        break;
    case DHCP_BOOTREPLY:
        Serial.print("BOOTREPLY");
        break;
    default:
        break;
    }
    Serial.println();
    Serial.print("    htype: ");
    switch (message.htype) {
    case DHCP_ETHERNET:
        Serial.print("Ethernet (10Mb)");
        break;
    case DHCP_EXPERIMENTAL:
        Serial.print("Experimental Ethernet (3Mb)");
        break;
    case DHCP_RADIO:
        Serial.print("Amateur Radio AX.25");
        break;
    case DHCP_PROTEON:
        Serial.print("Proteon ProNET Token Ring");
        break;
    case DHCP_CHAOS:
        Serial.print("Chaos");
        break;
    case DHCP_IEEE_802:
        Serial.print("IEEE 802 Network");
        break;
    case DHCP_ARCNET:
        Serial.print("ARCNET");
        break;
    case DHCP_HYPERCHANNEL:
        Serial.print("Hyperchannel");
        break;
    case DHCP_LANSTAR:
        Serial.print("Lanstar");
        break;
    case DHCP_AUTO_SHORT:
        Serial.print("Autonet Short Address");
        break;
    case DHCP_LOCAL_TALK:
        Serial.print("LocalTalk");
        break;
    case DHCP_LOCAL_NET:
        Serial.print("LocalNet");
        break;
    case DHCP_ULTRA_LINK:
        Serial.print("Ultra Link");
        break;
    case DHCP_SMDS:
        Serial.print("SMDS");
        break;
    case DHCP_FRAME_RELAY:
        Serial.print("Frame Relay");
        break;
    case DHCP_ATM_A:
        Serial.print("Asynchronous Transfer Mode(ATM)");
        break;
    case DHCP_HDLC:
        Serial.print("HDLC");
        break;
    case DHCP_FIBRE:
        Serial.print("Fibre Channel");
        break;
    case DHCP_ATM_B:
        Serial.print("Asynchronous Transfer Mode(ATM)");
        break;
    case DHCP_SERIAL:
        Serial.print("Serial Line");
        break;
    case DHCP_ATM_C:
        Serial.print("Asynchronous Transfer Mode(ATM)");
        break;
    case DHCP_MIL_STD_188_220:
        Serial.print("MIL-STD-188-220");
        break;
    default:
        break;
    }
    Serial.println();
    Serial.print("    hlen: ");
    Serial.print(message.hlen);
    Serial.println();
    Serial.print("    hops: ");
    Serial.print(message.hops);
    Serial.println();
    Serial.print("    xid: ");
    Serial.print(message.xid);
    Serial.println();
    Serial.print("    secs: ");
    Serial.print(message.secs);
    Serial.print(" s");
    Serial.println();
    Serial.print("    flags: ");
    Serial.print(message.flags, HEX);
    Serial.println();
    Serial.print("    ciaddr: ");
    for (int i = 0; i < 4; i++) {
        if (i > 0) Serial.print(".");
        Serial.print(message.ciaddr[i]);
    }
    Serial.println();
    Serial.print("    yiaddr: ");
    for (int i = 0; i < 4; i++) {
        if (i > 0) Serial.print(".");
        Serial.print(message.yiaddr[i]);
    }
    Serial.println();
    Serial.print("    siaddr: ");
    for (int i = 0; i < 4; i++) {
        if (i > 0) Serial.print(".");
        Serial.print(message.siaddr[i]);
    }
    Serial.println();
    Serial.print("    giaddr: ");
    for (int i = 0; i < 4; i++) {
        if (i > 0) Serial.print(".");
        Serial.print(message.giaddr[i]);
    }
    Serial.println();
    Serial.print("    chaddr: ");
    for (int i = 0; i < message.hlen; i++) {
        if (i > 0) Serial.print(":");
        if (message.chaddr[i] < 16) Serial.print("0");
        Serial.print(message.chaddr[i], HEX);
    }
    Serial.println();
    Serial.print("    sname: ");
    for (int i = 0; i < 64; i++) {
        Serial.print(message.sname[i]);
    }
    Serial.println();
    Serial.print("    bootf: ");
    for (int i = 0; i < 128; i++) {
        Serial.print(message.bootf[i]);
    }
    Serial.println();
    Serial.print("    magic: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(message.magic[i]);
    }
    Serial.println();
    Serial.println("    options: ");
    Serial.println("        Type  Len  Data");
    for (int i = 0; i < DHCP_DEFAULT_OPTIONS_SIZE; i++) {
        Serial.print("        ");
        Serial.print(message.options[i]);
        if (message.options[i] == DHCP_END) {
            i = DHCP_DEFAULT_OPTIONS_SIZE;
            break;
        }
        if (message.options[i] < 10) Serial.print(" ");
        Serial.print("    ");
        i++;
        Serial.print(message.options[i]);
        if (message.options[i+1] < 10) Serial.print(" ");
        Serial.print("   ");
        for (int j = 1; j <= message.options[i]; j++) {
            Serial.print(message.options[i+j]);
        }
        Serial.println();
        i += message.options[i];
    }
    Serial.println();
}

// Print a raw UDP packet
void DHCP_SERVER::printRawUDPPayload(uint8_t *packet_buffer, uint16_t packet_size) {
    Serial.println("Raw DHCP UDP Payload");
    Serial.println();
    int i = 0, row = 0;
    while (i < packet_size) {
        if (row < 1) Serial.print("0");
        if (row < 16) Serial.print("0");
        if (row < 256) Serial.print("0");
        Serial.print((row*16), HEX);
        Serial.print("  ");
        if ((packet_size - i) > 16) {
            for (int j = 0; j < 16; j++) {
                if (j == 8) Serial.print(" ");
                if (j > 0) Serial.print(" ");
                if (packet_buffer[i+j] < 16) Serial.print("0");
                Serial.print(packet_buffer[i+j], HEX);
            }
            Serial.println();
            i += 16;
        } else {
            int r = (packet_size - i);
            for (int j = 0; j < r; j++) {
                if (j == 8) Serial.print(" ");
                if (j > 0) Serial.print(" ");
                if (packet_buffer[i+j] < 16) Serial.print("0");
                Serial.print(packet_buffer[i+j], HEX);
            }
            Serial.println();
            i += r;
        }
        row++;
    }
    Serial.println();
}

// ********** DHCP CLIENT **********
// TODO: Fully Implement this class

DHCP_CLIENT::DHCP_CLIENT() {
    for (int i = 0; i < 16; i++) {
        if (i < 6) {
            H_ADDRESS[i] = random(0, 256);
        } else {
            H_ADDRESS[i] = 0;
        }
    }
}

DHCP_CLIENT::DHCP_CLIENT(uint8_t hlen) {
    for (int i = 0; i < 16; i++) {
        if (i < hlen) {
            H_ADDRESS[i] = random(0, 256);
        } else {
            H_ADDRESS[i] = 0;
        }
    }
}

DHCP_CLIENT::DHCP_CLIENT(uint8_t chaddr[]) {
    for (int i = 0; i < 16; i++) {
        if (i < 6) {
            H_ADDRESS[i] = chaddr[i];
        } else {
            H_ADDRESS[i] = 0;
        }
    }
}

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

// ********** DHCP UNIT TESTER **********
// TODO: Fully Implement this class

// Constructor
DHCP_TESTER::DHCP_TESTER() {
    randomSeed(1111);
    test_xid = (uint32_t)random(2147483647);
    test_server_ip = IPAddress(10,0,0,1);
    uint8_t test_client_mac[] = {0x01, 0xEA, 0xFD, 0xBC, 0x05, 0x06};
    _dhcp_server = new DHCP_SERVER(IPAddress(10, 0, 0, 1), 32);
    _dhcp_client = new DHCP_CLIENT(test_client_mac, 6);
}

// Destructor
DHCP_TESTER::~DHCP_TESTER() {
    Serial.println("Destroying DHCP Tester");
    delete _dhcp_server;
    delete _dhcp_client;
}

// Handle failed tests
bool DHCP_TESTER::testFailed() {
    Serial.print("[ FAIL ]");
    Serial.println();
    return false;
}

// Handle successful tests
bool DHCP_TESTER::testPassed() {
    Serial.print("[  OK  ]");
    Serial.println();
    return true;
}

// Run SimpleDHCP tests
bool DHCP_TESTER::runTests() {
    Serial.println("************ Running tests ************");
    bool results = true;
    if (!runServerTests()) results = false;
    if (!runClientTests()) results = false;
    Serial.println("************ Test Results *************");
    Serial.print("Final result:    ");
    if (results) {
        return testPassed();
    } else {
        return testFailed();
    }
}

// Run DHCP Server tests
bool DHCP_TESTER::runServerTests() {
    Serial.println("********** DHCP Server Tests **********");
    bool results = true;
    if (!runServerAddressManagementTests()) results = false;
    if (!runServerMessageGenerationTests()) results = false;
    if (!runServerDebugTests()) results = false;
    if (!runServerParsingTests()) results = false;
    return results;
}

// Run Server address management tests
bool DHCP_TESTER::runServerAddressManagementTests() {
    Serial.println("       Address Management Tests        ");
    bool results = true;
    if (!testAddressInRange()) results = false;
    if (!testAddressOutOfRange()) results = false;
    if (!testAutoAddressAssignment()) results = false;
    if (!testAddressRelease()) results = false;
    if (!testAddressReassignment()) results = false;
    return results;
}

// Test to ensure requested addresses that are withing the range of the address pool are assigned
bool DHCP_TESTER::testAddressInRange() {
    Serial.print("In Range:        ");
    IPAddress address = _dhcp_server->assignAddress(IPAddress(10, 0, 0, 2));
    if (address != IPAddress(10, 0, 0, 2)) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Test to ensure that out of range address requests are given an auto-assigned address instead
bool DHCP_TESTER::testAddressOutOfRange() {
    Serial.print("Out of Range:    ");
    IPAddress address = _dhcp_server->assignAddress(IPAddress(192, 168, 1, 353));
    if (address != IPAddress(10, 0, 0, 3)) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Test to ensure addresses are correctly auto-assigned
bool DHCP_TESTER::testAutoAddressAssignment() {
    Serial.print("Auto Assign:     ");
    IPAddress address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 4)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 5)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 6)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 7)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 8)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 9)) return testFailed();
    address = _dhcp_server->assignAddress(IPAddress(0, 0, 0, 0));
    if (address != IPAddress(10, 0, 0, 10)) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Test to make sure that addresses are correctly released
bool DHCP_TESTER::testAddressRelease() {
    Serial.print("Release:         ");
    _dhcp_server->releaseAddress(IPAddress(10, 0, 0, 4));
    if(!_dhcp_server->_addresses[2]) return testFailed();
    _dhcp_server->releaseAddress(IPAddress(10, 0, 0, 6));
    if(!_dhcp_server->_addresses[4]) return testFailed();
    _dhcp_server->releaseAddress(IPAddress(10, 0, 0, 8));
    if(!_dhcp_server->_addresses[6]) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Test reassignment of released addresses
bool DHCP_TESTER::testAddressReassignment() {
    Serial.print("Reassignment:    ");
    IPAddress address = _dhcp_server->assignAddress(address);
    if (address != IPAddress(10, 0, 0, 4)) return testFailed();
    address = _dhcp_server->assignAddress(address);
    if (address != IPAddress(10, 0, 0, 6)) return testFailed();
    address = _dhcp_server->assignAddress(address);
    if (address != IPAddress(10, 0, 0, 8)) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server message generation tests
bool DHCP_TESTER::runServerMessageGenerationTests() {
    Serial.println("    Server Message Generation Tests    ");
    bool results = true;
    if (!testDHCPOFFERGeneration()) results = false;
    if (!testDHCPACKGeneration()) results = false;
    if (!testDHCPNAKGeneration()) results = false;
    return results;
}

// Run Server DHCP OFFER generation test
bool DHCP_TESTER::testDHCPOFFERGeneration() {
    Serial.print("DHCP OFFER:      ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP ACK generation test
bool DHCP_TESTER::testDHCPACKGeneration() {
    Serial.print("DHCP ACK:        ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP NAK generation test
bool DHCP_TESTER::testDHCPNAKGeneration() {
    Serial.print("DHCP NAK:        ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

//
bool DHCP_TESTER::runServerDebugTests() {
    Serial.println("          Server Debug Tests           ");
    bool results = true;
    if (!testServerMessagePrinter()) results = false;
    if (!testServerPacketPrinter()) results = false;
    return results;
}

//
bool DHCP_TESTER::testServerMessagePrinter() {
    Serial.print("DHCP Message:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

//
bool DHCP_TESTER::testServerPacketPrinter() {
    Serial.print("Raw UDP Packet:  ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server parsing tests
bool DHCP_TESTER::runServerParsingTests() {
    Serial.println("     Server Message Parsing Tests      ");
    bool results = true;
    if (!testDHCPDISCOVERParsing()) results = false;
    if (!testDHCPINFORMParsing()) results = false;
    if (!testDHCPREQUESTParsing()) results = false;
    if (!testDHCPDECLINEParsing()) results = false;
    if (!testDHCPRELEASEParsing()) results = false;
    return results;
}

// Run Server DHCP DISCOVER parsing test
bool DHCP_TESTER::testDHCPDISCOVERParsing() {
    Serial.print("DHCP DISCOVER:   ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP INFORM parsing test
bool DHCP_TESTER::testDHCPINFORMParsing() {
    Serial.print("DHCP INFORM:     ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP REQUEST parsing test
bool DHCP_TESTER::testDHCPREQUESTParsing() {
    Serial.print("DHCP REQUEST:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP DECLINE parsing test
bool DHCP_TESTER::testDHCPDECLINEParsing() {
    Serial.print("DHCP DECLINE:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Server DHCP RELEASE parsing test
bool DHCP_TESTER::testDHCPRELEASEParsing() {
    Serial.print("DHCP RELEASE:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client tests
bool DHCP_TESTER::runClientTests() {
    Serial.println("********** DHCP Client Tests **********");
    bool results = true;
    if (!runClientMessageGenerationTests()) results = false;
    if (!runClientParsingTests()) results = false;
    return results;
}

// Run Client message generation tests
bool DHCP_TESTER::runClientMessageGenerationTests() {
    Serial.println("    Client Message Generation Tests    ");
    bool results = true;
    if (!testDHCPDISCOVERGeneration()) results = false;
    if (!testDHCPINFORMGeneration()) results = false;
    if (!testDHCPREQUESTGeneration()) results = false;
    if (!testDHCPDECLINEGeneration()) results = false;
    if (!testDHCPRELEASEGeneration()) results = false;
    return results;
}

// Run Client DHCP DISCOVER generation test
bool DHCP_TESTER::testDHCPDISCOVERGeneration() {
    Serial.print("DHCP DISCOVER:   ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP INFORM generation test
bool DHCP_TESTER::testDHCPINFORMGeneration() {
    Serial.print("DHCP INFORM:     ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP REQUEST generation test
bool DHCP_TESTER::testDHCPREQUESTGeneration() {
    Serial.print("DHCP REQUEST:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP DECLINE generation test
bool DHCP_TESTER::testDHCPDECLINEGeneration() {
    Serial.print("DHCP DECLINE:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP RELEASE generation test
bool DHCP_TESTER::testDHCPRELEASEGeneration() {
    Serial.print("DHCP RELEASE:    ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client parsing tests
bool DHCP_TESTER::runClientParsingTests() {
    Serial.println("     Client Message Parsing Tests      ");
    bool results = true;
    if (!testDHCPOFFERParsing()) results = false;
    if (!testDHCPACKParsing()) results = false;
    if (!testDHCPNAKParsing()) results = false;
    return results;
}

// Run Client DHCP OFFER parsing test
bool DHCP_TESTER::testDHCPOFFERParsing() {
    Serial.print("DHCP OFFER:      ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP ACK parsing test
bool DHCP_TESTER::testDHCPACKParsing() {
    Serial.print("DHCP ACK:        ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}

// Run Client DHCP NAK parsing test
bool DHCP_TESTER::testDHCPNAKParsing() {
    Serial.print("DHCP NAK:        ");
    if (true) return testFailed();
    return testPassed(); // If we reached here then all the tests passed
}