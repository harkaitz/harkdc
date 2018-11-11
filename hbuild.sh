#!/bin/sh
hpack -v 2.0 -t x86_64-linux-gnu -t x86_64-w64-mingw32 -s -d dist -P .
hpack -v 2.0 -t win64 -p / -i bin/* -i README.txt -d dist -P . -F README.md=README.txt
