#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 10
#define ESC 27
#define ESCAPE 34
#define SPACE (int)' '

// machine type
struct BaseCell;

struct BaseList {
  struct BaseCell *car;
  struct BaseCell *cdr;
};

enum Type {
  NIL = 0,
  INTEGER,
  ATOM,
  LIST
};

union Content {
  int64_t integer;
  struct BaseList *list;
  char *string;
};

struct BaseCell {
  enum Type type;
  union Content content;
  struct BaseCell *next;
};

struct SECD {
  struct BaseCell *s;
  struct BaseCell *bottom;
  char *c;
};

struct SECD secd_machine;

// interpreter type
enum StopType {
  CONTINUE = 0,
  FIND,
  TOO_MORE
};

void compile_and_run(enum StopType);
void store_code(int16_t, char *);

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

struct BaseCell *push_atom(char *atom_string) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = ATOM;
  cell->content.string = atom_string;
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

void atom(char *atom_string) {
  struct BaseCell *cell = push_atom(atom_string);
  set_stack_next(cell);
}

// machine command
void init_machine(void) {
  struct BaseCell *cell = push_nil();
  secd_machine.s = cell;
  secd_machine.bottom  = cell;
  secd_machine.c = (char*)malloc(sizeof(char) * CMD_BUFFER_SIZE);
}

void stop_machine(void) {
  struct BaseCell *current = secd_machine.bottom;
  struct BaseCell *next = secd_machine.bottom->next;

  while(current != NULL) {
    if(current->type == ATOM) {
      free(current->content.string);
    }

    free(current);
    current = NULL;

    if(next != NULL) {
      current = next;
      next = next->next;
    }
  }
  free(secd_machine.c);
}

void debug_stack(void) {
  if(secd_machine.s->type == NIL) {
    printf("top is nil\n");
  }
  else if(secd_machine.s->type == INTEGER) {
    printf("top is integer %d\n", secd_machine.s->content.integer);
  }
  else if(secd_machine.s->type == ATOM) {
    printf("top is atom %s\n", secd_machine.s->content.string);
  }
  else {
    printf("top is other\n");
  }
}

void run(void) {
  enum StopType status = CONTINUE;
  int8_t next = 0;
  int16_t not_stop = 1;
  char command[CMD_BUFFER_SIZE];
  char exit_command[5];
  memset(command, 0, CMD_BUFFER_SIZE);
  memset(exit_command, 0, 5);

  while(not_stop) {
    if(status != TOO_MORE) {
      printf(")> ");
    }
    status = CONTINUE;

    /* Required on Windows. */
    fflush(stdout);
    fgets(command, CMD_BUFFER_SIZE, stdin); 

    next = 0;
    strncpy(exit_command, command, 4);

    if(!strcmp(exit_command, "exit")) {
      not_stop = 0;
      continue;
    }

    while(status == CONTINUE) {
      if(next >= CMD_BUFFER_SIZE) {
        status = TOO_MORE;
        break;
      }
      else if(command[next] == '\n') {
        command[next] = ' ';
        status = FIND;
        break;
      }
      next++;
    }
    store_code(next, command);
    compile_and_run(status);
  }
}

void store_code(int16_t input_size, char *command) {
  memset(secd_machine.c, ' ', CMD_BUFFER_SIZE);
  strncpy(secd_machine.c, command, input_size);
}

char *new_string(int16_t input_size) {
  if (input_size < 10) {
    return (char *)malloc(sizeof(char) * input_size);
  }
  else {
    return (char *)malloc(sizeof(char) * 10);
  }
}

void normal_mode(int16_t next) {
  char *command = &(secd_machine.c[next]);
  char *atom_string = NULL;
  int16_t counter = 0;

  while(next < CMD_BUFFER_SIZE) {
    if(((int)secd_machine.c[next]) == SPACE) {
      if(counter >= 1) {
        atom_string = new_string(counter);
        strncpy(atom_string, command, counter);
        atom(atom_string);
      }
      command += counter + 1;
      counter = 0;
      next++;
    }
    else {
      next++;
      counter++;
    }
  }
}

void compile_and_run(enum StopType status) {
  normal_mode(0);
}

int main(void) {
  init_machine();
  run();
  debug_stack();
  stop_machine();
  return 1;
}
