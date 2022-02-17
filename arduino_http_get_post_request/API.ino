#include "Arduino.h"
#include "API.h"
API::API()
{
  EthernetClient client;
}

void _post(Request &req, Response &res) {
  res.print("got a post World!");
}
void _pizza(Request &req, Response &res) {
  res.print("got a get some pizza!");
}

void API::begine()
{
  _initialiseApi();
}


void API::_initialiseApi(void)
{
  _app.use("/api", &_api);

  _app.get("/", &_post);
  _app.get("/pizza", &_pizza);
}
void API::checkEthernet(EthernetClient * client)
{

  if (client->connected())
  {
    _app.process(client);
    //client->stop();
  }    
}
