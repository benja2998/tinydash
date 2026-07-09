build:
	cc -o main main.c -Wextra -Wall -Werror -pedantic -O3 -march=native

install: build
	cp -f "$(CURDIR)/main" /usr/local/bin/tinydash
