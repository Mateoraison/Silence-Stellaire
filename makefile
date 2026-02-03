# ========================
# Projet
# ========================
NAME := silenceStellaire

# Dossiers
SRC_DIR := src
BIN_DIR := bin
INC_DIR := include
LIB_DIR := lib

# Fichiers
SRC := $(SRC_DIR)/main.c
OBJ := $(SRC:.c=.o)
BIN := $(BIN_DIR)/$(NAME)

# Compilateur
CC := gcc

# Flags
CFLAGS := -Wall -Wextra -I$(INC_DIR)
LDFLAGS := -L$(LIB_DIR) -lSDL3 -lSDL3_image -Wl,-rpath,'$$ORIGIN/../lib'

# ========================
# Règles
# ========================
all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(SRC) -o $(BIN) $(CFLAGS) $(LDFLAGS)

run: all
	./$(BIN)

clean:
	rm -f $(BIN)

re: clean all

.PHONY: all run clean re
