#pragma once

#include <Print.h>

#ifndef HWSERIAL
#define HWSERIAL Serial
#endif

#define SerialPrintf(fmt, ...) SerialPrintfOut(HWSERIAL, 0, fmt, ##__VA_ARGS__)
#define SerialPrintfln(fmt, ...) SerialPrintfOut(HWSERIAL, 1, fmt, ##__VA_ARGS__)
void SerialPrintfOut(Print &output, bool newline, const char* fmt, ...);

#define SerialClear() SerialClearOut(HWSERIAL)
void SerialClearOut(Print &output);

#define SerialDeleteChars(len) SerialDeleteCharsOut(HWSERIAL, len)
void SerialDeleteCharsOut(Print &output, int len);

#define SerialClearLine() SerialClearLineOut(HWSERIAL)
void SerialClearLineOut(Print &output);

#define SerialAnsiEsc(esc) SerialAnsiEscOut(HWSERIAL, esc)
void SerialAnsiEscOut(Print &output, const char* seq);

// just a way to normalize checks for verbosity mode.
// silent isn't used so far, but it's here for completeness.
#define IF_STANDARD if ( (verbosity && STANDARD) == STANDARD )
#define IF_VERBOSE if (verbosity == VERBOSE)
#define IF_SILENT if (verbosity == SILENT)

// Compile time bernstein hash function
constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}
