CFLAGS :=
INSTDIR ?= /usr/local/bin

build: main.c format
	clang -o main *.c -Wextra -Wall -Werror -pedantic -O3 -march=native $(CFLAGS) -fsanitize=address
	./gen_tags

format:
	clang-format -i *.c
	clang-format -i *.h

install: main
	ln -sf "$(CURDIR)/main" $(INSTDIR)/tinydash
