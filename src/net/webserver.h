#ifndef NET_WEBSERVER_H
#define NET_WEBSERVER_H


#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>

#include "utils/sw/task.h"

namespace abckoth {

class WebServer : Task<4 * 1024, delaypolicies::Delay<500>> {
  public:
  WebServer(const char* module_name = "webserver")
  : Task(module_name), m_web_server(80), m_dns_server() { }

  void create() override;
  void taskFunc() override;
  void on(const char*, WebRequestMethodComposite, ArRequestHandlerFunction);
private:
  AsyncWebServer m_web_server;
  AsyncDNSServer m_dns_server;
}; // class WebServer

} // namespace abckoth

#endif // NET_WEBSERVER_h