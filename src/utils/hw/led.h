#ifndef UTILS_HW_LED_H
#define UTILS_HW_LED_H

#include <Arduino.h>

namespace abckoth {

class Led {
 public:
  explicit constexpr Led(const uint8_t pin) noexcept : c_pin(pin) { }

  void begin() const noexcept { pinMode(c_pin, OUTPUT); }

  void on() const { digitalWrite(c_pin, HIGH); }
  void off() const { digitalWrite(c_pin, LOW); }
 private:
  const uint8_t c_pin;
}; // class Led

} // namespace abckoth


#endif // UTILS_HW_LED_H