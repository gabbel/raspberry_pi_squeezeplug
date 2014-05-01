#! /bin/sh
gcc -std=c99 -o io_poller config.c core.c -l bcm2835 -lX11 -lXtst -O2



