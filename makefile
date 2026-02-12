# ========================
# Projet
# ========================
NAME := silenceStellaire

# ========================
# Dossiers
# ========================
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
INC_DIR := include
HDR_DIR := src/headers
LIB_DIR := lib

# ========================
# Fichiers
# ========================
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
BIN := $(BIN_DIR)/$(NAME)

# ========================
# Compilateur
# ========================
CC := gcc

# ========================
# Flags
# ========================
CFLAGS := -Wall -Wextra -I$(INC_DIR) -I$(HDR_DIR)
LDFLAGS := -L$(LIB_DIR) -lSDL3 -lSDL3_image -lSDL3_ttf -lm -Wl,-rpath,'$$ORIGIN/../lib'

# ========================
# Règles
# ========================
all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(BIN)

clean:
	rm -rf $(OBJ_DIR) $(BIN)

re: clean all

.PHONY: all run clean re
