APP = secd
CODE = src/register_function.c src/secd_machine.c
CC = clang -Wall -g

BUILD_DIR = bin

.PHONY: debug $(APP) clean
$(APP): $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) $(CODE)

debug: $(BUILD_DIR)
	$(CC) -o $(BUILD_DIR)/$(APP) -pedantic -DDEBUG=1 $(CODE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	cd ../simple && rm -r ./bin/*
