#pragma once

#define HWSERIAL Serial1
#include "utils.h"

#define KBD_BUFFSZ 200
#define KEYNAME_BUFFSZ 25
#define PREFIX 17 // CTRL-Q
#define VPREFIX 19 // CTRL-S
#define PEEK_EMPTY 255

// I picked these locations at random
#define EE_MODE 56
#define EE_VERBOSITY 57

// just a way to normalize checks for verbosity mode.
// silent isn't used so far, but it's here for completeness.
#define IF_STANDARD if ( (verbosity && STANDARD) == STANDARD )
#define IF_VERBOSE if (verbosity == VERBOSE)
#define IF_SILENT if (verbosity == SILENT)

// Util functions
int mode_select(char* in_ascii);
int verbose_select(char in_ascii);
uint16_t escape_sequence_to_keycode(char in_ascii);
uint16_t special_char_to_keycode(char in_ascii);
uint16_t keyname_to_keycode(const char* keyname);
void usb_send_key(uint16_t key, uint16_t mod);

// Interactive mode functions
void interactive_mode(char in_ascii);

// Command mode functions
void command_mode(char in_ascii);
void c_parse(char* str);
bool c_parse_ext(char* str, bool send_single, int modifier);
void c_sendraw(char* pch);
void c_send(char* pch);
void c_unicode(char* pch, bool linux);
void c_sleep(int ms);

// Debug mode functions
void debug_mode(char in_ascii);
