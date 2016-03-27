#!/bin/bash

make clean
make CC="gcc" \
     CFLAGS="-Wall -g -DDEBUG" \
     LD="gcc" \
     LDFLAGS="-Wall -g -DDEBUG"