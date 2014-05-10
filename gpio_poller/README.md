#----------------Files------------------------------
code2flow.txt = Use this file to create graphics of the flowcharts for the Program
autobuild.sh = build a binary named "io_poller" for your Raspberry Pi.
core.c = c Programm source code for the "io_poller"
config.h = header used in core.c to asign keys, configure timings and more.
#-----------What it is and what it does-------------
This is the sourcecode for a Raspberry Pi GPIO keyborard (x11 required).
The Program will generate keyboards events from GPIO states.
for a rotary encoder two keys can be asigned GPIO Pins will be low active.
The binary "io_poller" will translate the inputs from the GPIO to keyboard events and send these to a X11 display.
The purpose to use this program is to generate commands for a "jivelite" Squeezebox remote.
