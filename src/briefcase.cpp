// The function definitions and class implementation for the briefcase header

#include "briefcase.h"

namespace abckoth {

// blocking!
void blink_led(Led led) {
  led.on(); ::delay(200);  // !
  led.off(); ::delay(200); // !
  led.on();
};

void change_winning_led(Led losing, Led winning) {
  losing.off();
  winning.on();
  //blink_led(winning);
}

Briefcase::Briefcase(const std::string& module_name) : Task(module_name) { }

void Briefcase::create() {
  winning = KOTH_ACTIVE::INACTIVE;

  lcd_display.init();
  lcd_display.backlight();
  lcd_display.clear();

  red_led.begin();
  yellow_led.begin();
  reset_led.begin();

  red_button.begin();
  yellow_button.begin();
  reset_button.begin();

  last_time_millis = ::millis();
  
  Task::create();
}

void Briefcase::taskFunc() {
  // concurrency should not be a problem, it is physically *hard* to press both buttons
  reset_button();
  red_button();
  yellow_button();

  reset_led_blink();
  update_counter();
  print_standings();
}

void Briefcase::reset_led_blink() {
  static auto last_blink = ::millis();
  auto current = ::millis(); 

  switch (winning)
  {
  case KOTH_ACTIVE::INACTIVE:
    {
      if (current > last_blink + 1000) {
        last_blink = current;
        reset_led.on(); ::delay(100); reset_led.off();
      }
    }
    break;
  case KOTH_ACTIVE::RESULTS:
    {
      if (current > last_blink + 500) {
        last_blink = current;
        reset_led.on(); ::delay(100); reset_led.off();
      }
    }
  default: break;
  }
}

void Briefcase::update_counter() {
  const std::size_t elapsed = ::millis() - last_time_millis;
  last_time_millis = ::millis();
  if (winning == KOTH_ACTIVE::RED) { red_counter_millis += elapsed; }
  else if (winning == KOTH_ACTIVE::YELLOW) { yellow_counter_millis += elapsed; }
}

void Briefcase::print_standings() {
  constexpr const char *format = "%c %-6s: %3u:%02u";

  auto ahead = (winning == KOTH_ACTIVE::INACTIVE ? KOTH_ACTIVE::INACTIVE
    : red_counter_millis > yellow_counter_millis ? KOTH_ACTIVE::RED
      : KOTH_ACTIVE::YELLOW);
  
  lcd_display.setCursor(0,0);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::RED ? 'W' : 'L', "RED",
    red_counter_millis / 60000, red_counter_millis / 1000 % 60);

  lcd_display.setCursor(0, 1);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::YELLOW ? 'W' : 'L', "YELLOW",
    yellow_counter_millis / 60000, yellow_counter_millis / 1000 % 60);
}

} // namespace abckoth