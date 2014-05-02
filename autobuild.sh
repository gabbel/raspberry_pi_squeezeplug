#! /bin/sh
gcc -std=c99 -o io_poller core.c -l bcm2835 -lX11 -lXtst -O3



