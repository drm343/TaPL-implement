#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define CMD_BUFFER_SIZE 1024
#define ESC 27
#define ESCAPE 34
#define SPACE (int)' '
#define ADD_FUNCTION(name, func) add_function(name, sizeof(name) - 1, func);

//#define DEBUG 1

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
  FUNC,
  LIST
};

union Content {
  int64_t integer;
  struct BaseList *list;
  char *string;
  void (*func)(void);
};

struct BaseCell {
  enum Type type;
  union Content content;
  struct BaseCell *next;
};

struct SECD {
  struct BaseCell *s;
  struct BaseCell *bottom;
  struct BaseCell *c;
  struct BaseCell *c_bottom;
  char *tmp_code;
  struct BaseCell *env;
  struct BaseCell *env_bottom;

  struct BaseCell *atom_pool;
  struct BaseCell *atom_pool_top;

  struct BaseCell *integer_pool;
  struct BaseCell *integer_pool_top;

  struct BaseCell *list_pool;
  struct BaseCell *list_pool_top;
};

struct SECD secd_machine;

// interpreter type
enum StopType {
  CONTINUE = 0,
  FIND,
  TOO_MORE
};

void compile_code(int16_t, char *);
void run_code(void);
void free_list(struct BaseList *);
void free_stack(struct BaseCell *);

// kernel command
struct BaseCell *new_nil(void) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = NIL;
  cell->content.integer = 0;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_interger(int64_t integer) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = INTEGER;
  cell->content.integer = integer;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_atom(char *atom_string) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = ATOM;
  cell->content.string = atom_string;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_function(void (*func)(void)) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = FUNC;
  cell->content.func = func;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_list(struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  struct BaseList *pair = (struct BaseList*)malloc(sizeof(struct BaseList));
  pair->car = car;
  pair->cdr = cdr;

  cell->type = LIST;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

void clean_code(void) {
  secd_machine.c_bottom->next = NULL;
  secd_machine.c = secd_machine.c_bottom;
}

void set_stack_next(struct BaseCell *cell) {
  secd_machine.s->next = cell;
  secd_machine.s = cell;
}

void set_code_next(struct BaseCell *cell) {
  secd_machine.c->next = cell;
  secd_machine.c = cell;
}

void set_env_next(struct BaseCell *cell) {
  secd_machine.env->next = cell;
  secd_machine.env = cell;
}

void drop_atom(struct BaseCell *cell) {
  printf("[drop atom]%s\n", cell->content.string);
  if(secd_machine.atom_pool == NULL) {
    secd_machine.atom_pool = cell;
    secd_machine.atom_pool_top = cell;
  }
  else {
    secd_machine.atom_pool_top->next = cell;
    secd_machine.atom_pool_top = cell;
  }
}

void drop_integer(struct BaseCell *cell) {
  printf("[drop integer]%s\n", cell->content.integer);
  if(secd_machine.integer_pool == NULL) {
    secd_machine.integer_pool = cell;
    secd_machine.integer_pool_top = cell;
  }
  else {
    secd_machine.integer_pool_top->next = cell;
    secd_machine.integer_pool_top = cell;
  }
}

void drop_list(struct BaseCell *cell) {
  printf("[drop list]%s\n", cell->content.list->car->content.string);
  if(secd_machine.list_pool == NULL) {
    secd_machine.list_pool = cell;
    secd_machine.list_pool_top = cell;
  }
  else {
    secd_machine.list_pool_top->next = cell;
    secd_machine.list_pool_top = cell;
  }
}

// user command
void ldc(int64_t integer) {
  struct BaseCell *cell = new_interger(integer);
  set_stack_next(cell);
}

void compile_atom(char *atom_string) {
  struct BaseCell *cell = new_atom(atom_string);
  set_code_next(cell);
}

void compile_function(char *atom_string, void (*func)(void)) {
  struct BaseCell *name_cell = new_atom(atom_string);
  struct BaseCell *func_cell = new_function(func);
  struct BaseCell *cell = new_list(name_cell, func_cell);
  set_env_next(cell);
}

// primitive code
void hello(void) {
  printf("hello\n");
}

void nil(void) {
  struct BaseCell *cell = new_nil();
  set_stack_next(cell);
}

// machine command
void init_machine(void) {
  struct BaseCell *cell = new_nil();
  secd_machine.s = cell;
  secd_machine.bottom  = cell;

  cell = new_nil();
  secd_machine.c = cell;
  secd_machine.c_bottom = cell;
  secd_machine.tmp_code = (char*)malloc(sizeof(char) * CMD_BUFFER_SIZE);

  cell = new_nil();
  secd_machine.env = cell;
  secd_machine.env_bottom = cell;

  secd_machine.atom_pool = NULL;
  secd_machine.atom_pool_top = NULL;

  secd_machine.integer_pool = NULL;
  secd_machine.integer_pool_top = NULL;

  secd_machine.list_pool = NULL;
  secd_machine.list_pool_top = NULL;
}

void free_list(struct BaseList *current) {
  struct BaseCell *car = current->car;
  struct BaseCell *cdr = current->cdr;

  free(current);
  free_stack(car);
  free_stack(cdr);
}

void free_stack(struct BaseCell *current) {
  struct BaseCell *next = NULL;

  if(current != NULL) {
    current->next;
  }

  while(current != NULL) {
    if(current->type == ATOM) {
      free(current->content.string);
    }
    else if(current->type == LIST) {
      free_list(current->content.list);
    }

    free(current);
    current = NULL;

    if(next != NULL) {
      current = next;
      next = next->next;
    }
  }
}

void stop_machine(void) {
  free_stack(secd_machine.bottom);
  free_stack(secd_machine.c_bottom);
  free_stack(secd_machine.env_bottom);

  free_stack(secd_machine.atom_pool);
  free_stack(secd_machine.integer_pool);
  free_stack(secd_machine.list_pool);

  free(secd_machine.tmp_code);
}

void debug_stack(void) {
#ifdef DEBUG
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
#endif
}

void debug_code(void) {
#ifdef DEBUG
  if(secd_machine.c->type == NIL) {
    printf("top is nil\n");
  }
  else if(secd_machine.c->type == INTEGER) {
    printf("top is integer %d\n", secd_machine.c->content.integer);
  }
  else if(secd_machine.c->type == ATOM) {
    printf("top is atom %s\n", secd_machine.c->content.string);
  }
  else {
    printf("top is other\n");
  }
#endif
}

void debug_env(void) {
#ifdef DEBUG
  struct BaseList *list = secd_machine.env->content.list;
  struct BaseCell *car = list->car;
  struct BaseCell *cdr = list->cdr;

  printf("func %s\n", car->content.string);
  printf("func %p\n", cdr->content.func);

  if(cdr->content.func == NULL) {
    printf("[error]: not func\n");
  }
  else {
    cdr->content.func();
  }
#endif
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
    compile_code(next, command);
    run_code();
  }
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
  char *command = &(secd_machine.tmp_code[next]);
  char *atom_string = NULL;
  int16_t counter = 0;

  while(next < CMD_BUFFER_SIZE) {
    if(((int)secd_machine.tmp_code[next]) == SPACE) {
      if(counter >= 1) {
        atom_string = new_string(counter);
        strncpy(atom_string, command, counter);
        compile_atom(atom_string);
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

void compile_code(int16_t input_size, char *command) {
  memset(secd_machine.tmp_code, ' ', CMD_BUFFER_SIZE);
  strncpy(secd_machine.tmp_code, command, input_size);

  normal_mode(0);
}

void add_function(char *name, int16_t name_size, void (*func)(void)) {
  char *atom_string = new_string(name_size);

  strncpy(atom_string, name, name_size);
  compile_function(atom_string, func);
}

void register_function(void) {
  ADD_FUNCTION("nil", nil);
  ADD_FUNCTION("hello", hello);
}

void find_function(char *func) {
  struct BaseCell *current = secd_machine.env_bottom->next;
  struct BaseList *item = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;

  while(current != NULL) {
    item = current->content.list;
    car = item->car;
    cdr = item->cdr;
    
    if(!strcmp(car->content.string, func)) {
      cdr->content.func();
      break;
    }

    current = current->next;
  }
}

void run_code(void) {
  struct BaseCell *current = secd_machine.c_bottom->next;
  struct BaseCell *next = NULL;

  while(current != NULL) {
    next = current->next;
    current->next = NULL;

    if(current->type == ATOM) {
      find_function(current->content.string);
      drop_atom(current);
    }
    else if(current->type == NIL) {
      printf("[Error]:nil\n");
      drop_integer(current);
    }
    else if(current->type == INTEGER) {
      printf("[Error]:%s is integer\n", current->content.integer);
      drop_integer(current);
    }
    else if(current->type == LIST) {
      printf("[Error]:list\n");
      drop_list(current);
    }
    current = next;
  }

  clean_code();
}

int main(void) {
  init_machine();
  register_function();
  run();
  debug_stack();
  stop_machine();
  return 1;
}
