CC = gcc
CFLAGS = -Wall -Wextra -g
BIN_DIR = bin
SRC = common.c
INCLUDES = common.h

all: $(BIN_DIR)/server $(BIN_DIR)/client

$(BIN_DIR)/server: server.c $(SRC) $(INCLUDES) | $(BIN_DIR)
	$(CC) $(CFLAGS) server.c $(SRC) -o $(BIN_DIR)/server

$(BIN_DIR)/client: client.c $(SRC) $(INCLUDES) | $(BIN_DIR)
	$(CC) $(CFLAGS) client.c $(SRC) -o $(BIN_DIR)/client

.PHONY: all
