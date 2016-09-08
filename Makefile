APP = secd
CODE = secd_machine.c
CC = gcc

BUILD_DIR = bin
SOURCE_DIR = src

.PHONY: $(APP)
$(APP): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) $(SOURCE_DIR)/$(CODE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
