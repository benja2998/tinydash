# tinydash

tinydash is a tiny terminal-based game similar to "Geometry Dash". It uses only standard POSIX libraries and can practically run on any system.

## Usage

Build it with make, then run ./main. To play, press space to jump before the number symbols hit your character, the @ symbol.

## Installing

Simply run make install. You need to be root.

## Configuration

You can increase the number of enemies to make it harder for yourself by compiling like this:
```bash
# $X is what you want the number of enemies to be
make CFLAGS=-DNUM_ENM=$X
```
