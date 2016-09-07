APP = secd
SOURCE = secd_machine.c
CC = gcc

.PHONY: $(APP)
$(APP):
	$(CC) -o $(APP) $(SOURCE)
