#! /bin/sh
# Version: v0.1 Date: 2014-05-13

# Use gcc to build the "io_poller" binary using core.c the bcm2835 library, X11 library and X11test library. Build with optimized settings.
gcc -std=c99 -o io_poller core.c -l bcm2835 -lX11 -lXtst -O3



