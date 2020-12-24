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

// ********** Required Libraries **********

#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUDP.h>

// ********** Definitions **********

// Library Specific Definitions
#define SIMPLE_DHCP_VERSION                 (0x000004)

// DHCP Standard Definitions

// DHCP Message limits
#define DHCP_MESSAGE_SIZE                   576                     // DHCP Minimum message size
#define DHCP_DEFAULT_OPTIONS_SIZE           344                     // DHCP Default Options size

// DHCP Ports
#define DHCP_SERVER_PORT                    67                      // Port for DHCP server to listen on
#define DHCP_CLIENT_PORT                    68                      // Port for client to listen on for DHCP

// DHCP OP Codes
#define DHCP_BOOTREQUEST                    1                       // DHCP request OP code
#define DHCP_BOOTREPLY                      2                       // DHCP reply OP code

// DHCP Message types
#define	DHCP_DISCOVER                       1                       // DHCP Discover message
#define DHCP_OFFER                          2                       // DHCP Offer message
#define	DHCP_REQUEST                        3                       // DHCP Request message
#define	DHCP_DECLINE                        4                       // DHCP Decline message
#define	DHCP_ACK                            5                       // DHCP Ack message
#define DHCP_NAK                            6                       // DHCP Nak message
#define	DHCP_RELEASE                        7                       // DHCP Release message
#define DHCP_INFORM                         8                       // DHCP Inform message

// DHCP HType Codes
#define DHCP_ETHERNET                       1                       // DHCP Ethernet (10 Mb)
#define DHCP_IEEE_802                       6                       // DHCP IEEE 802 Network
#define DHCP_ARCNET                         7                       // DHCP ARCNET
#define DHCP_LOCAL_TALK                     11                      // DHCP LocalTalk
#define DHCP_LOCAL_NET                      12                      // DHCP LocalNet
#define DHCP_SMDS                           14                      // DHCP SMDS
#define DHCP_FRAME_RELAY                    15                      // DHCP Frame Relay
#define DHCP_ATM_A                          16                      // DHCP Asynchronous Transfer Mode(ATM)
#define DHCP_HDLC                           17                      // DHCP HDLC
#define DHCP_FIBRE                          18                      // DHCP Fibre Channel
#define DHCP_ATM_B                          19                      // DHCP Asynchronous Transfer Mode(ATM)
#define DHCP_SERIAL                         20                      // DHCP Serial Line

// DHCP HLen Codes
#define DHCP_MAC_ADDRESS_LENGTH             6                       // DHCP Hardware Address length for networks using IEEE 802 MAC addresses

// DHCP Special Codes
#define DHCP_BROADCAST_FLAG                 (0x8000)                // DHCP Broadcast Flag
#define DHCP_MAGIC_COOKIE                   (0x63825363)            // DHCP Magic Cookie

// DHCP Lease Parameters
#define DHCP_DEFAULT_MAX_LEASES             16                      // DHCP Default Maximum Leases
#define DHCP_DEFAULT_LEASE_TIME             ((long)60*60*24)        // DHCP Default Lease Time

// DHCP Options
// RFC 1497 Vendor Extensions
#define DHCP_PAD                            0                       // DHCP Pad Option
#define DHCP_SUBNET_MASK                    1                       // DHCP Subnet Mask Option
#define DHCP_TIME_OFFSET                    2                       // DHCP Time Offset Option
#define DHCP_ROUTER                         3                       // DHCP Router Option
#define DHCP_TIME_SERVER                    4                       // DHCP Time Server Option
#define DHCP_IEN_116_NAME_SERVER            5                       // DHCP IEN-116 Name Server Option
#define DHCP_DNS_NAME_SERVER                6                       // DHCP DNS Name Server Option
#define DHCP_LOG_SERVER                     7                       // DHCP Log Server Option
#define DHCP_COOKIE_SERVER                  8                       // DHCP Cookie Server Option
#define DHCP_LRP_SERVER                     9                       // DHCP LRP Server Option
#define DHCP_IMPRESS_SERVER                 10                      // DHCP Impress Server Option
#define DHCP_RESOURCE_LOCATION_SERVER       11                      // DHCP Resource Location Server Option
#define DHCP_HOST_NAME                      12                      // DHCP Host Name Option
#define DHCP_BOOT_FILE_SIZE                 13                      // DHCP Boot File Size Option
#define DHCP_MERIT_DUMP_FILE                14                      // DHCP Merit Dump File Option
#define DHCP_DOMAIN_NAME                    15                      // DHCP Domain Name Option
#define DHCP_SWAP_SERVER                    16                      // DHCP Swap Server Option
#define DHCP_ROOT_PATH                      17                      // DHCP Root Path Option
#define DHCP_EXTENSIONS_PATH                18                      // DHCP Extensions Path Option
#define DHCP_END                            255                     // DHCP End Option - placed after all other options to mark the end of the options list
// IP Layer Parameters Per Host
#define DHCP_IP_FORWARDING                  19                      // DHCP IP Forwarding Option
#define DHCP_SOURCE_ROUTING                 20                      // DHCP Non-Local Source Routing Option
#define DHCP_POLICY_FILTER                  21                      // DHCP Policy Filter Option
#define DHCP_MAX_DATAGRAM_SIZE              22                      // DHCP Maximum Datagram Reassembly Size Option
#define DHCP_DEFAULT_TIME_TO_LIVE           23                      // DHCP Default IP Time-To-Live Option
#define DHCP_MTU_AGING_TIMEOUT              24                      // DHCP Path MTU Aging Timeout Option
#define DHCP_MTU_PLATEAU_TABLE              25                      // DHCP Path MTU Plateau Table Option
//IP Layer Parameters Per Interface
#define DHCP_INTERFACE_MTU                  26                      // DHCP Interface MTU Option
#define DHCP_ALL_SUBNETS_ARE_LOCAL          27                      // DHCP All Subnets Are Local Option
#define DHCP_BROADCAST_ADDRESS              28                      // DHCP Broadcast Address Option
#define DHCP_PERFORM_MASK_DISCOVERY         29                      // DHCP Perform Mask Discovery Option
#define DHCP_MASK_SUPPLIER                  30                      // DHCP Mask Supplier Option
#define DHCP_PERFORM_ROUTER_DISCOVERY       31                      // DHCP Perform Router Discovery Option
#define DHCP_ROUTER_SOLICITATION_ADDRESS    32                      // DHCP Router Solicitation Address Option
#define DHCP_STATIC_ROUTE                   33                      // DHCP Static Route Option
// Link Layer Parameters Per Interface
#define DHCP_TRAILER_ENCAPSULATION          34                      // DHCP Trailer Encapsulation Option
#define DHCP_ARP_CACHE_TIMEOUT              35                      // DHCP ARP Cache Timeout Option
#define DHCP_ETHERNET_ENCAPSULATION         36                      // DHCP Ethernet Encapsulation Option
// TCP Parameters
#define DHCP_DEFAULT_TTL                    37                      // DHCP Default TTL Option
#define DHCP_TCP_KEEPALIVE_INTERVAL         38                      // DHCP TCP Keepalive Interval Option
#define DHCP_TCP_KEEPALIVE_GARBAGE          39                      // DHCP TCP Keepalive Garbage Option
// Application and Service Parameters
#define DHCP_NIS_DOMAIN                     40                      // DHCP Network Information Service Domain Option
#define DHCP_NIS_SERVERS                    41                      // DHCP Network Information Service Servers Option
#define DHCP_NTP_SERVERS                    42                      // DHCP Network Time Protocol Servers Option
#define DHCP_VENDOR_INFO                    43                      // DHCP Vendor Specific Information Option
#define DHCP_NETBIOS_NAME_SERVERS           44                      // DHCP NetBIOS Over TCP/IP Name Servers Option
#define DHCP_NETBIOS_DIST_SERVERS           45                      // DHCP NetBIOS Over TCP/IP Datagram Distibution Servers Option
#define DHCP_NETBIOS_NODE_TYPE              46                      // DHCP NetBIOS Over TCP/IP Node Type Option
#define DHCP_NETBIOS_SCOPE                  47                      // DHCP NetBIOS Over TCP/IP Scope Option
#define DHCP_X_FONT_SERVERS                 48                      // DHCP X Window System Font Servers Option
#define DHCP_X_DISPLAY_MANAGER              49                      // DHCP X Window System Display Manager Option
#define DHCP_NISP_DOMAIN                    64                      // DHCP Network Information Service+ Domain Option
#define DHCP_NISP_SERVERS                   65                      // DHCP Network Information Service+ Servers Option
#define DHCP_MOBILE_IP_HOME_AGENT           68                      // DHCP Mobile IP Home Agent Option
#define DHCP_SMTP_SERVERS                   69                      // DHCP Simple Mail Transport Protocol(SMTP) Servers Option
#define DHCP_POP3_SERVERS                   70                      // DHCP Post Office Protocol(POP3) Servers Option
#define DHCP_NNTP_SERVERS                   71                      // DHCP Network News Transfer Protocol(NNTP) Servers Option
#define DHCP_WWW_SERVERS                    72                      // DHCP Default World Wide Web(WWW) Servers Option
#define DHCP_FINGER_SERVERS                 73                      // DHCP Default Finger Servers Option
#define DHCP_IRC_SERVERS                    74                      // DHCP Default Internet Relay Chat(IRC) Servers Option
#define DHCP_STREET_TALK_SERVERS            75                      // DHCP StreetTalk Servers Option
#define DHCP_STDA_SERVERS                   76                      // DHCP StreetTalk Directory Assistance(STDA) Servers Option
// DHCP Extensions
#define DHCP_REQUESTED_IP                   50                      // DHCP Requested IP Address Option
#define DHCP_IP_LEASE_TIME                  51                      // DHCP IP Address Lease Time Option
#define DHCP_OPTION_OVERLOAD                52                      // DHCP Option Overload Option
#define DHCP_OVERLOAD_FILE                  1                       // DHCP File Overload, Requires the DHCP_OPTION_OVERLOAD Option
#define DHCP_OVERLOAD_SNAME                 2                       // DHCP SName Overload, Requires the DHCP_OPTION_OVERLOAD Option
#define DHCP_OVERLOAD_BOTH                  3                       // DHCP File and SName Overload, Requires the DHCP_OPTION_OVERLOAD Option
#define DHCP_MESSAGE_TYPE                   53                      // DHCP DHCP Message Type Option
#define DHCP_SERVER_IDENTIFIER              54                      // DHCP Server Identifier Option
#define DHCP_PARAMETER_REQUEST_LIST         55                      // DHCP Parameter Request List Option
#define DHCP_MESSAGE_OPTION                 56                      // DHCP Message Option
#define DHCP_MAX_MESSAGE_SIZE               57                      // DHCP Maximum DHCP Message Size Option
#define DHCP_RENEWAL_TIME_VALUE             58                      // DHCP Renewal(T1) Time Value Option
#define DHCP_REBINDING_TIME_VALUE           59                      // DHCP Rebinding(T2) Time Value Option
#define DHCP_VENDOR_CLASS_IDENTIFIER        60                      // DHCP Vendor Class Identifier Option
#define DHCP_CLIENT_IDENTIFIER              61                      // DHCP Client Identifier Option
#define DHCP_TFTP_SERVER_NAME               66                      // DHCP TFTP Server Name Option
#define DHCP_BOOTFILE_NAME                  67                      // DHCP Bootfile Name Option

// ********** Constants **********

// DHCP Addresses
const IPAddress DHCP_BROADCAST = {255, 255, 255, 255};              // DHCP Broadcast Address
const IPAddress DHCP_CLIENT_ADDRESS = {0, 0, 0, 0};                 // DHCP Default Client Address

// ********** Structures **********

// DHCP Message Structure Without Options
typedef struct DHCP_MESSAGE {
    uint8_t     op;                                                 // Operation Code
    uint8_t     htype;                                              // Hardware Type
    uint8_t     hlen;                                               // Hardware Address Length
    uint8_t     hops;                                               // Hops
    uint32_t    xid;                                                // Transaction Identifier
    uint16_t    secs;                                               // Seconds
    uint16_t    flags;                                              // Flags
    uint8_t     ciaddr[4];                                          // Client IP Address
    uint8_t     yiaddr[4];                                          // "Your" IP Address
    uint8_t     siaddr[4];                                          // Server IP Address
    uint8_t     giaddr[4];                                          // Gateway IP Address
    uint8_t     chaddr[16];                                         // Client Hardware Address
    uint8_t     sname[64];                                          // Server Name
    uint8_t     bootf[128];                                         // Boot Filename
    uint8_t     magic[4];                                           // Magic Cookie
    uint8_t     options[DHCP_DEFAULT_OPTIONS_SIZE];                 // DHCP Options - Assumes max DHCP message is DHCP_MESSAGE_SIZE (576) - TODO: Implement dynamic options size
} DHCP_MESSAGE;

// DHCP Lease Structure
typedef struct DHCP_LEASE {
    byte            status;                                         // Lease Status
    long            expires;                                        // Expiry Time
    unsigned long   mac_crc;                                        // MAC Cyclic Redundancy Check
    unsigned long   host_crc;                                       // Host Cyclic Redundancy Check
} DHCP_LEASE;

// DHCP Address Pool Structure
typedef struct DHCP_ADDRESS_POOL {
    uint8_t firstOctet;
    uint8_t secondOctet;
    uint8_t thirdOctet;
    uint8_t range;
} DHCP_ADDRESS_POOL;

// ********** Classes **********

// DHCP Server Class
class DHCP_SERVER {
    friend class DHCP_TESTER;
private:
    // Members
    bool _verbose;
    EthernetUDP DHCP_SOCKET;                                        // DHCP Server UDP Socket
    IPAddress SERVER_ADDRESS;                                       // DHCP Server Network Address
    DHCP_ADDRESS_POOL address_pool;                                 // DHCP Server Address Pool
    // Methods
    IPAddress getAddressFromPool();                                 // DHCP Server Get Network Address from pool
    bool isAddressAvailable(IPAddress);                             // DHCP Server check if network address is valid and available
    IPAddress assignAddress(IPAddress);                             // DHCP Server Assign Network Address
    void printDHCPMessage(DHCP_MESSAGE);                            // DHCP Server Print the raw DHCP message
    void printRawUDPPayload(uint8_t *, uint16_t);                   // DHCP Server Print the raw UDP payload
    DHCP_MESSAGE parseDHCPRequest(DHCP_MESSAGE);                    // DHCP Server Request Parser
    DHCP_MESSAGE createDHCPReply(uint8_t, IPAddress, uint32_t);     // DHCP Server Create Reply to Request
public:
    // Constructors
    DHCP_SERVER();                                                  // DHCP Server Default Constructor, this constructor should be avoided
    DHCP_SERVER(IPAddress, uint8_t);                                // DHCP Server Intended Constructor
    DHCP_SERVER(IPAddress, uint8_t, bool);                          // DHCP Server Intended Constructor with verbosity option
    // Destructor
    ~DHCP_SERVER();                                                 // DHCP Server Destructor
    // Public methods
    bool getVerbosity();                                            // DHCP Server get current verbosity
    bool setVerbosity(bool);                                        // DHCP Server set verbosity
    uint8_t checkForRequests();                                     // DHCP Server Check for requests
    void assignAddressPool(IPAddress, uint8_t);                     // DHCP Server Assign Address Pool range
};

// DHCP Client Class - TODO: Implement
class DHCP_CLIENT {
    friend class DHCP_TESTER;
private:
    uint8_t H_ADDRESS[16];
    DHCP_MESSAGE createDHCPMessage(uint8_t, uint32_t);
    DHCP_MESSAGE createDHCPMessage(uint8_t, uint32_t, IPAddress);
public:
    DHCP_CLIENT();
    DHCP_CLIENT(uint8_t [], uint8_t);
    ~DHCP_CLIENT();
};

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