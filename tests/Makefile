# Tests Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -I..
LDFLAGS = -lm

COMPILER_OBJS = ../build/obj/compiler/common.o \
                ../build/obj/compiler/lexer.o \
                ../build/obj/compiler/parser.o \
                ../build/obj/compiler/ast.o

TEST_SRCS = test_lexer.c test_parser.c
TEST_BINS = $(TEST_SRCS:.c=)

.PHONY: all test clean

all: $(TEST_BINS)

test_lexer: test_lexer.c $(COMPILER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_parser: test_parser.c $(COMPILER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_BINS)
	@echo "Running tests..."
	@./test_lexer
	@./test_parser
	@echo ""
	@echo "✓ All tests passed!"

clean:
	rm -f $(TEST_BINS)
