#include "briefcase.h"
#include "net/webserver.h"

abckoth::Briefcase game{"briefcase"};
auto& webserver = abckoth::WebServer::getInstance();

void setup() {
  game.create();
  webserver.create();
}

void loop() {
}