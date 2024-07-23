#include "briefcase.h"
#include "net/webserver.h"

abckoth::Briefcase game{"briefcase"};

void setup() {
  game.create();
  abckoth::WebServer::instance.create();
}

void loop() {
}