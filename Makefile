CC=gcc

LIBS=sdl2 SDL2_image SDL2_ttf

CFLAGS=-Wall -Wextra -pedantic -std=c17
CFLAGS+=`pkgconf --cflags $(LIBS)` 
LDFLAGS=`pkgconf --libs $(LIBS)`

SRC_DIR=src
HDR_DIR=src 
OBJ_DIR=obj

BIN_DIR=bin

SRC_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

TARGET=nesemu2

ifeq ($(OS),Windows_NT)
	TARGET_DEBUG := $(BIN_DIR)/$(TARGET)_DEBUG.exe
	TARGET_RELEASE := $(BIN_DIR)/$(TARGET).exe
else
	TARGET_DEBUG := $(BIN_DIR)/$(TARGET)_DEBUG
	TARGET_RELEASE := $(BIN_DIR)/$(TARGET)
endif


.PHONY: debug release clean

all: debug release

<target>: <dependencies>
	<action>

debug: CFLAGS += -g3 -O0
debug: $(BIN_DIR) $(OBJ_DIR) $(TARGET_DEBUG)

release: CFLAGS += -O2 -DNDEBUG
release: $(BIN_DIR) $(OBJ_DIR) $(TARGET_RELEASE)

$(TARGET_DEBUG): $(OBJ_FILES) 
	$(CC) $(CFLAGS) $^ -I$(HDR_DIR) -o $@ $(LDFLAGS)

$(TARGET_RELEASE): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -I$(HDR_DIR) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CC_FLAGS) -c $^ -I$(HDR_DIR) -o $@

$(OBJ_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

clean: 
	rm -rf $(OBJ_DIR) $(BIN_DIR)
