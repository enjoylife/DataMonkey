CFLAGS= -g -O1 -Wall -Wextra -Isrc -rdynamic 
PREFIX?=/usr/local
CC=gcc


ALLOBJS= $(patsubst %.c, %.o, $(wildcard *.c))


flextest: flex_test.o  flex.o
	$(CC) -g -o $@  $^

flex_test.o: flex_test.c flex.o minunit.h
	$(CC) -c $<

flex.o: flex.c flex.h dbg.h
	$(CC) -c $<



# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

