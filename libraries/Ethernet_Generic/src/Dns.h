/****************************************************************************************************************************
  Dns.h

  Ethernet_Generic is a library for the W5x00 Ethernet shields trying to merge the good features of
  previous Ethernet libraries
  
  Based on and modified from 
  
  1) Ethernet Library         https://github.com/arduino-libraries/Ethernet
  2) EthernetLarge Library    https://github.com/OPEnSLab-OSU/EthernetLarge
  3) Ethernet2 Library        https://github.com/adafruit/Ethernet2
  4) Ethernet3 Library        https://github.com/sstaub/Ethernet3
    
  Built by Khoi Hoang https://github.com/khoih-prog/EthernetWebServer
  
  Version: 2.6.0
    
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  2.0.0   K Hoang      31/03/2022 Initial porting and coding to support SPI2, debug, h-only library
  2.0.1   K Hoang      08/04/2022 Add support to SPI1 for RP2040 using arduino-pico core
  2.1.0   K Hoang      22/04/2022 Add support to WIZNet W5100S
  2.2.0   K Hoang      02/05/2022 Add support to custom SPI for any board, such as STM32
  2.3.0   K Hoang      03/05/2022 Add support to custom SPI for RP2040, Portenta_H7, etc. using Arduino-mbed core
  2.3.1   K Hoang      21/05/2022 Add setHostname() and related functions
  2.4.0   K Hoang      31/07/2022 Using raw_address() as default instead of private IPAddress data
  2.4.1   K Hoang      25/08/2022 Auto-select SPI SS/CS pin according to board package
  2.5.0   K Hoang      26/08/2022 Using raw_address() as default only for arduino-pico for compatibility
  2.5.1   K Hoang      01/09/2022 Slow SPI clock for old W5100 shield using SAMD Zero
  2.5.2   K Hoang      06/09/2022 Slow SPI clock only when necessary. Improve support for SAMD21
  2.6.0   K Hoang      11/09/2022 Add support to AVR Dx (AVR128Dx, AVR64Dx, AVR32Dx, etc.) using DxCore
 *****************************************************************************************************************************/
 
// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef DNSClient_h
#define DNSClient_h

#include "Ethernet_Generic.hpp"

class DNSClient
{
  public:
    void begin(const IPAddress& aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int inet_aton(const char *aIPAddrString, IPAddress& aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int getHostByName(const char* aHostname, IPAddress& aResult, uint16_t timeout = 5000);

  protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, IPAddress& aAddress);

    IPAddress   iDNSServer;
    uint16_t    iRequestId;
    EthernetUDP iUdp;
};

#endif
