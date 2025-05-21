# apartman-sim/Makefile

CC      ?= gcc
CFLAGS  := -std=c17 -Wall -Wextra -pedantic -pthread
SRC_DIR := src
OBJ_DIR := build

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LDFLAGS := -pthread -lrt
else
    LDFLAGS := -pthread
endif

TARGET := $(OBJ_DIR)/sim

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) | $(OBJ_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR)
