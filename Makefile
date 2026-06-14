build:
	cc -o main main.c -Wextra -Wall -Werror -pedantic

install: build
	cp -f "$(CURDIR)/main" /usr/local/bin/tinydash
