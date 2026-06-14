build:
	cc -o main main.c -Wextra -Wall -Werror -pedantic

install: build
	ln -sf "$(CURDIR)/main" /usr/local/bin/tinydash
