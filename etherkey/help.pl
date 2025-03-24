#!usr/bin/perl

while (<DATA>) {
	chomp;
	print "\t\t\"$_\\n\\r\"\n";
}


__END__
Etherkey is a serial controlled keyboard.

MODES: Change via ^Q
  Interactive Mode -- all text is sent straight to target system
  Command Mode     -- Send complex strings with modifiers via commands
  Debug Mode       -- Show received characters from serial, no sending

VERBOSITY: Changed via ^S
  Silent   -- Minimal information, interactive is silent
  Standard -- Standard information, interactive shows what is sent
  Verbose  -- Too much information

COMMAND MODE:
  A single char literal can be enclosed in braces. If integer i is 
  included after the char or keyname, it repeats that keypress i times.

  Commands:
    sendraw -- Send the following text literally
    send    -- Send an interpreted string

  Modifiers: Prepended to a char to modify them, as in ^C for Ctrl-C
    ! ALT          + SHIFT          ^ CTRL          # WIN

  Keynames: Send non-printable characters: (case indifferent)
    {Enter}             {Escape}           {Space}            {Tab}
    {Backspace}/{BS}    {Delete}/{Del}     {Insert}/{Ins}     {Up}
    {Down}              {Left}             {Right}            {Home}
    {End}               {PgUp}             {PgDn}             {Windows}/{Win}
    {F1}..{F12}
  These keypresses can also be entered as a singular command.

There's other stuff too, see the docs.
