// The function definitions and class implementation for the briefcase header

#include "briefcase.h"

namespace abckoth {

// blocking!
void blink_led(Led led) {
  led.on(); ::delay(200);  // !
  led.off(); ::delay(200); // !
  led.on();
};

void change_capturing_led(Led losing, Led capturing) {
  losing.off();
  capturing.on();
  //blink_led(capturing);
}

auto Briefcase::get_ahead_team() {
  switch (capturing)
  {
  case KOTH_ACTIVE::INACTIVE: return KOTH_ACTIVE::INACTIVE; break;
  default: return (red_counter_millis > yellow_counter_millis)
    ?  KOTH_ACTIVE::RED : KOTH_ACTIVE::YELLOW; break;
  }
}

Briefcase::Briefcase(const std::string& module_name) : Task(module_name) { }

void Briefcase::create() {
  capturing = KOTH_ACTIVE::INACTIVE;

  lcd_display.init();
  lcd_display.backlight();
  lcd_display.clear();

  red_led.begin();
  yellow_led.begin();
  reset_led.begin();
  reset_led.on();

  red_button.begin(INPUT_PULLDOWN);
  yellow_button.begin(INPUT_PULLDOWN);
  reset_button.begin(INPUT_PULLDOWN);

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

  switch (capturing)
  {
  case KOTH_ACTIVE::INACTIVE:
    {
      if (current > last_blink + 1000) {
        last_blink = current;
        //reset_led.on(); ::delay(100); reset_led.off();
      }
    }
    break;
  case KOTH_ACTIVE::RESULTS:
    {
      if (current > last_blink + 500) {
        last_blink = current;
        //reset_led.on(); ::delay(100); reset_led.off();
      }
    }
  default: break;
  }
}

void Briefcase::update_counter() {
  const std::size_t elapsed = ::millis() - last_time_millis;
  last_time_millis = ::millis();
  if (capturing == KOTH_ACTIVE::RED) { red_counter_millis += elapsed; }
  else if (capturing == KOTH_ACTIVE::YELLOW) { yellow_counter_millis += elapsed; }
}

void Briefcase::print_standings() {
  constexpr const char *format = "%c %-6s: %3u:%02u";

  auto ahead = get_ahead_team();
  
  lcd_display.setCursor(0,0);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::RED ? 'W' : 'L', "RED",
    red_counter_millis / 60000, red_counter_millis / 1000 % 60);

  lcd_display.setCursor(0, 1);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::YELLOW ? 'W' : 'L', "YELLOW",
    yellow_counter_millis / 60000, yellow_counter_millis / 1000 % 60);
}

} // namespace abckoth