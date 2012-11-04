CFLAGS= -g -O1 -Wall -Wextra -Isrc -rdynamic 
PREFIX?=/usr/local
CC=gcc
SKIPDIR=src/

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/DataMonkey # 
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# The Target Build
all: $(SO_TARGET) tests

COMMONDIR=src
COMMON= $(patsubst %.c, %.o, $(wildcard COMMONDIR/*.c))

FLEXDIR=src/flexArray
FLEXOBJS=$(patsubst %.c, %.o, $(wildcard $(FLEXDIR)/*.c)) $(COMMON)
flex: $(FLEXOBJS)
	$(CC) -c $@  $^

flextest: $(patsubst %.c, %.o, $(wildcard FLEXDIR/tests/*.c)) $(FLEXOBJS) $(COMMON)
	$(CC) -o $@  $^
	sh ./tests/teststruct.sh $(FLEXDIR)/tests/*




$(SO_TARGET): CFLAGS += -fPIC
$(SO_TARGET): build $(OBJECTS)
#	ar rcs $@ $(OBJECTS) 
#	ranlib $@

$(SO_TARGET): $(OBJECTS) 
	$(CC) -shared -o $@ $(OBJECTS) 

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(SO_TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log 
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)' $(SOURCES) || true

