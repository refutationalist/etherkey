# Etherkey

Emulate a conventional USB keyboard with a scriptable network capable microcontroller.

By using dedicated hardware it is possible to control systems even before the operating system is booted and without being dependent on the running software. For example this allows automatic bootloder selectios or modification of BIOS settings.

Also useful as "keyboard cable" in the bag of a roving sysadmin, especially when paired with a [cheap HDMI capture device](https://www.ebay.com/sch/i.html?_nkw=USB HDMI capture&_udhi=20). 

## Requirements
* Compatible Microcontroller
  * [Teensy](https://www.pjrc.com/teensy/index.html) (tested on Teensy 3 and 4.1)
  * An ATmega32u4-based Arduino ([Leonardo](https://docs.arduino.cc/hardware/leonardo/), [Micro](https://docs.arduino.cc/hardware/micro), Pro Micro [widely available])
* One of the following:
  * a) USB-to-UART Adapter, [for example](http://www.adafruit.com/product/954)
  * b) [Raspberry PI](http://www.raspberrypi.org/)

## Setup

* Flash the MCU with the sketch in the etherkey folder. (Using [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html))
* Connect the MCU's USB-Port to the System you want to control.

### a) Direct connection to MCU
You may connect directly to the MCU, using a USB-to-UART Adapter.

Connect Ground to the MCU's GND-Pin.  On a Teensy connect TX to Pin 0, RX to Pin 1.  Please check your MCU's documentation for the proper pins on an ATmega32u4-based MCU.

Now you can use any tool you like to connect to the Teensy. Baudrate is 115200, device most likely `/dev/ttyUSB0` on Linux/UNIX. For example: `cu -l /dev/ttyUSB0 -s 115200`

### b) Using a Raspberry PI for Network features
Example setup with a Raspberry PI for the ethernet connection.

![](doc/teensy-pi_bb.png)

Connect GND of the Raspberry to Teensy's GND-Pin, TX to Pin 0, RX to Pin 1.


When using Raspbian as operating system, the serial port [must be configured for outgoing connections](http://elinux.org/RPi_Serial_Connection#Connection_to_a_microcontroller_or_other_peripheral).
After that a serial connection can be established with `cu -l /dev/ttyAMA0 -s 115200`. On newer Raspberry PI or Raspbian versions you might need to use `/dev/serial0` instead.


## Usage and Syntax

### Mode selection
`Ctrl+Q` to switch between modes.

Available modes:
* Interactive mode
* Command mode
* Debug mode

### Verbosity selection
`Ctrl+S` changes the verbosity of etherkey.

* Silent prints out very little.  Specifically it does not echo what is typed in Interactive mode.
* Standard prints out useful information about what is entered into the USB end of the keyboard.   This is considered the default.
* Verbose mode includes a lot of debugging information of questionable usefulness.

### Interactive mode
Directly sends the recieved keystroke
Supported Characters:

* All printable ASCII characters
* Arrow keys
* Backspace
* Enter
* Delete
* Tab
* Escape

### Command mode
Parses the whole line and interprets the first Word as command. Commands are case-insensitive.

#### Available Commands

##### SendRaw
Sends the rest of the line literally

##### Send
Sends the rest of the line while interpreting special characters.
This command behaves similarly to the send command of [AutoHotkey](http://ahkscript.org/docs/commands/Send.htm)

##### Help
Prints out some useful help, a simplified and denser version of what is available here.

##### Clear
Clears the screen.


#### Modifiers
The following characters are treated as modifiers:

* `!`: Send the next character with the ALT key pressed.

  Example: `Send text!a` sends the keys "This is text" and then presses `ALT+a`.

* `+`: Send the next character with the SHIFT key pressed.

  Example: `Send +abC` sends the keys "AbC".

* `^`: Send the next character with the CTRL key pressed.

  Example: `Send ^c` sends a `CTRL+c` keystroke.

* `#`: Send the next character with the WIN key pressed.

  Example: `Send #d` sends a `WIN + d` keystroke.

These modifiers can be combined, so a `^+t` would send a `CTRL+SHIFT+t`, thus restoring the last tab in Firefox.

#### Keynames
Non printable characters can be sent by specifying the keyname enclosed in braces:

* `{Enter}`
* `{Escape}`
* `{Space}`
* `{Tab}`
* `{Backspace}/{BS}`
* `{Delete}/{Del}`
* `{Insert}/{Ins}`
* `{Up}`
* `{Down}`
* `{Left}`
* `{Right}`
* `{Home}`
* `{End}`
* `{PgUp}`
* `{PgDn}`
* `{Windows}/{Win}`
* `{F1}`..`{F12}`

These keynames can also be combined with modifiers. For example `Send ^!{Delete}` sends the `CTRL+SHIFT+Delete` combination.

Many of these keynames can be used as simple commands.  For example, `F1` by itself will sent the F1 key.

#### Escape sequences

To send a single character literally, it can be enclosed in braces:

* `{x}`
* `{!}`
* `{+}`
* `{^}`
* `{#}`
* `{{}`
* `{}}`

This syntax can also be used to repeat a keystroke multiple times:

Enclose the character or keyname followed by a whitespace and the number of repetitions in braces.

* For example: `{x 10}` sends the x character 10 times and `{Enter 5}` presses the Enter key 5 times.

#### Unicode characters
Arbitrary Unicode characters can be sent by specifying the 4-digit character code after the operating system specific command:

##### UnicodeLinux or UCL
Initializes the GKT+/Qt Unicode Sequence and sends the following 4-digit hexadecimal Unicode Character.

For example: `{UCL 00F8}`

##### UnicodeWindows or UCW (experimental)
Initializes the Windows Unicode Sequence and sends the following Unicode Character. Please note: Some Windows applications require 4-digit decimal Code (e.g. Wordpad, Chrome), some other require 4-digit hexadecimal Code (e.g. Notepad++, Firefox)

You might as well need to [change a Registry Setting](http://en.wikipedia.org/wiki/Unicode_input#In_Microsoft_Windows) on your Windows machine.

For example: `{UCW 00F8}` or `{UCW 0248}`


### Debug mode
Displays information about the received character (ASCII code, USB keycode)


## Client
TODO

There is some code in the client directory.  This fork has not tried it.

## Known issues/ Caveats

### Keyboard Layouts
Due to the nature of the emulation on a USB keyboard layer, the list of available characters is determined by the keyboard layout on the target host.

Additionally only basic ASCII characters are transmitted correctly over the serial interface.

Thus an English keyboard layout is recommended for conventional usage. This is also configured by default in the Teensyduino development environment, but can be changed under "Tools" -> "Keyboard Layout".

To transmit other keys:

  * Sending keys by their name in command mode
  * Mixing keyboard layouts for transmission to send keystrokes defined by their position on the keyboard

By setting the desired keyboard layout on the target system while configuring an English keyboard layout on the Teensy and the controlling system, it is possible to send keystronkes independent of the possibility to transmit the associated character.  This works as the pressed key is interpreted on the controlling system.

### Arduino Library lacks Keyboard LED Support

It's been in the waiting room for a very long time.  See [this post](https://www.sjoerdlangkemper.nl/2022/11/16/running-etherkey-on-arduino-leonardo/), where the Arduino compatibility (mostly) comes from.


## Work Done On This Fork

I'm building the [Forbidden Cable](https://electronics.stackexchange.com/questions/644717/are-usb-2-0-type-a-male-male-cables-compliant-with-usb-specification).

Combined with a cheap HDMI capture device, `picocom`, and `vlc`, I'll be able to use my computer as a head for any servers I may encounter.   This includes a small one in my bag for development purposes.   Since I'm talking to it directly rather than through a script of some kind, I wanted the interface to be relatively smooth.

Here's what I've done:

* Arduino compatibility
* Verbosity switching (to hide passwords, mostly)
* UI cleanup
* persistent mode and verbosity
* easier to read TUI
* An overflow bug
* A timing hack to let arrow keys to work in command mode
* 115200 baud
* help text
* some cleanup on command parsing

I've tried to keep the commits pretty atomic so it can be easier to upstream in parts, should that be useful to do.
