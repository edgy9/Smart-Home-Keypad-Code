/****************************************************************************************************************************
  defines.h
  
  Ethernet_Generic is a library for the W5x00 Ethernet shields trying to merge the good features of
  previous Ethernet libraries
  
  Built by Khoi Hoang https://github.com/khoih-prog/Ethernet_Generic
 ***************************************************************************************************************************************/

#ifndef defines_h
#define defines_h


#define SerialDebug   Serial


#define DEBUG_ETHERNET_GENERIC_PORT         SerialDebug

// Debug Level from 0 to 4
#define _ETG_LOGLEVEL_                      3

// Default to use W5100. Must change to false for W5500, W5100S, for faster SPI clock
#define USE_W5100                           false



#if ( defined(ESP32) )
  // For ESP32
  #warning Use ESP32 architecture
  #define ETHERNET_USE_ESP32
  #define BOARD_TYPE      "ESP32"
  
  #define W5500_RST_PORT   26
#endif

#ifndef BOARD_NAME
  #define BOARD_NAME    BOARD_TYPE
#endif

#include <SPI.h>

///////////////////////////////////////////////////////////

// W5100 chips can have up to 4 sockets.  W5200 & W5500 can have up to 8 sockets.
// Use EthernetLarge feature, Larger buffers, but reduced number of simultaneous connections/sockets (MAX_SOCK_NUM == 2)
#define ETHERNET_LARGE_BUFFERS

//////////////////////////////////////////////////////////

// For boards supporting multiple hardware / software SPI buses, such as STM32
// Tested OK with Nucleo144 F767ZI and L552ZE_Q so far
#if ( defined(STM32F0) || defined(STM32F1) || defined(STM32F2) || defined(STM32F3)  ||defined(STM32F4) || defined(STM32F7) || \
       defined(STM32L0) || defined(STM32L1) || defined(STM32L4) || defined(STM32H7)  ||defined(STM32G0) || defined(STM32G4) || \
       defined(STM32WB) || defined(STM32MP1) || defined(STM32L5) )
       
  // Be sure to use true only if necessary for your board, or compile error
  #define USING_CUSTOM_SPI            true

  #if ( USING_CUSTOM_SPI )
    // Currently test OK for F767ZI and L552ZE_Q
    #define USING_SPI2                  true
  
    #if (USING_SPI2)
      //#include <SPI.h>
      // For L552ZE-Q, F767ZI, but you can change the pins for any other boards
      // SCK: 23,  MOSI: 22, MISO: 25, SS/CS: 24 for SPI1
      #define CUR_PIN_MISO              25
      #define CUR_PIN_MOSI              22
      #define CUR_PIN_SCK               23
      #define CUR_PIN_SS                24
  
      #define SPI_NEW_INITIALIZED       true
  
      // Don't create the instance with CUR_PIN_SS, or Ethernet not working
      // To change for other boards' SPI libraries
      SPIClass SPI_New(CUR_PIN_MOSI, CUR_PIN_MISO, CUR_PIN_SCK);
      
      //#warning Using USE_THIS_SS_PIN = CUR_PIN_SS = 24
  
      #if defined(USE_THIS_SS_PIN)
        #undef USE_THIS_SS_PIN
      #endif   
      #define USE_THIS_SS_PIN       CUR_PIN_SS    //24
      
    #endif
    
  #endif

#endif

//////////////////////////////////////////////////////////

#include "Ethernet_Generic.h"

#if defined(ETHERNET_LARGE_BUFFERS)
  #define SHIELD_TYPE           "W5x00 using Ethernet_Generic Library with Large Buffer"
#else
  #define SHIELD_TYPE           "W5x00 using Ethernet_Generic Library"
#endif

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

// Select the IP address according to your local network
IPAddress ip(192, 168, 0, 220);

// Google DNS Server IP
IPAddress myDns(8, 8, 8, 8);

#endif    //defines_h
