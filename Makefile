APP = secd
CODE = src/register_function.c src/secd_machine.c
CC = clang -Wall -g

BUILD_DIR = bin

.PHONY: $(APP)
$(APP): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) $(CODE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
