CFLAGS :=

build: main.c format
	clang -o main *.c -Wextra -Wall -Werror -pedantic -O3 -march=native $(CFLAGS) -fsanitize=address
	./gen_tags

format:
	clang-format -i *.c
	clang-format -i *.h

install: main
	cp -f "$(CURDIR)/main" /usr/local/bin/tinydash
