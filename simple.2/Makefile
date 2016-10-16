APP = secd
CODE = src/register_function.c src/secd_machine.c src/secd_debug.c src/secd_memory.c
CC = clang -Wall -g

APP2 = secd2
CODE2 = src/register_function2.c src/secd_machine2.c src/secd_debug2.c src/secd_memory2.c

BUILD_DIR = bin

.PHONY: debug $(APP) clean $(APP2)
$(APP): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) $(CODE)

$(APP2): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP2) $(CODE2)

debug: $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) -DDEBUG=1 $(CODE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	cd ../simple && rm -r ./bin/*
