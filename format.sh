#!/bin/sh
find -type f -name "*.[ch]" -exec clang-format -i {} \;
