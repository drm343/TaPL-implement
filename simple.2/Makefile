APP = secd
CODE = src/register_function.c src/secd_machine.c src/secd_debug.c src/secd_memory.c src/secd_type.c
CC = clang -Wall -g
#CC = gcc -Wall -g

BUILD_DIR = bin

.PHONY: debug $(APP) clean
$(APP): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) $(CODE)

debug: $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) -DDEBUG=1 $(CODE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	cd ../simple && rm -r ./bin/*
