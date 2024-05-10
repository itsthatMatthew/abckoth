# Button header file documentation

The `button.h` header file contains several classes to match your specific needs in most situations. These utility classes can be easily dropped into your code to wrap raw digital pin reads and comparison.

## The `Button` class

The `Button` class is a simple wrapper for initializing and reading a digital pin. It's stateful with several helper function, which are handy to debounce a button in software, as a class template value can be set to dictate the minimum interval between digital reads.

The class has a public internal enum declared `StateChange`, which is used to keep track of the button's state change between digital reads.

### Class templates

|Type|Name|Default value
|-|-|-
|uint8_t|DEBOUNCE_DELAY_MILLIS|100 

### Member functions