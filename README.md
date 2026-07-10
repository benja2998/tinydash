# tinydash

tinydash is a tiny terminal-based game similar to "Geometry Dash". It uses only standard POSIX libraries and can practically run on any system.

## Usage

Build it with make, then run ./main. To play, press space to jump before the number symbols hit your character, the @ symbol.

## Installing

Simply run make install. You need to be root.

## Configuration

You can optionally add a third enemy, using make `CFLAGS=-DTHIRD_ENM`.
