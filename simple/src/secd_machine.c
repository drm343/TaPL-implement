#include "secd_struct.h"
#include "secd_machine.h"

#define CMD_BUFFER_SIZE 1024
#define ESC 27
#define ESCAPE 34
#define SPACE (int)' '
#define ASCII_0 48
#define ASCII_9 57


#define DEBUG 1
struct SECD secd_machine;

void compile_code(int16_t, char *);
void run_code(void);
void free_list(struct BaseList *);
void free_stack(struct BaseCell *);

// kernel command
char *new_string(int16_t input_size) {
  if (input_size > 0) {
    char * result = (char *)malloc(sizeof(char) * input_size);
    if(result == NULL) {
      return result;
    }
    else {
      memset(result, 0, input_size);
      return result;
    }
  }
  else {
    return NULL;
  }
}

struct BaseCell *new_nil(void) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = NIL;
  cell->content.integer = 0;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_integer(int64_t integer) {
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

struct BaseCell *new_type(char *atom_string) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = TYPE;
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
  secd_machine.c = secd_machine.c_bottom;
}

void set_stack_next(struct BaseCell *cell) {
  if(secd_machine.bottom == NULL) {
    secd_machine.bottom = cell;
    secd_machine.s = cell;
  }
  else {
    cell->next = secd_machine.s;
    secd_machine.s = cell;
  }
}

void set_code_next(struct BaseCell *cell) {
  if(secd_machine.c == NULL) {
    secd_machine.c = cell;
  }

  if(secd_machine.c_bottom != NULL) {
    secd_machine.c_bottom->next = cell;
  }
  secd_machine.c_bottom = cell;
}

void set_env_next(struct BaseCell *cell) {
  if(secd_machine.env_bottom == NULL) {
    secd_machine.env_bottom = cell;
  }

  if(secd_machine.env == NULL) {
    secd_machine.env = cell;
  }
  else {
    cell->next = secd_machine.env;
    secd_machine.env = cell;
  }
}

struct BaseCell *pop_code_next(void) {
  struct BaseCell *cell = secd_machine.c;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine.c_bottom = NULL;
    }

    secd_machine.c = cell->next;
    cell->next = NULL;
  }
  return cell;
}

struct BaseCell *pop_stack_next(void) {
  struct BaseCell *cell = secd_machine.s;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine.s = NULL;
      secd_machine.bottom  = NULL;
    }
    else {
      secd_machine.s = cell->next;
    }
    cell->next = NULL;
  }
  return cell;
}

void drop_atom(struct BaseCell *cell) {
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
  if(secd_machine.list_pool == NULL) {
    secd_machine.list_pool = cell;
    secd_machine.list_pool_top = cell;
  }
  else {
    secd_machine.list_pool_top->next = cell;
    secd_machine.list_pool_top = cell;
  }
}

void drop_cell(struct BaseCell *cell) {
  if(cell->type == ATOM) {
    drop_atom(cell);
  }
  else if(cell->type == NIL) {
    drop_integer(cell);
  }
  else if(cell->type == INTEGER) {
    drop_integer(cell);
  }
  else if(cell->type == LIST) {
    drop_list(cell);
  }
}

void compile_atom(char *atom_string) {
  struct BaseCell *cell = new_atom(atom_string);
  set_code_next(cell);
}

bool compile_integer(char *atom_string, int16_t atom_size) {
  int64_t count = 0;

  for(int16_t i = 0; i < atom_size; i++) {
    if((atom_string[i] >= ASCII_0) && (atom_string[i] <= ASCII_9)) {
      count = count * 10 + (atom_string[i] - ASCII_0);
    }
    else {
      return false;
    }
  }

  struct BaseCell *cell = new_integer(count);
  set_code_next(cell);
  return true;
}

void compile_function(char *atom_string, void (*func)(void)) {
  struct BaseCell *name_cell = new_atom(atom_string);
  struct BaseCell *func_cell = new_function(func);
  struct BaseCell *cell = new_list(name_cell, func_cell);
  set_env_next(cell);
}

void compile_primitive(char *func_string, void (*func)(void)) {
}

// user command
void ldc(int64_t integer) {
  struct BaseCell *cell = new_integer(integer);
  set_stack_next(cell);
}

// machine command
void init_machine(void) {
  secd_machine.s = NULL;
  secd_machine.bottom = NULL;

  secd_machine.c_bottom = NULL;
  secd_machine.c = NULL;
  secd_machine.tmp_code = (char*)malloc(sizeof(char) * CMD_BUFFER_SIZE);

  secd_machine.env = NULL;
  secd_machine.env_bottom = NULL;

  secd_machine.atom_pool = NULL;
  secd_machine.atom_pool_top = NULL;

  secd_machine.integer_pool = NULL;
  secd_machine.integer_pool_top = NULL;

  secd_machine.list_pool = NULL;
  secd_machine.list_pool_top = NULL;

  secd_machine.status = SECD_STATUS_NS(CONTINUE);
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
    next = current->next;
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

void SECD_MACHINE_NS(error)(char *msg) {
  printf("%s\n", msg);
  secd_machine.status = SECD_STATUS_NS(ERROR);

  clean_code();
}

void SECD_MACHINE_NS(recover_machine)(void) {
  secd_machine.status = SECD_STATUS_NS(CONTINUE);
}

void stop_machine(void) {
  free_stack(secd_machine.bottom);
  free_stack(secd_machine.c);
  free_stack(secd_machine.env);

  free_stack(secd_machine.atom_pool);
  free_stack(secd_machine.integer_pool);
  free_stack(secd_machine.list_pool);

  free(secd_machine.tmp_code);
}

void debug_code(void) {
#ifdef DEBUG
  struct BaseCell *cell = secd_machine.c;

  printf("[all code]: ");
  while(cell != NULL) {
    if(cell->type == NIL) {
      printf("nil:nil ");
    }
    else if(cell->type == INTEGER) {
      printf("%" PRId64 ":integer ", cell->content.integer);
    }
    else if(cell->type == ATOM) {
      printf("%s:atom ", cell->content.string);
    }
    else {
      printf("undef:bottom ");
    }

    cell = cell->next;
  }
  printf("\n");
#endif
}

void debug_stack(void) {
#ifdef DEBUG
  struct BaseCell *cell = secd_machine.s;

  printf("[all stack]: ");
  while(cell != NULL) {
    if(cell->type == NIL) {
      printf("nil:nil ");
    }
    else if(cell->type == INTEGER) {
      printf("%" PRId64 ":integer ", cell->content.integer);
    }
    else if(cell->type == ATOM) {
      printf("%s:atom ", cell->content.string);
    }
    else {
      printf("undef:bottom ");
    }

    cell = cell->next;
  }
  printf("\n");
#endif
}

void debug_top_code(void) {
#ifdef DEBUG
  struct BaseCell *cell = secd_machine.c;

  printf("[top code]: ");
  if(cell == NULL) {
    printf("null");
  }
  else if(cell->type == NIL) {
    printf("nil:nil ");
  }
  else if(cell->type == INTEGER) {
    printf("%" PRId64 ":integer ", cell->content.integer);
  }
  else if(cell->type == ATOM) {
    printf("%s:atom ", cell->content.string);
  }
  else {
    printf("undef:bottom ");
  }
  printf("\n");
#endif
}

void debug_top_stack(void) {
#ifdef DEBUG
  if(secd_machine.s->type == NIL) {
    printf("top is nil\n");
  }
  else if(secd_machine.s->type == INTEGER) {
    printf("top is integer %" PRId64 "\n", secd_machine.s->content.integer);
  }
  else if(secd_machine.s->type == ATOM) {
    printf("top is atom %s\n", secd_machine.s->content.string);
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
  int16_t next = 0;
  char command[CMD_BUFFER_SIZE];
  char exit_command[5];
  memset(command, 0, CMD_BUFFER_SIZE);
  memset(exit_command, 0, 5);

  while(secd_machine.status) {
    if(status != TOO_MORE) {
      printf(")> ");
    }
    SECD_MACHINE_NS(recover_machine)();
    status = CONTINUE;

    /* Required on Windows. */
    fflush(stdout);
    fgets(command, CMD_BUFFER_SIZE, stdin); 

    next = 0;
    strncpy(exit_command, command, 4);

    if(!strcmp(exit_command, "exit")) {
      secd_machine.status = SECD_STATUS_NS(STOP);
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

void compile_code(int16_t input_size, char *command) {
  memset(secd_machine.tmp_code, ' ', CMD_BUFFER_SIZE);
  strncpy(secd_machine.tmp_code, command, input_size);

  char *atom_string = NULL;
  int16_t counter = 0;
  int16_t next = 0;

  while(next < CMD_BUFFER_SIZE) {
    if(((int)secd_machine.tmp_code[next]) == SPACE) {
      if(counter >= 1) {
        atom_string = new_string(counter + 1);
        strncpy(atom_string, command, counter);

        if (!compile_integer(atom_string, counter)) {
          compile_atom(atom_string);
        }
        else {
          free(atom_string);
        }
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

void add_primitive(char *func_string, int16_t name_size, void (*func)(void)) {
  enum COMPILE_PRIMITIVE status = PRIMITIVE_NS(start);
  struct BaseCell *name_cell = NULL;
  struct BaseCell *type_cell = NULL;
  struct BaseCell *func_cell = new_function(func);
  struct BaseCell *tmp = NULL;
  struct BaseCell *next = NULL;
  int16_t counter = 0;

  while(status != PRIMITIVE_NS(end)) {
    switch(status) {
      case PRIMITIVE_NS(start):
        if(func_string[counter] == ':') {
          char *atom_string = new_string(counter);
          strncpy(atom_string, func_string, counter);

          printf("%s: ", atom_string);
          name_cell = new_atom(atom_string);
          func_string += counter + 1;
          counter = 0;
          status = PRIMITIVE_NS(find_type_start);
        }
        else {
          counter++;
        }
        break;
      case PRIMITIVE_NS(find_type_start):
        if(func_string[counter] == '[') {
          counter++;
          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(skip_space);
        }
        else {
          counter++;
        }
        break;
      case PRIMITIVE_NS(skip_space):
        if((func_string[counter] == '-') && (func_string[counter + 1] == '>')) {
          counter += 2;
          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(return_type);
        }
        else if(func_string[counter] != ' ') {
          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(input_type);
        }
        else {
          counter++;
        }
        break;
      case PRIMITIVE_NS(input_type):
        if((func_string[counter] == '-') && (func_string[counter + 1] == '>')) {
          counter += 2;
          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(return_type);
        }
        else if(func_string[counter] == '!') {
          counter++;
          char *atom_string = new_string(counter);
          strncpy(atom_string, func_string, counter);

          printf("%s ", atom_string);
          next = new_type(atom_string);
          if(type_cell == NULL) {
            type_cell = next;
            tmp = next;
          }
          else {
            tmp->next = next;
            tmp = next;
          }

          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(skip_space);
        }
        else {
          counter++;
        }
        break;
      case PRIMITIVE_NS(return_type):
        if(func_string[counter] == ' ') {
          counter++;
          func_string += counter;
          counter = 0;
        }
        else if(func_string[counter] == '!') {
          counter++;
          char *atom_string = new_string(counter);
          strncpy(atom_string, func_string, counter);

          printf("-> %s\n", atom_string);
          next = new_type(atom_string);
          type_cell = new_list(type_cell, next);

          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(find_type_stop);
        }
        else {
          counter++;
        }
        break;
      case PRIMITIVE_NS(find_type_stop):
        if(func_string[counter] == ' ') {
          counter++;
          func_string += counter;
          counter = 0;
        }
        else if(func_string[counter] == ']') {
          counter++;
          func_string += counter;
          counter = 0;
          status = PRIMITIVE_NS(end);
        }
        else {
          counter++;
        }
        break;
      default:
        status = PRIMITIVE_NS(end);
        break;
    }
  }

  struct BaseCell *cell = new_list(name_cell, new_list(type_cell, func_cell));
  set_env_next(cell);
}

bool find_function(char *func) {
  struct BaseCell *current = secd_machine.env;
  struct BaseList *item = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;

  while(current != NULL) {
    item = current->content.list;
    car = item->car;
    cdr = item->cdr;
    
    if(!strcmp(car->content.string, func)) {
      item = cdr->content.list;
      cdr = item->cdr;
      cdr->content.func();
      return true;
    }

    current = current->next;
  }
  return false;
}

bool run_integer(struct BaseCell *cell, char *error_msg) {
  if(cell == NULL) {
    SECD_MACHINE_NS(error)(error_msg);
    return false;
  }
  else if(cell->type == ATOM) {
    if(!find_function(cell->content.string)) {
      drop_atom(cell);
      return false;
    }
  }
  else if(cell->type == NIL) {
    drop_integer(cell);
    SECD_MACHINE_NS(error)(error_msg);
    return false;
  }
  else if(cell->type == INTEGER) {
    set_stack_next(cell);
  }
  else if(cell->type == LIST) {
    drop_list(cell);
    SECD_MACHINE_NS(error)(error_msg);
    return false;
  }

  if(secd_machine.status == SECD_STATUS_NS(ERROR)) {
    return false;
  }
  else {
    return true;
  }
}

void run_code(void) {
#ifdef DEBUG
  debug_code();
  debug_stack();
#endif
  struct BaseCell *current = pop_code_next();

  while((current != NULL) && (secd_machine.status != SECD_STATUS_NS(ERROR))) {
#ifdef DEBUG
    debug_code();
    debug_stack();
#endif
    if(current->type == ATOM) {
      if(secd_machine.status == SECD_STATUS_NS(CONTINUE)) {
        find_function(current->content.string);
      }
      drop_atom(current);
    }
    else if(current->type == NIL) {
      printf("[Error]:nil\n");
      drop_integer(current);
    }
    else if(current->type == INTEGER) {
      printf("[Error]:%" PRId64 " is integer\n", current->content.integer);
      drop_integer(current);
    }
    else if(current->type == LIST) {
      printf("[Error]:list\n");
      drop_list(current);
    }

    current = pop_code_next();
  }

  clean_code();
}
