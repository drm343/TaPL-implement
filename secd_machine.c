#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 1024
#define ESC 27
#define ESCAPE 34

struct BaseCell;

struct BaseList {
  struct BaseCell *car;
  struct BaseCell *cdr;
};

enum Type {
  NIL = 0,
  INTEGER,
  LIST
};

union Content {
  int64_t integer;
  struct BaseList *list;
};

struct BaseCell {
  enum Type type;
  union Content content;
  struct BaseCell *next;
};

struct SECD {
  struct BaseCell *s;
  struct BaseCell *bottom;
};

struct SECD secd_machine;

// kernel command
struct BaseCell *push_nil(void) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = NIL;
  cell->content.integer = 0;
  cell->next = NULL;
  return cell;
}

struct BaseCell *push_interger(int64_t integer) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = INTEGER;
  cell->content.integer = integer;
  cell->next = NULL;
  return cell;
}

void set_stack_next(struct BaseCell *cell) {
  secd_machine.s->next = cell;
  secd_machine.s = cell;
}

// user command
void nil(void) {
  struct BaseCell *cell = push_nil();
  set_stack_next(cell);
}

void ldc(int64_t integer) {
  struct BaseCell *cell = push_interger(integer);
  set_stack_next(cell);
}

// machine command
void init_machine(void) {
  struct BaseCell *cell = push_nil();
  secd_machine.s = cell;
  secd_machine.bottom  = cell;
}

void stop_machine(void) {
  struct BaseCell *current = secd_machine.bottom;
  struct BaseCell *next = secd_machine.bottom->next;

  while(current != NULL) {
    free(current);
    current = NULL;

    if(next != NULL) {
      current = next;
      next = next->next;
    }
  }
}

void debug_stack(void) {
  if(secd_machine.s->type == NIL) {
    printf("top is nil\n");
  }
  else if(secd_machine.s->type == INTEGER) {
    printf("top is integer %d\n", secd_machine.s->content.integer);
  }
  else {
    printf("top is other\n");
  }
}

void run(void) {
  int not_stop = 1;
  char command[CMD_BUFFER_SIZE];
  memset(command, 0, CMD_BUFFER_SIZE);

  while(not_stop) {
    printf(")> ");
    /* Required on Windows. */
    fflush(stdout);
    fgets(command, CMD_BUFFER_SIZE, stdin); 

    if(command[0] == ESC) {
      // Up   27 91 65
      // Down 27 91 66
      printf("\n");
    }
    else if(command[0] != '\n') {
      printf("cmd: %s ok\n", command);
    }
    else {
      not_stop = 0;
      printf("done\n");
    }
  }
}

int main(void) {
  init_machine();
  run();
  printf("%d\n", '\"');
  stop_machine();
  return 1;
}
