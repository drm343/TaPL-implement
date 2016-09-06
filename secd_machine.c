#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 1024

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
  int count = 0;

  while(current != NULL) {
    printf("stop %d\n", count);

    free(current);
    current = NULL;

    if(next != NULL) {
      current = next;
      next = next->next;
    }
    count++;
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

int main(void) {
  init_machine();
  nil();
  ldc(3);
  debug_stack();
  stop_machine();
  return 1;
}
