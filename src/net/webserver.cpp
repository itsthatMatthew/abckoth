#include "webserver.h"
#include "websrc.h"

namespace abckoth
{

WebServer& WebServer::instance{ WebServer::getInstance() };

void WebServer::create() {
  assert(WiFi.mode(WIFI_MODE_AP));
  assert(WiFi.softAPConfig({192, 168, 1, 1}, {192, 168, 1, 1}, {255, 255, 255, 0}));
  assert(WiFi.softAP("ABC:KOTH", nullptr));

  m_web_server.onNotFound([](AsyncWebServerRequest *req) {
    req->send_P(404, "text/html", notfound_html);
  });

  for (auto& src : websrc) {
    m_web_server.on(src.name, HTTP_GET, [=](AsyncWebServerRequest *req) {
      req->send_P(200, src.type, src.data);
    });
  }

  m_web_server.begin();

  m_dns_server.setTTL(300);
  m_dns_server.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);
  assert(m_dns_server.start(53, "abckoth.open", {192, 168, 1, 1}));

  Task::create();
}

void WebServer::taskFunc() {}

} // namespace abckoth
