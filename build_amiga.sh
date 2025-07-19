#!/bin/bash
CC=m68k-amigaos-gcc     \
NM=m68k-amigaos-nm      \
AR=m68k-amigaos-ar      \
CFLAGS="-noixemul -m68020 -mregparm=4 -Ofast -fomit-frame-pointer -flto -ffunction-sections -fdata-sections"  \
LDFLAGS="-Wl,--gc-sections -lm" \
make

m68k-amigaos-strip xxhsum
