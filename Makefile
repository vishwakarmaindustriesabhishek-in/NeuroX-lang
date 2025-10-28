# NeuroX-lang Makefile

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -O2
LDFLAGS = -lm -lpthread

# Directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
GEN_DIR = $(BUILD_DIR)/gen

COMPILER_SRC = compiler
RUNTIME_SRC = runtime
EXAMPLES_SRC = examples
TESTS_SRC = tests

# Compiler sources
COMPILER_SRCS = $(wildcard $(COMPILER_SRC)/*.c)
COMPILER_OBJS = $(patsubst $(COMPILER_SRC)/%.c,$(OBJ_DIR)/compiler/%.o,$(COMPILER_SRCS))
CLI_OBJ = $(OBJ_DIR)/tools/neuroxc.o

# Runtime sources
RUNTIME_CORE_SRCS = $(wildcard $(RUNTIME_SRC)/core/*.c)
RUNTIME_HAL_SRCS = $(wildcard $(RUNTIME_SRC)/hal/*.c)
RUNTIME_NET_SRCS = $(wildcard $(RUNTIME_SRC)/net/*.c)

RUNTIME_OBJS = $(patsubst $(RUNTIME_SRC)/%.c,$(OBJ_DIR)/runtime/%.o,$(RUNTIME_CORE_SRCS) $(RUNTIME_HAL_SRCS) $(RUNTIME_NET_SRCS))

# Targets
NEUROXC = $(BIN_DIR)/neuroxc
RUNTIME_LIB = $(BIN_DIR)/libneurox_runtime.a

.PHONY: all clean compiler runtime test examples

all: compiler runtime

# Create directories
$(OBJ_DIR) $(BIN_DIR) $(GEN_DIR):
	mkdir -p $@

$(OBJ_DIR)/compiler $(OBJ_DIR)/runtime/core $(OBJ_DIR)/runtime/hal $(OBJ_DIR)/runtime/net $(OBJ_DIR)/tools:
	mkdir -p $@

# Compiler
compiler: $(NEUROXC)

$(NEUROXC): $(COMPILER_OBJS) $(CLI_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built compiler: $@"

$(OBJ_DIR)/compiler/%.o: $(COMPILER_SRC)/%.c | $(OBJ_DIR)/compiler
	$(CC) $(CFLAGS) -I$(COMPILER_SRC) -c -o $@ $<

# Runtime library
runtime: $(RUNTIME_LIB)

$(RUNTIME_LIB): $(RUNTIME_OBJS) | $(BIN_DIR)
	ar rcs $@ $^
	@echo "Built runtime library: $@"

$(OBJ_DIR)/runtime/core/%.o: $(RUNTIME_SRC)/core/%.c | $(OBJ_DIR)/runtime/core
	$(CC) $(CFLAGS) -I$(RUNTIME_SRC)/core -I$(RUNTIME_SRC)/hal -I$(RUNTIME_SRC)/net -c -o $@ $<

$(OBJ_DIR)/runtime/hal/%.o: $(RUNTIME_SRC)/hal/%.c | $(OBJ_DIR)/runtime/hal
	$(CC) $(CFLAGS) -I$(RUNTIME_SRC)/hal -I$(RUNTIME_SRC)/core -c -o $@ $<

$(OBJ_DIR)/runtime/net/%.o: $(RUNTIME_SRC)/net/%.c | $(OBJ_DIR)/runtime/net
	$(CC) $(CFLAGS) -I$(RUNTIME_SRC)/net -I$(RUNTIME_SRC)/core -c -o $@ $<

# CLI tools
$(OBJ_DIR)/tools/neuroxc.o: tools/neuroxc.c | $(OBJ_DIR)/tools
	$(CC) $(CFLAGS) -I$(COMPILER_SRC) -c -o $@ $<

$(OBJ_DIR)/tools/neurox_cli.o: tools/neurox_cli.c | $(OBJ_DIR)/tools
	$(CC) $(CFLAGS) -I$(COMPILER_SRC) -c -o $@ $<

# Package manager CLI
cli: $(NEUROX_CLI)

$(NEUROX_CLI): $(OBJ_DIR)/tools/neurox_cli.o $(OBJ_DIR)/compiler/common.o | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built package manager: $@"

# Tests
test: compiler runtime
	@echo "Running tests..."
	@$(MAKE) -C $(TESTS_SRC) test

# Examples
examples: compiler runtime
	@echo "Building examples..."
	@for example in $(wildcard $(EXAMPLES_SRC)/*.neuro); do \
		echo "Compiling $$example..."; \
		$(NEUROXC) emit-c $$example -o $(GEN_DIR)/$$(basename $$example .neuro).c; \
	done

# Clean
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build artifacts"

# Help
help:
	@echo "NeuroX-lang Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build compiler and runtime (default)"
	@echo "  compiler  - Build the NeuroX compiler (neuroxc)"
	@echo "  runtime   - Build the runtime library"
	@echo "  test      - Run all tests"
	@echo "  examples  - Build example robots"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"
