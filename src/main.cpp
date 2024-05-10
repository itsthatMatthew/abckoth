// 0123456789012346
// RED:    mm:ss  W
// YELLOW: mm:ss  L

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// #include <esp_log.h>
// static const char *TAG = "MAIN";

#include "utils/hw/button.h"
#include "utils/hw/led.h"

enum class KOTH_ACTIVE : uint8_t {
  INACTIVE,
  RED,
  YELLOW
} winning;

LiquidCrystal_I2C lcd_display(0x27, 16, 2);

std::size_t red_counter_millis = 0;
std::size_t yellow_counter_millis = 0;
std::size_t last_time_millis = 0;

abckoth::Led red_led(15);
abckoth::Led yellow_led(4);

void blink_led(const abckoth::Led& led) {
  led.on(); ::delay(200);
  led.off(); ::delay(200);
  led.on();
}

abckoth::FunctorButton<100, std::function<void(void)>> red_button(18, [&](){
  if (winning == KOTH_ACTIVE::RED) return;
  winning = KOTH_ACTIVE::RED;
  yellow_led.off();
  blink_led(red_led);
});
abckoth::FunctorButton<100, std::function<void(void)>> yellow_button(19, [&](){
  if (winning == KOTH_ACTIVE::YELLOW) return;
  winning = KOTH_ACTIVE::YELLOW;
  red_led.off();
  blink_led(yellow_led);
});

void setup() {
  winning = KOTH_ACTIVE::INACTIVE;

  lcd_display.init();
  lcd_display.backlight();
  lcd_display.clear();

  red_led.begin();
  yellow_led.begin();

  red_button.begin();
  yellow_button.begin();

  last_time_millis = ::millis();
}

void loop() {
  constexpr const char *format = "%c %-6s: %3u:%02u";

  // concurrency should not be a problem, since it is physically *hard* to do
  red_button();
  yellow_button();

  const std::size_t elapsed = ::millis() - last_time_millis;
  last_time_millis = ::millis();
  if (winning == KOTH_ACTIVE::RED) { red_counter_millis += elapsed; }
  else if (winning == KOTH_ACTIVE::YELLOW) { yellow_counter_millis += elapsed; }

  const auto ahead = winning == KOTH_ACTIVE::INACTIVE ? KOTH_ACTIVE::INACTIVE
    : red_counter_millis > yellow_counter_millis
      ? KOTH_ACTIVE::RED : KOTH_ACTIVE::YELLOW;

  lcd_display.setCursor(0,0);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::RED ? 'W' : 'L', "RED",
    red_counter_millis / 60000, red_counter_millis / 1000 % 60);

  lcd_display.setCursor(0, 1);
  lcd_display.printf(format, ahead == KOTH_ACTIVE::YELLOW ? 'W' : 'L', "YELLOW",
    yellow_counter_millis / 60000, yellow_counter_millis / 1000 % 60);
}