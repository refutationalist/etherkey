#include "usb-keyboard.h"

#ifdef ARDUINO_AVR_LEONARDO
#include "Keyboard.h"
#include "compat-arduino.h"
#endif

#define BAUD 115200

char in_ascii;
char kbd_buff[KBD_BUFFSZ];
int kbd_idx = 0;
int crs_idx = 0;

int mode = 1;
int newmode = 0;
enum mode {INVALID, COMMAND, INTERACTIVE, DEBUG};
const char* mode_strings[] = {"invalid", "command", "interactive", "debug"};
const char* selectMode = "--> Input Mode: [1] Command - [2] Interactive - [3] Debug";

int verbosity = 0;
enum verbosity {SILENT, STANDARD, VERBOSE};
const char* verbosity_strings[] = {"silent", "standard", "verbose"};

void setup() {
  HWSERIAL.begin(BAUD);
  delay(1000);
  SerialClear();
  SerialPrintfln(
    "--> etherkey running.  mode:[%s] verbosity:[%s]",
    mode_strings[mode], verbosity_strings[verbosity]
  );
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
