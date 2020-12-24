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

// ********** DHCP SERVER **********

// DHCP_SERVER Default constructor, this constructor should be avoided
DHCP_SERVER::DHCP_SERVER() {
    if (_verbose) Serial.println("DHCP UDP Socket opened");
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
}

// DHCP_SERVER Intended Constructor, sets the address pool and server IP
DHCP_SERVER::DHCP_SERVER(IPAddress server_address, uint8_t range) {
    SERVER_ADDRESS = server_address;
    address_pool.firstOctet = server_address[0];
    address_pool.secondOctet = server_address[1];
    address_pool.thirdOctet = server_address[2];
    address_pool.range = range;
    _verbose = false;
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
}

// DHCP_SERVER Intended Constructor, sets the address pool, server IP, and verbosity
DHCP_SERVER::DHCP_SERVER(IPAddress server_address, uint8_t range, bool verbose) {
    SERVER_ADDRESS = server_address;
    address_pool.firstOctet = server_address[0];
    address_pool.secondOctet = server_address[1];
    address_pool.thirdOctet = server_address[2];
    address_pool.range = range;
    _verbose = verbose;
    DHCP_SOCKET.begin(DHCP_SERVER_PORT);
    if (_verbose) Serial.println("DHCP UDP Socket opened");
}

// DHCP Server Destructor
DHCP_SERVER::~DHCP_SERVER() {
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

// Set the DHCP Address Pool
void DHCP_SERVER::assignAddressPool(IPAddress server_address, uint8_t address_range) {
    address_pool.firstOctet  = server_address[0];
    address_pool.secondOctet = server_address[1];
    address_pool.thirdOctet  = server_address[2];
    address_pool.range = address_range;
}

// Assign network address from available addresses in the pool
IPAddress DHCP_SERVER::assignAddress(IPAddress requested_ip) {
    if (isAddressAvailable(requested_ip)) {
        return requested_ip;
    } else {
        return getAddressFromPool();
    }
}

// Get a valid network address from the address pool
IPAddress DHCP_SERVER::getAddressFromPool() {
    IPAddress address;
    return address;
}

// Check if a network address is in the pool and available
bool DHCP_SERVER::isAddressAvailable(IPAddress) {
    return false;
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
    case DHCP_IEEE_802:
        Serial.print("IEEE 802 Network");
        break;
    case DHCP_ARCNET:
        Serial.print("ARCNET");
        break;
    case DHCP_LOCAL_TALK:
        Serial.print("LocalTalk");
        break;
    case DHCP_LOCAL_NET:
        Serial.print("LocalNet");
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
    Serial.print(message.flags);
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