#!/bin/bash

make clean
make CC="gcc" \
     CFLAGS="-Wall -O3 -DDEBUG -ggdb"