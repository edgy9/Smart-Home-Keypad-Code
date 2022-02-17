

#ifndef API_h
#define API_h



#include "Arduino.h"
#include <aWOT.h>

class API
{
  public:
    API();
    void checkEthernet(EthernetClient * client);
    void begine();
  private:
    Application _app;
    Router _api;

    void _initialiseApi(void);
    
};


#endif
