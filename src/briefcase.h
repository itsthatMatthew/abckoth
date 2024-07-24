#ifndef BRIEFCASE_H
#define BRIEFCASE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "utils/hw/button.h"
#include "utils/hw/led.h"
#include "utils/sw/task.h"

namespace abckoth {

// Constans for initialization
constexpr uint8_t RED_LED_PIN = 15;
constexpr uint8_t YELLOW_LED_PIN = 4;
constexpr uint8_t RESET_LED_PIN = 2;

constexpr uint8_t RED_BUTTON_PIN = 18;
constexpr uint8_t YELLOW_BUTTON_PIN = 19;
constexpr uint8_t RESET_BUTTON_PIN = 17;

using button_t = abckoth::FunctorButton<100, std::function<void(void)>>;

// Helper functions (global)
void blink_led(Led led);
void change_capturing_led(Led losing, Led capturing);

// Class declaration
class Briefcase : public Task<2 * 1024> {
public:
  Briefcase(const std::string& module_name);
  void create() override;
  void taskFunc() override;

private:
  // Helper functions
  void reset_led_blink();
  void update_counter();
  void print_standings();
  auto get_ahead_team();

  // Game state
  enum class KOTH_ACTIVE : uint8_t {
    INACTIVE = 0b00,
    RED = 0b01,
    YELLOW = 0b10,
    RESULTS = 0b11
  } capturing;

  // Members (software)
  std::size_t red_counter_millis = 0;
  std::size_t yellow_counter_millis = 0;
  std::size_t last_time_millis = 0;

  // Members (hardware peripheries)
  LiquidCrystal_I2C lcd_display{0x27, 16, 2};

  Led red_led{RED_LED_PIN};
  Led yellow_led{YELLOW_LED_PIN};
  Led reset_led{RESET_LED_PIN};

  button_t red_button{RED_BUTTON_PIN, [&](){
    if (capturing == KOTH_ACTIVE::RED) return;
    capturing = KOTH_ACTIVE::RED;
    change_capturing_led(yellow_led, red_led);
  }};

  button_t yellow_button{YELLOW_BUTTON_PIN, [&](){
    if (capturing == KOTH_ACTIVE::YELLOW) return;
    capturing = KOTH_ACTIVE::YELLOW;
    change_capturing_led(red_led, yellow_led);
  }};

  button_t reset_button{RESET_BUTTON_PIN, [&](){
    capturing = KOTH_ACTIVE::RESULTS;
  }};
}; // class Briefcase

} // namespace abckoth

#endif // BRIEFCASE_H