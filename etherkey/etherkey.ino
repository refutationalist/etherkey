#include "usb-keyboard.h"
#include <EEPROM.h>

#ifdef ARDUINO_AVR_LEONARDO
#include "compat-arduino.h"
#endif

#define BAUD 115200

char in_ascii;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int crs_idx = 0;

int mode;
enum mode {INVALID, COMMAND, INTERACTIVE, DEBUG};
#define MODE_LEN 4
const char* mode_strings[] = {"invalid", "command", "interactive", "debug"};
const char* selectMode = "--> Input Mode: [1] Command - [2] Interactive - [3] Debug";

int verbosity = 0;
enum verbosity {SILENT, STANDARD, VERBOSE};
#define VERBOSITY_LEN 3
const char* verbosity_strings[] = {"silent", "standard", "verbose"};

void setup() {
  HWSERIAL.begin(BAUD);
  delay(1000);
  SerialClear();

  verbosity = EEPROM.read(EE_VERBOSITY) % VERBOSITY_LEN;
  mode = EEPROM.read(EE_MODE) % MODE_LEN;
  if (mode == 0) mode++;

  SerialPrintfln(
    "--> Etherkey Online: Mode: %s, Verbosity: %s",
    mode_strings[mode], verbosity_strings[verbosity]
  );
  if (mode != COMMAND) {
    HWSERIAL.println(F("--> For help, ^Q, press 1, then enter 'help'"));
  } else {
    HWSERIAL.println(F("--> Enter 'help' for the obvious."));
  }
}

void loop() {
  if (HWSERIAL.available() > 0) {
    in_ascii = HWSERIAL.read();

    if (in_ascii<0 || in_ascii>127)
      // Ignore non-basic ascii characters
      return;

    if (verbose_select(in_ascii)) return;
    if (mode_select(&in_ascii)) return;

    switch(mode) {
      case COMMAND:
        command_mode(in_ascii);
        break;

      case INTERACTIVE:
        interactive_mode(in_ascii);
        break;

      case DEBUG:
        debug_mode(in_ascii);
        break;
    }
  }
}

void help() {

  HWSERIAL.print(F(
		"Etherkey is a serial controlled keyboard.\n\r"
		"\n\r"
		"MODES: Change via ^Q\n\r"
		"  Interactive Mode -- all text is sent straight to target system\n\r"
		"  Command Mode     -- Send complex strings with modifiers via commands\n\r"
		"  Debug Mode       -- Show received characters from serial, no sending\n\r"
		"\n\r"
		"VERBOSITY: Change via ^S\n\r"
		"  Silent   -- Minimal information, interactive is silent\n\r"
		"  Standard -- Standard information, interactive shows what is sent\n\r"
		"  Verbose  -- Too much information\n\r"
		"\n\r"
		"COMMAND MODE:\n\r"
		"  A single char literal can be enclosed in braces. If integer i is \n\r"
		"  included after the char or keyname, it repeats that keypress i times.\n\r"
		"\n\r"
		"  Commands:\n\r"
		"    sendraw -- Send the following text literally\n\r"
		"    send    -- Send an interpreted string\n\r"
		"\n\r"
		"  Modifiers: Prepended to a char to modify them, as in ^C for Ctrl-C\n\r"
		"    ! ALT          + SHIFT          ^ CTRL          # WIN\n\r"
		"\n\r"
		"  Keynames: Send non-printable characters: (case indifferent)\n\r"
		"    {Enter}             {Escape}           {Space}            {Tab}\n\r"
		"    {Backspace}/{BS}    {Delete}/{Del}     {Insert}/{Ins}     {Up}\n\r"
		"    {Down}              {Left}             {Right}            {Home}\n\r"
		"    {End}               {PgUp}             {PgDn}             {Windows}/{Win}\n\r"
		"    {F1}..{F12}\n\r"
		"  These keypresses can also be entered as a singular command.\n\r"
		"\n\r"
		"There's other stuff too, see the docs.\n\r"
  ));

}
