#include "secd_struct.h"
#include "secd_machine.h"
#include "secd_debug.h"

#define CMD_BUFFER_SIZE 1024
#define ESC 27
#define ESCAPE 34
#define SPACE (int)' '
#define ASCII_0 48
#define ASCII_9 57

struct SECD secd_machine;

void run_code(void);
void free_list(struct BaseList *);
void free_stack(struct BaseCell *);

// kernel command
char *new_string(int16_t input_size) {
  if (input_size > 0) {
    char *result = (char *)malloc(1 + sizeof(char) * input_size);
    if(result == NULL) {
      return result;
    }
    else {
      result[input_size] = '\0';
      return result;
    }
  }
  else {
    return NULL;
  }
}

struct BaseCell *lookup_env(char *func) {
  struct BaseCell *current = secd_machine.env;
  struct BaseList *item = NULL;
  struct BaseCell *car = NULL;

  while(current != NULL) {
    item = current->content.list;
    car = item->car;
    
    if(!strcmp(car->content.string, func)) {
      break;
    }

    current = current->next;
  }
  return current;
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
  memset(cell, ' ', sizeof(struct BaseCell));
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

struct BaseCell *new_uncheck_function(struct BaseCell *name, struct BaseCell *func) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  struct BaseList *pair = (struct BaseList*)malloc(sizeof(struct BaseList));
  pair->car = name;
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_dump(struct BaseCell *item) {
  struct BaseCell *cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  cell->type = DUMP;
  cell->content.item = item;
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
  struct BaseCell *cell = pop_code_next();

  while(cell != NULL) {
    drop_cell(cell);
    cell = pop_code_next();
  }
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
  if(secd_machine.code_bottom == NULL) {
    secd_machine.code_bottom = cell;
  }

  if(secd_machine.code != NULL) {
    secd_machine.code->next = cell;
  }
  else {
    secd_machine.code = cell;
  }
  secd_machine.code = cell;
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

void set_dump_next(struct BaseCell *origin_cell) {
  struct BaseCell *cell = new_dump(origin_cell);
  if(secd_machine.dump_bottom == NULL) {
    secd_machine.dump_bottom = cell;
    secd_machine.dump = cell;
  }
  else {
    cell->next = secd_machine.dump;
    secd_machine.dump = cell;
  }
}

struct BaseCell *pop_code_next(void) {
  struct BaseCell *cell = secd_machine.code_bottom;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine.code = NULL;
    }

    secd_machine.code_bottom = cell->next;
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

struct BaseCell *pop_dump_next(void) {
  struct BaseCell *cell = secd_machine.dump;
  struct BaseCell *item = NULL;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine.dump = NULL;
      secd_machine.dump_bottom = NULL;
    }
    else {
      secd_machine.dump = cell->next;
    }
    cell->next = NULL;
  }
  item = cell->content.item;
  cell->content.item = NULL;
  drop_cell(cell);
  return item;
}

void dump_stack() {
  set_dump_next(secd_machine.s);
  set_dump_next(secd_machine.bottom);
  secd_machine.s = NULL;
  secd_machine.bottom = NULL;
}

void recover_stack(void) {
  secd_machine.bottom = pop_dump_next();
  secd_machine.s = pop_dump_next();
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
  else if(cell->type == DUMP) {
    drop_atom(cell);
  }
  else if(cell->type == UNCHECK_FUNC) {
    drop_list(cell);
  }
  else if(cell->type == TYPE) {
    drop_atom(cell);
  }
  else if(cell->type == FUNC) {
    free_stack(cell);
  }
}

void compile_atom(char *atom_string) {
  struct BaseCell *cell = new_atom(atom_string);
  set_code_next(cell);
}

struct BaseCell *compile_integer(struct BaseCell *cell) {
  size_t atom_size = strlen(cell->content.string);
  char *atom_string = cell->content.string;
  int64_t count = 0;

  for(size_t i = 0; i < atom_size; i++) {
    if((atom_string[i] >= ASCII_0) && (atom_string[i] <= ASCII_9)) {
      count = count * 10 + (atom_string[i] - ASCII_0);
    }
    else {
      return cell;
    }
  }

  drop_cell(cell);
  struct BaseCell *new_cell = new_integer(count);
  return new_cell;
}

struct BaseCell *compile_unchecked_function(struct BaseCell *cell) {
  struct BaseCell *result = lookup_env(cell->content.string);

  if(result != NULL) {
    return new_uncheck_function(cell, result);
  }
  else {
    return cell;
  }
}

struct BaseCell *compile_checked_function(struct BaseCell *cell) {
  struct BaseList *list = cell->content.list;
  struct BaseCell *result = list->cdr;
  list->cdr = NULL;
  drop_cell(cell);
  return result;
}

void tokenize_pass(void) {
  char *command = secd_machine.tmp_code;
  int16_t counter = 0;
  int16_t next = 0;

  while(next < CMD_BUFFER_SIZE) {
    if(((int)secd_machine.tmp_code[next]) == SPACE) {
      if(counter >= 1) {
        char *atom_string = new_string(counter + 1);
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

void turn_integer_pass(void) {
  struct BaseCell *current = pop_code_next();
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_bottom = NULL;

  while(current != NULL) {
    if(current->type == ATOM) {
      current = compile_integer(current);
    }

    if(tmp != NULL) {
      tmp->next = current;
    }

    if(tmp_bottom == NULL) {
      tmp_bottom = current;
    }
    tmp = current;
    current = pop_code_next();
  }
  secd_machine.code_bottom = tmp_bottom;
}

void turn_function_pass(void) {
  struct BaseCell *current = pop_code_next();
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_bottom = NULL;

  while(current != NULL) {
    if(current->type == ATOM) {
      current = compile_unchecked_function(current);
    }

    if(tmp != NULL) {
      tmp->next = current;
    }

    if(tmp_bottom == NULL) {
      tmp_bottom = current;
    }
    tmp = current;
    current = pop_code_next();
  }
  secd_machine.code_bottom = tmp_bottom;
}

void check_type_pass(void) {
  int16_t parameter_number = 0;
  int16_t tmp_parameter_number = 0;
  int16_t status = 0;
  char *check_type = NULL;
  struct BaseCell *current = pop_code_next();
  struct BaseCell *bottom = NULL;
  struct BaseCell *top = NULL;
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_type = NULL;

  struct BaseList *list = NULL;
  struct BaseCell *parameter = NULL;
  struct BaseCell *return_type = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;

  dump_stack();

  while(current != NULL) {
    if(top == NULL) {
      bottom = current;
    }
    else {
      top->next = current;
    }
    top = current;

    if(current->type == UNCHECK_FUNC) {
      list = current->content.list;
      cdr = list->cdr;
      list = cdr->content.list;
      cdr = list->cdr;
      list = cdr->content.list;
      car = list->car;
      list = cdr->content.list;
      car = list->car;
      list = car->content.list;
      parameter = list->car;
      return_type = list->cdr;
      parameter_number = parameter->content.integer;

      if(parameter_number > 1) {
        parameter = parameter->next;
      }
      else {
        parameter = NULL;
      }

      tmp_type = pop_stack_next();

      if(tmp_type == NULL) {
        if(parameter_number > 0) {
          dump_stack();
          set_dump_next(tmp);
          set_dump_next(new_integer(tmp_parameter_number));
          tmp = current;
          tmp_parameter_number = parameter_number;
          status++;

          for(int16_t i = 0; i < parameter_number; i++) {
            int16_t size = strlen(parameter->content.string);
            char *str = new_string(size);
            strncpy(str, parameter->content.string, size);

            struct BaseCell *type = new_type(str);
            set_stack_next(type);
            parameter = parameter->next;
          }
        }
      }
      else if((tmp_type != NULL) && (!strcmp(tmp_type->content.string, return_type->content.string))) {
        drop_cell(tmp_type);
        tmp_parameter_number--;

        if(parameter_number > 0) {
          dump_stack();
          set_dump_next(tmp);
          set_dump_next(new_integer(tmp_parameter_number));
          tmp = current;
          tmp_parameter_number = parameter_number;
          status++;

          for(int16_t i = 0; i < parameter_number; i++) {
            int16_t size = strlen(parameter->content.string);
            char *str = new_string(size);
            strncpy(str, parameter->content.string, size);

            struct BaseCell *type = new_type(str);
            set_stack_next(type);
            parameter = parameter->next;
          }
        }
      }
      else {
        printf("except type %s but you give type %s\n", tmp_type->content.string, return_type->content.string);
        SECD_MACHINE_NS(type_error)("type error");
      }
    }
    else {
      switch(current->type) {
        case NIL:
          check_type = "nil!";
          break;
        case INTEGER:
          check_type = "int!";
          break;
        case ATOM:
          check_type = "atom!";
          break;
        default:
          check_type = "bottom!";
          break;
      }

      tmp_type = pop_stack_next();
      if(tmp_type == NULL) {
      }
      else if((tmp_type != NULL) && (!strcmp(tmp_type->content.string, check_type))) {
        drop_cell(tmp_type);
        tmp_parameter_number--;
      }
      else {
        printf("except type %s but you give type %s\n", tmp_type->content.string, return_type->content.string);
        SECD_MACHINE_NS(type_error)("type error");
      }
    }

    if((tmp_parameter_number <= 0) && (status > 0)) {
      status--;

      tmp = pop_dump_next();
      tmp_parameter_number = tmp->content.integer;
      drop_cell(tmp);
      tmp = pop_dump_next();

      recover_stack();
    }
    current = pop_code_next();
  }

  if(secd_machine.status != SECD_STATUS_NS(ERROR)) {
    secd_machine.code = top;
    secd_machine.code_bottom = bottom;
  }

  recover_stack();
}

void compile_code(int16_t input_size, char *command) {
  memset(secd_machine.tmp_code, ' ', CMD_BUFFER_SIZE);
  strncpy(secd_machine.tmp_code, command, input_size);

  tokenize_pass();
  turn_integer_pass();
  turn_function_pass();
  check_type_pass();
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

  secd_machine.code_bottom = NULL;
  secd_machine.code = NULL;
  secd_machine.tmp_code = new_string(CMD_BUFFER_SIZE);

  secd_machine.env = NULL;
  secd_machine.env_bottom = NULL;

  secd_machine.dump = NULL;
  secd_machine.dump_bottom = NULL;

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

void free_uncheck_function(struct BaseList *current) {
  struct BaseCell *car = current->car;

  free(current);
  free_stack(car);
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
    else if(current->type == UNCHECK_FUNC) {
      free_uncheck_function(current->content.list);
    }
    else if(current->type == TYPE) {
      free(current->content.string);
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

void SECD_MACHINE_NS(type_error)(char *msg) {
  printf("%s\n", msg);
  secd_machine.status = SECD_STATUS_NS(ERROR);

  clean_code();
}

void SECD_MACHINE_NS(recover_machine)(void) {
  secd_machine.status = SECD_STATUS_NS(CONTINUE);
}

void stop_machine(void) {
  free_stack(secd_machine.bottom);
  free_stack(secd_machine.code_bottom);
  free_stack(secd_machine.env);

  free_stack(secd_machine.atom_pool);
  free_stack(secd_machine.integer_pool);
  free_stack(secd_machine.list_pool);

  free(secd_machine.tmp_code);
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

void add_primitive(char *func_string, int16_t name_size, void (*func)(void)) {
  enum COMPILE_PRIMITIVE status = PRIMITIVE_NS(start);
  struct BaseCell *name_cell = NULL;
  struct BaseCell *type_cell = NULL;
  struct BaseCell *func_cell = new_function(func);
  struct BaseCell *tmp = NULL;
  struct BaseCell *next = NULL;
  int16_t counter = 0;
  int16_t parameter = 0;

  while(status != PRIMITIVE_NS(end)) {
    switch(status) {
      case PRIMITIVE_NS(start):
        if(func_string[counter] == ':') {
          char *atom_string = new_string(counter);
          strncpy(atom_string, func_string, counter);

#ifdef DEBUG
          printf("%s: ", atom_string);
#endif
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
          next = new_integer(parameter);

          if(type_cell == NULL) {
            type_cell = next;
          }
          else {
            next->next = type_cell;
            type_cell = next;
          }
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

#ifdef DEBUG
          printf("%s ", atom_string);
#endif
          next = new_type(atom_string);
          if(type_cell == NULL) {
            type_cell = next;
            tmp = next;
          }
          else {
            tmp->next = next;
            tmp = next;
          }

          parameter++;
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

#ifdef DEBUG
          printf("-> %s\n", atom_string);
#endif
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

void run_uncheck_function(struct BaseCell *cell) {
  struct BaseList *item = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;

  item = cell->content.list;
  car = item->car;
  cdr = item->cdr;
    
  item = cdr->content.list;
  cdr = item->cdr;

  item = cdr->content.list;
  cdr = item->cdr;
  cdr->content.func();
}

bool run_integer(struct BaseCell *cell, char *error_msg) {
  if(cell == NULL) {
    SECD_MACHINE_NS(error)(error_msg);
    return false;
  }
  else if(cell->type == UNCHECK_FUNC) {
    if(secd_machine.status == SECD_STATUS_NS(CONTINUE)) {
      run_uncheck_function(cell);
    }
    drop_cell(cell);
  }
  else if(cell->type == ATOM) {
    drop_atom(cell);
    SECD_MACHINE_NS(error)("not integer");
    return false;
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
  printf("[run]:start\n");
  debug_code();
  debug_stack();
#endif
  struct BaseCell *current = pop_code_next();

  while((current != NULL) && (secd_machine.status != SECD_STATUS_NS(ERROR))) {
    if(current->type == UNCHECK_FUNC) {
      if(secd_machine.status == SECD_STATUS_NS(CONTINUE)) {
        run_uncheck_function(current);
      }
      drop_cell(current);
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
#ifdef DEBUG
  printf("[run]:done\n");
#endif
}
