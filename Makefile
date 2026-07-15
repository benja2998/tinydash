CFLAGS := 

build:
	cc -o main *.c -Wextra -Wall -Werror -pedantic -O3 -march=native $(CFLAGS)

install: build
	cp -f "$(CURDIR)/main" /usr/local/bin/tinydash
