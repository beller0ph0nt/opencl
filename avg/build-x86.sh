#!/bin/bash

make clean
make CC="gcc" \
     CFLAGS="-Wall -O3 -DDEBUG" \
     LD="gcc" \
     LDFLAGS="-Wall -O3 -DDEBUG"