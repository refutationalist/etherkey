// this is a group of rebindings to work with ATmega32u4

// shamelessly stolen from:
// https://www.sjoerdlangkemper.nl/2022/11/16/running-etherkey-on-arduino-leonardo/

#define KEY_UP KEY_UP_ARROW
#define KEY_DOWN KEY_DOWN_ARROW
#define KEY_RIGHT KEY_RIGHT_ARROW
#define KEY_LEFT KEY_LEFT_ARROW

#define KEYPAD_PLUS KEY_KP_PLUS
#define KEYPAD_0 KEY_KP_0

#define KEY_ENTER KEY_RETURN
#define KEY_SPACE ' '

#define MODIFIERKEY_CTRL KEY_LEFT_CTRL
#define MODIFIERKEY_ALT KEY_LEFT_ALT
#define MODIFIERKEY_SHIFT KEY_LEFT_SHIFT
#define MODIFIERKEY_GUI KEY_LEFT_GUI

// Arduino library has no keyboard led awareness :(
// https://github.com/arduino/ArduinoCore-avr/pull/446

#define keyboard_leds 0

#define PEEK_EMPTY -1