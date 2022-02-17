

#ifndef API_h
#define API_h



#include "Arduino.h"
#include <aWOT.h>
#include <Ethernet.h>

EthernetClient client;
class API
{
  public:
    API();
    void checkEthernet(EthernetClient * client);
     //void checkEthernet();
    void begine();
  private:
    Application _app;
    Router _api;

    void _initialiseApi(void);
    
};


#endif
