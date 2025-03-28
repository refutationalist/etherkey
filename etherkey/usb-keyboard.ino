#include "usb-keyboard.h"

// Util functions
int mode_select(char* in_ascii) {
  int newmode = 0;
  char peekChar;

  if (*in_ascii == PREFIX) {
    SerialPrintf("%s", selectMode);
    do {

      // wait for a char to become available,
      // then see what it is without grabbing
      while (HWSERIAL.available() == 0) {
        delay(100);
      }
      peekChar = HWSERIAL.peek();

      // are we a prefix character?  then just send it.
      if (peekChar == PREFIX  || peekChar == VPREFIX) {
        *in_ascii = HWSERIAL.read();
        SerialDeleteChars(strlen(selectMode));
        return 0;
      
      // are we one of the modal numbers?  then change mode.
      } else if (peekChar > '0' && peekChar < ('0'+sizeof(mode_strings)/sizeof(char*))) {
        *in_ascii = HWSERIAL.read();
        mode = *in_ascii - '0';
        SerialClear();
        SerialPrintfln("--> Switching to mode: %s", mode_strings[mode]);
        EEPROM.update(EE_MODE, mode);
        return 1;

      // are we the escape key?   then ditch.
      } else if (peekChar == 27) {
        HWSERIAL.read();
        SerialDeleteChars(strlen(selectMode));
        return 0;

      // are we any other key?  try again.
      } else {
        HWSERIAL.read();
        SerialDeleteChars(strlen(selectMode));
        SerialPrintf("%s", selectMode);
      }
    } while (peekChar != 27 || peekChar != PREFIX || peekChar != VPREFIX || (peekChar >= 49 && peekChar <= 51));
    //          is not esc  ||  is not ^Q         ||  is not ^S          ||  is not between 1 and 3 inclusive
  }
  return 0;
}

int verbose_select(char in_ascii) {
  int newverbose;

  if (in_ascii == VPREFIX) {
    verbosity = (++verbosity) % VERBOSITY_LEN;
    SerialPrintfln("--> Verbosity: %s", verbosity_strings[verbosity]);
    EEPROM.update(EE_VERBOSITY, verbosity);
    return 1;
  } else {
    return 0;
  }

}

uint16_t escape_sequence_to_keycode(char key) {
  char in_ascii = key;
  uint16_t keycode = 0;

  if (in_ascii == 91) {
    in_ascii = HWSERIAL.peek();
    if (in_ascii >= 65 && in_ascii <= 68) {
      HWSERIAL.read();
      if (in_ascii == 65)
        keycode = KEY_UP;
      if (in_ascii == 66)
        keycode = KEY_DOWN;
      if (in_ascii == 67)
        keycode = KEY_RIGHT;
      if (in_ascii == 68)
        keycode = KEY_LEFT;
    } else if (in_ascii == 51) {
      HWSERIAL.read();
      in_ascii = HWSERIAL.peek();
      if (in_ascii == 126) {
        HWSERIAL.read();
        keycode = KEY_DELETE;
      }
    }
  }
  return keycode;
}

uint16_t special_char_to_keycode(char in_ascii) {
  uint16_t keycode = 0;

  switch(in_ascii) {
    case 10:  //LF
    case 13:  //CR
      keycode = KEY_ENTER;
      break;
    case 8:   //BS
    case 127: //DEL
      keycode = KEY_BACKSPACE;
      break;
    case 9:   //HT
      keycode = KEY_TAB;
      break;
    case 27:
      delay(10);  // wait for more characters to hit the buffer
      in_ascii = HWSERIAL.peek();
      if(in_ascii == PEEK_EMPTY) {
        keycode = KEY_ESC;
      } else {
        HWSERIAL.read();
        keycode = escape_sequence_to_keycode(in_ascii);
      }
      break;
  }
  return keycode;
}

uint16_t keyname_to_keycode(const char* keyname) {
  uint16_t keycode = 0;
  char keyname_lower[KEYNAME_BUFFSZ];

  strncpy(keyname_lower, keyname, KEYNAME_BUFFSZ);

  IF_VERBOSE SerialPrintfln("\tKeyname: %-15s -> %x", keyname, str2int(keyname));

  // keyname matching shall be case-insensitive
  for (uint8_t i = 0; i <= strlen(keyname); i++) {
    keyname_lower[i] = tolower(keyname_lower[i]);
  }

  // F1 - F12
  for (uint8_t f = 1; f <= 12; f++) {\
    char fkey[4];
    snprintf(fkey, sizeof(fkey), "f%d", f);
    if (!strcmp(fkey, keyname_lower)) {
      keycode = KEY_F1+f-1;
      break;
    }
  }

  // Check for other special keynames, if none has been found yet
  if (!keycode) {
    switch (str2int(keyname_lower)) {
      case str2int("enter"):
        keycode = KEY_ENTER;
        break;
      case str2int("escape"):
      case str2int("esc"):
        keycode = KEY_ESC;
        break;
      case str2int("space"):
        keycode = KEY_SPACE;
        break;
      case str2int("tab"):
        keycode = KEY_TAB;
        break;
      case str2int("backspace"):
      case str2int("bs"):
        keycode = KEY_BACKSPACE;
        break;
      case str2int("delete"):
      case str2int("del"):
        keycode = KEY_DELETE;
        break;
      case str2int("insert"):
      case str2int("ins"):
        keycode = KEY_INSERT;
        break;
      case str2int("up"):
        keycode = KEY_UP;
        break;
      case str2int("down"):
        keycode = KEY_DOWN;
        break;
      case str2int("left"):
        keycode = KEY_LEFT;
        break;
      case str2int("right"):
        keycode = KEY_RIGHT;
        break;
      case str2int("home"):
        keycode = KEY_HOME;
        break;
      case str2int("end"):
        keycode = KEY_END;
        break;
      case str2int("pgup"):
        keycode = KEY_PAGE_UP;
        break;
      case str2int("pgdn"):
        keycode = KEY_PAGE_DOWN;
        break;
      case str2int("win"):
      case str2int("windows"):
        keycode = KEY_LEFT_GUI;
        break;
    }
  }
  return keycode;
}

void usb_send_key(uint16_t key, uint16_t mod=0) {
  // Uses Keyboard.press from the Teensyduino Core Library
  // key can be either a ascii char or a keycode

  IF_VERBOSE {
    char key_b[33];
    itoa(key, key_b, 2);
    char mod_b[33];
    itoa(mod, mod_b, 2);
    SerialPrintfln("\tSendKey: %6i = %08s | mod: %6i = %08s", key, key_b, mod, mod_b);
  }

  if (mod) Keyboard.press(mod);
  Keyboard.press(key);
  Keyboard.releaseAll();
}


// Interactive mode functions
void interactive_mode(char in_ascii) {
  uint16_t keycode;

  if ((keycode = special_char_to_keycode(in_ascii))) {
    usb_send_key(keycode);
    switch(keycode) {
      case KEY_ENTER:
        IF_STANDARD SerialPrintfln("");
        break;
      case KEY_BACKSPACE:
        IF_STANDARD SerialDeleteChars(1);
        break;
    }
  } else if (in_ascii <= 26) {
    in_ascii = in_ascii + 'a' - 1;
    usb_send_key(in_ascii, MODIFIERKEY_CTRL);
  } else {
    IF_STANDARD HWSERIAL.write(in_ascii);
    usb_send_key(in_ascii);
  }
}

// Command mode functions
void command_mode(char in_ascii) {
  uint16_t keycode = special_char_to_keycode(in_ascii);

  if(keycode) {
    switch(keycode) {
      case KEY_ENTER:
        SerialPrintfln("");
        kbd_buff[kbd_idx] = '\0';
        c_parse(kbd_buff);
        crs_idx = kbd_idx;
        kbd_idx = 0;
        break;
      case KEY_BACKSPACE:
        if (crs_idx == kbd_idx) {
          // TODO: Currently only supports deleting characters at the end of the line
          // To delete in the middle of the Array, all following characters must be moved
          SerialDeleteChars(1);
          if (kbd_idx>0) kbd_idx--;
          if (crs_idx>0) crs_idx--;
        }
        break;
      case KEY_LEFT:
        if (crs_idx>0) {
          SerialAnsiEsc("1D");
          crs_idx--;
        }
        break;
      case KEY_RIGHT:
        if (crs_idx<kbd_idx) {
          SerialAnsiEsc("1C");
          crs_idx++;
        }
        break;
      case KEY_UP:
        while (crs_idx>kbd_idx) {
          if (!kbd_buff[kbd_idx])
            kbd_buff[kbd_idx] = ' ';
          HWSERIAL.write(kbd_buff[kbd_idx++]);
        }
        break;
    }
  } else if (in_ascii == 3) {
    SerialClearLine();
    crs_idx = kbd_idx;
    kbd_idx = 0;
  } else if (kbd_idx >= KBD_BUFFSZ-1) {
    command_mode('\n');
  } else if (in_ascii <= 26) {
  } else {
    HWSERIAL.write(in_ascii);
    if (crs_idx>kbd_idx) crs_idx = kbd_idx;
    kbd_buff[crs_idx++] = in_ascii;
    if (kbd_idx<crs_idx) kbd_idx++;
  }
}

void c_parse(char* str) {
  char* pch;

  if (!(pch = strtok(str," "))) return;
  IF_VERBOSE SerialPrintfln("\tCommand: %-15s -> %x", pch, str2int(pch));

  // let's lower case the thing
  for ( char *p = pch ; *p ; ++p) *p = tolower(*p);

  switch (str2int(pch)) {
    case str2int("sendraw"):
      // Send the rest of the line literally
      if ((pch = strtok (NULL,"")))
        c_sendraw(pch);
      break;

    case str2int("send"):
      // Send the rest of the line (and parse special characters)
      if ((pch = strtok (NULL,"")))
        c_send(pch);
      break;

    case str2int("help"):
      help();
      break;

    case str2int("clear"):
      SerialClear();
      break;

    default:
      // Check if input is a keyname and send it

      if (!c_parse_ext(pch, false, 0)) {
        HWSERIAL.println(F("--> Bad command or taco"));
      }
      break;
  }
}

bool c_parse_ext(char* str, bool send_single, int modifier) {
  // Sends matching keyname or reacts to special commands
  uint16_t key = 0;
  int num = 1;
  char* pch;

  IF_VERBOSE SerialPrintfln("\tCmd-ext: %-15s -> %x", str, str2int(str));
  if (!(key = keyname_to_keycode(str))) {
    if (send_single && !*(str+1) && (*str >= 32 || *str < 127)) {
      key = *str;
    }
  }
  if (key) {
    if ((str = strtok(NULL," ")))
      num = atoi(str);
    while (num>0) {
      usb_send_key(key, modifier);
      num--;
    }
    return true;
  } else {
    for ( char *p = str ; *p ; ++p) *p = tolower(*p);
    
    switch (str2int(str)) {
      case str2int("sleep"):
      case str2int("delay"):
        // Sleep a certain amount of time in ms
        if ((pch = strtok(NULL,"")) && (num = atoi(pch))) {
          c_sleep(num);
          return true;

        }
        break;
      case str2int("unicodelinux"):
      case str2int("ucl"):
        // Send a single unicode character (on Linux)
        if ((pch = strtok (NULL,""))) {
          c_unicode(pch, true);
          return true;
        }
        break;

      case str2int("unicodewindows"):
      case str2int("ucw"):
        // Send a single unicode character (on Windows)
        if ((pch = strtok (NULL,""))) {
          c_unicode(pch, false);
        }
        break;
    }
  }
  return false;
}

void c_sendraw(char* pch) {
  char* c = pch;

  while (*c) {
    usb_send_key(*c);
    c++;
  }
}

void c_send(char* pch) {
  char* c = pch;
  int modifier = 0;
  bool braces = false;
  char keyname_buff[KEYNAME_BUFFSZ];
  int keyname_idx = 0;

  while (*c) {
    if (braces) {
      if ((keyname_idx >= KEYNAME_BUFFSZ-1) || *c == '}') {
        keyname_buff[keyname_idx] = '\0';
        if ((pch = strtok(keyname_buff," "))) {
          c_parse_ext(pch, true, modifier);
        }
        keyname_idx = 0;
        braces = false;
        modifier = 0;
      } else {
        keyname_buff[keyname_idx++] = *c;
      }
      c++;
      continue;
    }
    switch (*c) {
      case '!':
        // ALT
        modifier |= MODIFIERKEY_ALT;
        break;
      case '+':
        // SHIFT
        modifier |= MODIFIERKEY_SHIFT;
        break;
      case '^':
        // CTRL
        modifier |= MODIFIERKEY_CTRL;
        break;
      case '#':
        // WIN
        modifier |= MODIFIERKEY_GUI;
        break;
      case '{':
        braces = true;
        break;
      default:
        usb_send_key(*c, modifier);
        modifier = 0;
        break;
    }
    c++;
  }
}

void c_unicode(char* pch, bool linux) {
  char* c = pch;
  int modifier = 0;
  if (linux) {
    modifier |= MODIFIERKEY_CTRL;
    modifier |= MODIFIERKEY_SHIFT;
    usb_send_key('u', modifier);
    while (*c) {
      usb_send_key(*c);
      c++;
    }
    usb_send_key(KEY_ENTER);
  } else {
    if (!keyboard_leds & (1<<0))
      usb_send_key(KEY_NUM_LOCK);
    Keyboard.press(MODIFIERKEY_ALT);
    Keyboard.press(KEYPAD_PLUS);
    Keyboard.release(KEYPAD_PLUS);
    while (*c) {
      if (*c >= '1' && *c <= '9') {
        Keyboard.press((uint16_t)(*c+40) | 0x4000);
        Keyboard.release((uint16_t)(*c+40) | 0x4000);
      } else if (*c == '0') {
        Keyboard.press(KEYPAD_0);
        Keyboard.release(KEYPAD_0);
      } else {
        Keyboard.press(*c);
        Keyboard.release(*c);
      }
      c++;
    }
    Keyboard.releaseAll();
  }
}

void c_sleep(int ms) {
  IF_VERBOSE SerialPrintfln("\tSleeping: %i ms", ms);
  delay(ms);
}


// Debug mode functions
void debug_mode(char in_ascii) {
  SerialPrintfln("ASCII: %3i = %c", in_ascii, in_ascii);
}
