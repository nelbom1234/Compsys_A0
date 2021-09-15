CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wextra -pedantic -g3

file: file.c Makefile
	$(CC) $(CFLAGS) -o file file.c

../src.zip: file.c Makefile test.sh
	cd .. && zip src.zip src/file.c src/Makefile src/test.sh

clean:
	rm -f *.o
	rm -f file

.PHONY: clean
