#include "webserver.h"
#include "websrc.h"

namespace abckoth
{

WebServer& WebServer::instance{ WebServer::getInstance() };

void WebServer::create() {
  assert(WiFi.mode(WIFI_MODE_AP));
  assert(WiFi.softAPConfig({192, 168, 1, 1}, {192, 168, 1, 1}, {255, 255, 255, 0}));
  assert(WiFi.softAP("ABC:KOTH", nullptr));

  m_web_server.onNotFound([](auto *req) {
    req->send(404, "text/html", notfound_html);
  });

  for (auto& src : websrc) {
    m_web_server.on(src.name, HTTP_GET, [=](auto *req) {
      req->send(200, src.type, src.data);
    });
  }

  m_web_server.on("/getDiagnosticsData", HTTP_GET, [](auto *req) {
    using std::string, std::to_string;

    const string esp_idf_version = esp_get_idf_version();
    const string esp_arduino_core_version =
      to_string(ESP_ARDUINO_VERSION_MAJOR) + "." +
      to_string(ESP_ARDUINO_VERSION_MINOR)+ "." +
      to_string(ESP_ARDUINO_VERSION_PATCH);
    const string esp_model = string{ESP.getChipModel()} +
      " (rev. " + to_string(ESP.getChipRevision()) + ")";
    const string esp_cores = to_string(ESP.getChipCores());
    const string esp_cpu_freq_mhz = to_string(ESP.getCpuFreqMHz());
    const string esp_flash_chip_speed = to_string(ESP.getFlashChipSpeed());
    const string esp_flash_chip_size = to_string(ESP.getFlashChipSize());
    const string esp_heap_size = to_string(ESP.getHeapSize());
    const string esp_min_heap = to_string(ESP.getMinFreeHeap());
    const string esp_free_heap = to_string(ESP.getFreeHeap());
    const string esp_mac_address = to_string(ESP.getEfuseMac());

    string json_response = string{"{"} +
        "\"esp_idf_version\":\"" + esp_idf_version + "\"," +
        "\"esp_arduino_core_version\":\"" + esp_arduino_core_version + "\"," +
        "\"esp_model\":\"" + esp_model + "\"," +
        "\"esp_cores\":" + esp_cores + "," +
        "\"esp_cpu_freq_mhz\":" + esp_cpu_freq_mhz + "," +
        "\"esp_flash_chip_speed\":" + esp_flash_chip_speed + "," +
        "\"esp_flash_chip_size\":" + esp_flash_chip_size + "," +
        "\"esp_heap_size\":" + esp_heap_size + "," +
        "\"esp_min_heap\":" + esp_min_heap + "," +
        "\"esp_free_heap\":" + esp_free_heap + "," +
        "\"esp_mac_address\":" + esp_mac_address +
      "}";

      req->send(200, "text/json", json_response.c_str());
  });

  m_web_server.begin();

  m_dns_server.setTTL(300);
  m_dns_server.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);
  assert(m_dns_server.start(53, "abckoth.open", {192, 168, 1, 1}));

  Task::create();
}

void WebServer::taskFunc() {}

void WebServer::on(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest) {
  m_web_server.on(uri, method, onRequest);
}

} // namespace abckoth
