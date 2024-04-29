#ifndef UTILS_HW_BUTTON_H
#define UTILS_HW_BUTTON_H

#include <Arduino.h>
#include <optional>

namespace abckoth
{

/// @brief Utility wrapper class for initializing and reading digital pins as physical buttons
/// @tparam DEBOUNCE_DELAY_MILLIS minimum cooldown time between new digital reads
template<uint32_t DEBOUNCE_DELAY_MILLIS = 100>
class Button {
 public:
  enum StateChange : uint8_t {
      IS_RELEASED = 0b00,
      IS_RISING   = 0b01,
      IS_FALLING  = 0b10,
      IS_PRESSED  = 0b11,
  };

  /// @param pin the digital pin assigned to the button
  /// @param init whether or not to call begin() right away
  explicit constexpr Button(const uint8_t pin, const bool init = false) noexcept
  : c_pin(pin), m_state(LOW), m_delay_until(0)
  {
    if (init) begin();
  }
  
  /// @brief Initializes the digital pin and reads state
  void begin() const noexcept {
    pinMode(c_pin, INPUT);
    readNewState();
  }
  
  /// @brief Reads a new state if the cooldown time is down
  /// @return High or low value as in digitalRead()
  uint8_t readNewState() const noexcept {
    if (::millis() >= m_delay_until)
      m_state = digitalRead(c_pin);

    return m_state;
  }

  /// @return The stored state without updating
  [[nodiscard]] uint8_t getStoredState() const noexcept { return m_state; }

  /// @return The button's state change determined from its stored value and a new digital read
  [[nodiscard]] StateChange getStateChange() const noexcept {
    StateChange change = static_cast<StateChange>(0b00);

    if (getStoredState() == HIGH) // old state
      change = static_cast<StateChange>(0b10);

    if (readNewState() == HIGH) // new state
      change = static_cast<StateChange>(change | 0b01);

    if (change == RISING || change == FALLING) // only if changed
      m_delay_until = millis() + DEBOUNCE_DELAY_MILLIS;

    return change;
  }

 private:
  const uint8_t c_pin;
  mutable uint8_t m_state;
  mutable unsigned long m_delay_until;
}; // class Button

/// @brief Button class with callbacks available to set to the corresponding state changes
/// @tparam DEBOUNCE_DELAY_MILLIS minimum cooldown time between new digital reads, forwarded to the Button class
/// @tparam CALLBACK_RETURN_TYPE the return type of the callbacks
/// @tparam ...CALLBACK_ARGUMENT_TYPES the argument types of the callbacks
template<
  uint32_t DEBOUNCE_DELAY_MILLIS = 100,
  typename CALLBACK_RETURN_TYPE = void,
  typename... CALLBACK_ARGUMENT_TYPES
>
class CallbackButton : public Button<DEBOUNCE_DELAY_MILLIS>
{
 public:
  using CALLBACK_TYPE = CALLBACK_RETURN_TYPE(*)(CALLBACK_ARGUMENT_TYPES...);

  /// @param pin the digital pin assigned to the button
  /// @param on_rising callback for when the button is in a rising state
  /// @param on_falling callback for when the button is in a falling state
  /// @param on_pressed callback for when the button is in a pressed state
  /// @param on_released callback for when the button is in a released state
  /// @param init whether or not to call begin() right away
  explicit constexpr CallbackButton(const uint8_t pin, CALLBACK_TYPE on_rising = nullptr, CALLBACK_TYPE on_falling = nullptr, CALLBACK_TYPE on_pressed = nullptr, CALLBACK_TYPE on_released = nullptr, const bool init = false) noexcept
  : Button<>(pin, init), m_on_rising(on_rising), m_on_falling(on_falling), m_on_pressed(on_pressed), m_on_released(on_released)
  { }
  
  /// @brief reads the button's state change, and calls the appropriate callback (if none is set, returns an empty std::optional)
  /// @param ...args the parameters of the callbacks
  /// @return the value expected from the callbacks
  auto update(CALLBACK_ARGUMENT_TYPES... args) const noexcept {
    constexpr auto return_void = std::is_same_v<void, CALLBACK_RETURN_TYPE>;

    switch (Button<>::getStateChange()) {
      case Button<>::IS_RISING:   if (m_on_rising)   { if constexpr (return_void) m_on_rising(args...);   else return std::optional<CALLBACK_RETURN_TYPE>{m_on_rising(args...)}; }   break;
      case Button<>::IS_FALLING:  if (m_on_falling)  { if constexpr (return_void) m_on_falling(args...);  else return std::optional<CALLBACK_RETURN_TYPE>{m_on_falling(args...)}; }  break;
      case Button<>::IS_PRESSED:  if (m_on_pressed)  { if constexpr (return_void) m_on_pressed(args...);  else return std::optional<CALLBACK_RETURN_TYPE>{m_on_pressed(args...)}; }  break;
      case Button<>::IS_RELEASED: if (m_on_released) { if constexpr (return_void) m_on_released(args...); else return std::optional<CALLBACK_RETURN_TYPE>{m_on_released(args...)}; } break;
    }
    if constexpr (!return_void) return std::optional<CALLBACK_RETURN_TYPE>{};
  }

  /// @brief reads the button's state change, and calls the appropriate callback (if none is set, returns an empty std::optional)
  /// @param ...args the parameters of the callbacks
  /// @return the value expected from the callbacks, either void or std::optional<T>
  auto operator()(CALLBACK_ARGUMENT_TYPES... agrs) const noexcept { return update(agrs...); }
  
  /// @param callback the new callback to be set
  void onRising(CALLBACK_TYPE callback) noexcept { m_on_rising = callback; }
  /// @param callback the new callback to be set
  void onFalling(CALLBACK_TYPE callback) noexcept { m_on_falling = callback; }
  /// @param callback the new callback to be set
  void onPressed(CALLBACK_TYPE callback) noexcept { m_on_pressed = callback; }
  /// @param callback the new callback to be set
  void onReleased(CALLBACK_TYPE callback) noexcept { m_on_released = callback; }
  
 private:
  CALLBACK_TYPE m_on_rising;
  CALLBACK_TYPE m_on_falling;
  CALLBACK_TYPE m_on_pressed;
  CALLBACK_TYPE m_on_released;
}; // class CallbackButton

/// @brief Button class with functors available to set to the corresponding state changes
/// @tparam DEBOUNCE_DELAY_MILLIS minimum cooldown time between new digital reads, forwarded to the Button class
/// @tparam CALLBACK_TYPE the type of the functor supplied (e.g. as a lambda expression)
/// @tparam ...CALLBACK_ARGUMENT_TYPES the argument types of the functors
template<
  uint32_t DEBOUNCE_DELAY_MILLIS = 100,
  typename CALLBACK_TYPE = std::function<void(void)>,
  typename... CALLBACK_ARGUMENT_TYPES
>
class FunctorButton : public Button<DEBOUNCE_DELAY_MILLIS>
{
 public:
  using CALLBACK_RETURN_TYPE = typename std::invoke_result_t<CALLBACK_TYPE, CALLBACK_ARGUMENT_TYPES...>;

  /// @param pin the digital pin assigned to the button
  /// @param on_rising functor for when the button is in a rising state
  /// @param on_falling functor for when the button is in a falling state
  /// @param on_pressed functor for when the button is in a pressed state
  /// @param on_released functor for when the button is in a released state
  /// @param init whether or not to call begin() right away
  explicit constexpr FunctorButton(const uint8_t pin, CALLBACK_TYPE on_rising = nullptr, CALLBACK_TYPE on_falling = nullptr, CALLBACK_TYPE on_pressed = nullptr, CALLBACK_TYPE on_released = nullptr, const bool init = false) noexcept
  : Button<>(pin, init), m_on_rising(on_rising), m_on_falling(on_falling), m_on_pressed(on_pressed), m_on_released(on_released)
  {}
  
  /// @brief reads the button's state change, and calls the appropriate functor (if none is set, returns an empty std::optional)
  /// @param ...args the parameters of the functors
  /// @return the value expected from the functor, either void or std::optional<T>
  auto update(CALLBACK_ARGUMENT_TYPES... args) const noexcept {
    constexpr auto return_void = std::is_same_v<void, CALLBACK_RETURN_TYPE>;

    switch (Button<>::getStateChange()) {
      case Button<>::IS_RISING:   if (m_on_rising)   { if constexpr (return_void) m_on_rising(args...);   else return std::optional<CALLBACK_RETURN_TYPE>{m_on_rising(args...)}; }   break;
      case Button<>::IS_FALLING:  if (m_on_falling)  { if constexpr (return_void) m_on_falling(args...);  else return std::optional<CALLBACK_RETURN_TYPE>{m_on_falling(args...)}; }  break;
      case Button<>::IS_PRESSED:  if (m_on_pressed)  { if constexpr (return_void) m_on_pressed(args...);  else return std::optional<CALLBACK_RETURN_TYPE>{m_on_pressed(args...)}; }  break;
      case Button<>::IS_RELEASED: if (m_on_released) { if constexpr (return_void) m_on_released(args...); else return std::optional<CALLBACK_RETURN_TYPE>{m_on_released(args...)}; } break;
    }
    if constexpr (!return_void) return std::optional<CALLBACK_RETURN_TYPE>{};
  }

  /// @brief reads the button's state change, and calls the appropriate functor (if none is set, returns an empty std::optional)
  /// @param ...args the parameters of the functors
  /// @return the value expected from the functor, either void or std::optional<T>
  auto operator()(CALLBACK_ARGUMENT_TYPES... agrs) const noexcept { return update(agrs...); }
  
  /// @param callback the new functor to be set
  void onRising(CALLBACK_TYPE callback) noexcept { m_on_rising = callback; }
  /// @param callback the new functor to be set
  void onFalling(CALLBACK_TYPE callback) noexcept { m_on_falling = callback; }
  /// @param callback the new functor to be set
  void onPressed(CALLBACK_TYPE callback) noexcept { m_on_pressed = callback; }
  /// @param callback the new functor to be set
  void onReleased(CALLBACK_TYPE callback) noexcept { m_on_released = callback; }
  
 private:
  CALLBACK_TYPE m_on_rising;
  CALLBACK_TYPE m_on_falling;
  CALLBACK_TYPE m_on_pressed;
  CALLBACK_TYPE m_on_released;
}; // class CapturingCallbackButton

} // namespace abckoth

#endif // UTILS_HW_BUTTON_H