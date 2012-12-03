CFLAGS= -g -O3 -Wall  -Wextra -Isrc -rdynamic 
PREFIX?=/usr/local
CC=gcc

ALLOBJS= $(patsubst %.c, %.o, $(wildcard *.c))

default: test

.PHONY: test
test: flextest skiptest dynatest
#	./flextest;
#	./skiptest;
	./dynatest;

flextest: flex_test.o  flex.o
	$(CC) $(CFLAGS) -o  $@  $^

flex_test.o: flex_test.c flex.o minunit.h
	$(CC) $(CFLAGS) -c  $<
flex.o: flex.c flex.h debug.h
	$(CC) $(CFLAGS) -c  $<

skiptest: skip_test.o  skip.o
	$(CC) $(CFLAGS) -o  $@  $^
skip_test.o: skip_test.c skip.o minunit.h
	$(CC) $(CFLAGS) -c  $<
skip.o: skip.c skip.h debug.h
	$(CC) $(CFLAGS) -c  $<

dynatest: dyna_test.o dyna.o
	$(CC) $(CFLAGS) -o  $@  $^
dyna_test.o: dyna_test.c dyna.o minunit.h
	$(CC) $(CFLAGS) -c  $<
dyna.o: dyna.c dyna.h debug.h
	$(CC) $(CFLAGS) -c  $<

# The Cleaner
clean:
	rm -rf build $(ALLOBJS)
	rm -f tests/tests.log 
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

