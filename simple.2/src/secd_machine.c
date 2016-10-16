#include "secd_struct.h"
#include "secd_machine.h"
#include "secd_debug.h"
#include "secd_memory.h"

#define ESC 27
#define ESCAPE 34
#define SPACE (int)' '
#define ASCII_0 48
#define ASCII_9 57
#define STRCMP(x, y) strncmp(x, y, strlen(x))

void run_code(struct SECD *);

// kernel command
struct BaseCell *get_function_type(struct BaseCell *current) {
  struct BaseCell *cell = NULL;
  struct BaseList *list = current->content.list;
  cell = list->cdr;
  list = cell->content.list;
  cell = list->cdr;
  list = cell->content.list;
  cell = list->car;
  return cell;
}

struct BaseCell *get_parameter_type(struct BaseCell *current) {
    struct BaseList *list = current->content.list;
    return list->car;
}

struct BaseCell *get_return_type(struct BaseCell *current) {
    struct BaseList *list = current->content.list;
    return list->cdr;
}

struct BaseCell *lookup_env(struct SECD *secd_machine, char *func) {
  struct BaseCell *current = secd_machine->env;
  struct BaseList *item = NULL;
  struct BaseCell *car = NULL;

  while(current != NULL) {
    item = current->content.list;
    car = item->car;
    
    if(!STRCMP(car->content.string, func)) {
      break;
    }

    current = current->next;
  }
  return current;
}

void clean_code(struct SECD *secd_machine) {
  struct BaseCell *cell = pop_code_next(secd_machine);

  while(cell != NULL) {
    drop_cell(secd_machine, cell);
    cell = pop_code_next(secd_machine);
  }
}

void set_code_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->code_bottom == NULL) {
    secd_machine->code_bottom = cell;
  }

  if(secd_machine->code != NULL) {
    secd_machine->code->next = cell;
  }
  else {
    secd_machine->code = cell;
  }
  secd_machine->code = cell;
}

void set_dump_next(struct SECD *secd_machine, struct BaseCell *origin_cell) {
  struct BaseCell *cell = new_dump(secd_machine, origin_cell);

  if(secd_machine->dump_bottom == NULL) {
    secd_machine->dump_bottom = cell;
    secd_machine->dump = cell;
  }
  else {
    cell->next = secd_machine->dump;
    secd_machine->dump = cell;
  }
}

void set_env_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->env_bottom == NULL) {
    secd_machine->env_bottom = cell;
  }

  if(secd_machine->env == NULL) {
    secd_machine->env = cell;
  }
  else {
    cell->next = secd_machine->env;
    secd_machine->env = cell;
  }
}

void set_stack_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->bottom == NULL) {
    secd_machine->bottom = cell;
    secd_machine->s = cell;
  }
  else {
    cell->next = secd_machine->s;
    secd_machine->s = cell;
  }
}

struct BaseCell *copy_cell(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseCell *new_cell = NULL;
  struct BaseList *list = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;
  int16_t str_size = 0;
  char *atom_string = NULL;

  switch(cell->type) {
    case NIL:
      new_cell = new_nil(secd_machine);
      break;
    case INTEGER:
      new_cell = new_integer(secd_machine, cell->content.integer);
      break;
    case ATOM:
      str_size = strlen(cell->content.string) - 1;
      atom_string = new_string(str_size);
      strncpy(atom_string, cell->content.string, str_size);
      new_cell = new_atom(secd_machine, atom_string);
      break;
    case LIST:
      list = cell->content.list;
      car = copy_cell(secd_machine, list->car);
      cdr = copy_cell(secd_machine, list->cdr);
      new_cell = new_list(secd_machine, car, cdr);
      break;
    case TYPE:
      str_size = strlen(cell->content.string);
      atom_string = new_string(str_size);
      strncpy(atom_string, cell->content.string, str_size);
      new_cell = new_type(secd_machine, atom_string);
      break;
    case UNCHECK_FUNC:
      list = cell->content.list;
      new_cell = copy_cell(secd_machine, list->car);
      // TODO
      // DEBUG version
#ifdef DEBUG
      new_cell = debug_new_uncheck_function(secd_machine, new_cell, list->cdr);
#else
      new_cell = new_uncheck_function(secd_machine, new_cell, list->cdr);
#endif
      break;
    case DUMP:
      new_cell = copy_cell(secd_machine, cell->content.item);
      new_cell = new_dump(secd_machine, new_cell);
      break;
    case FUNC:
      list = cell->content.list;
      car = copy_cell(secd_machine, list->car);
      new_cell = new_function(secd_machine, car, list->cdr);
      break;
    case C_FUNC:
      new_cell = new_c_function(secd_machine, cell->content.func);
      break;
    default:
      break;
  };

  return new_cell;
}

void copy_stack_next(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseCell *new_cell = copy_cell(secd_machine, cell);
  set_stack_next(secd_machine, new_cell);
}

struct BaseCell *pop_code_next(struct SECD *secd_machine) {
  struct BaseCell *cell = secd_machine->code_bottom;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine->code = NULL;
    }

    secd_machine->code_bottom = cell->next;
    cell->next = NULL;
  }
  return cell;
}

struct BaseCell *pop_dump_next(struct SECD *secd_machine) {
  struct BaseCell *cell = secd_machine->dump;
  struct BaseCell *item = NULL;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine->dump = NULL;
      secd_machine->dump_bottom = NULL;
    }
    else {
      secd_machine->dump = cell->next;
    }
    cell->next = NULL;
  }
  item = cell->content.item;
  cell->content.item = NULL;
  cell->type = NIL;
  drop_cell(secd_machine, cell);
  return item;
}

struct BaseCell *pop_stack_next(struct SECD *secd_machine) {
  struct BaseCell *cell = secd_machine->s;
  
  if(cell != NULL) {
    if(cell->next == NULL) {
      secd_machine->s = NULL;
      secd_machine->bottom  = NULL;
    }
    else {
      secd_machine->s = cell->next;
    }
    cell->next = NULL;
  }
  return cell;
}

void dump_stack(struct SECD *secd_machine) {
  set_dump_next(secd_machine, secd_machine->s);
  set_dump_next(secd_machine, secd_machine->bottom);
  secd_machine->s = NULL;
  secd_machine->bottom = NULL;
}

void recover_stack(struct SECD *secd_machine) {
  secd_machine->bottom = pop_dump_next(secd_machine);
  secd_machine->s = pop_dump_next(secd_machine);
}

void compile_atom(struct SECD *secd_machine, char *atom_string) {
  struct BaseCell *cell = new_atom(secd_machine, atom_string);
  set_code_next(secd_machine, cell);
}

struct BaseCell *compile_integer(struct SECD *secd_machine, struct BaseCell *cell) {
  size_t atom_size = strlen(cell->content.string) - 1;
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

  drop_cell(secd_machine, cell);
  struct BaseCell *new_cell = new_integer(secd_machine, count);
  return new_cell;
}

#ifdef DEBUG
struct BaseCell *compile_unchecked_function(struct SECD *secd_machine,
    struct BaseCell *cell, int64_t status) {
  struct BaseCell *result = lookup_env(secd_machine, cell->content.string);

  if(result != NULL) {
    return new_uncheck_function(secd_machine, cell, result, status);
  }
  else {
    return cell;
  }
}
#else
struct BaseCell *compile_unchecked_function(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseCell *result = lookup_env(secd_machine, cell->content.string);

  if(result != NULL) {
    return new_uncheck_function(secd_machine, cell, result);
  }
  else {
    return cell;
  }
}
#endif

struct BaseCell *compile_checked_function(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseList *list = cell->content.list;
  struct BaseCell *result = list->cdr;
  list->cdr = NULL;
  drop_cell(secd_machine, cell);
  return result;
}

void tokenize_pass(struct SECD *secd_machine) {
  char *command = secd_machine->tmp_code;
  int16_t counter = 0;
  int16_t str_size = strlen(command);

  for(int16_t next = 0; next <= str_size; next++) {
    if(((int)secd_machine->tmp_code[next]) == SPACE) {
      if(counter >= 1) {
        char *atom_string = new_string(counter + 1);
        strncpy(atom_string, command, counter);
        compile_atom(secd_machine, atom_string);
      }
      command += counter + 1;
      counter = 0;
    }
    else {
      counter++;
    }
  }
}

void turn_integer_pass(struct SECD *secd_machine) {
  struct BaseCell *current = pop_code_next(secd_machine);
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_bottom = NULL;

  while(current != NULL) {
    if(current->type == ATOM) {
      current = compile_integer(secd_machine, current);
    }

    if(tmp_bottom == NULL) {
      tmp_bottom = current;
    }

    if(tmp != NULL) {
      tmp->next = current;
    }
    tmp = current;
    current = pop_code_next(secd_machine);
  }

  secd_machine->code = tmp;
  secd_machine->code_bottom = tmp_bottom;
}

void turn_function_pass(struct SECD *secd_machine) {
  struct BaseCell *current = pop_code_next(secd_machine);
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_bottom = NULL;
#ifdef DEBUG
  int64_t status = -1;
#endif

  while(current != NULL) {
#ifdef DEBUG
    status++;
#endif

    if(current->type == ATOM) {
#ifdef DEBUG
      current = compile_unchecked_function(secd_machine, current, status);
#else
      current = compile_unchecked_function(secd_machine, current);
#endif
    }

    if(tmp_bottom == NULL) {
      tmp_bottom = current;
    }

    if(tmp != NULL) {
      tmp->next = current;
    }
    tmp = current;
    current = pop_code_next(secd_machine);
  }

  secd_machine->code = tmp;
  secd_machine->code_bottom = tmp_bottom;
}

void typecheck_function(struct SECD *secd_machine, struct BaseCell *current,
    int16_t *uncheck_parameter_number, int16_t *dump_function_number) {
  struct BaseCell *tmp_type = pop_stack_next(secd_machine);

  if((tmp_type == NULL) && ((*dump_function_number) <= 0)) {
    struct BaseCell *unchecked_type = get_function_type(current);
    struct BaseCell *parameter = get_parameter_type(unchecked_type);
    int16_t parameter_number = parameter->content.integer;

    *uncheck_parameter_number = *uncheck_parameter_number - 1;

    if(parameter_number >= 1) {
      *dump_function_number = *dump_function_number + 1;

      parameter = parameter->next;

      dump_stack(secd_machine);
      set_dump_next(secd_machine, new_integer(secd_machine, *uncheck_parameter_number));
      *uncheck_parameter_number = parameter_number;

      for(;parameter_number > 0; parameter_number--) {
        copy_stack_next(secd_machine, parameter);
        parameter = parameter->next;
      }
    }
  }
  else {
    struct BaseCell *unchecked_type = get_function_type(current);
    struct BaseCell *return_type = get_return_type(unchecked_type);
    struct BaseCell *parameter = get_parameter_type(unchecked_type);
    int16_t parameter_number = parameter->content.integer;

    if(STRCMP(tmp_type->content.string, return_type->content.string)) {
      printf("except type %s but you give type %s\n", tmp_type->content.string, return_type->content.string);
      SECD_MACHINE_NS(type_error)(secd_machine, "type error");
    }
    else {
      *uncheck_parameter_number = *uncheck_parameter_number - 1;
      *dump_function_number = *dump_function_number + 1;

      if(parameter_number >= 1) {
        parameter = parameter->next;

        dump_stack(secd_machine);
        set_dump_next(secd_machine, new_integer(secd_machine, *uncheck_parameter_number));
        *uncheck_parameter_number = parameter_number;

        for(;parameter_number > 0; parameter_number--) {
          copy_stack_next(secd_machine, parameter);
          parameter = parameter->next;
        }
      }
    }
    drop_cell(secd_machine, tmp_type);
  }
}

void typecheck_not_function(struct SECD *secd_machine, char *check_type, int16_t *uncheck_parameter_number) {
  struct BaseCell *tmp_type = pop_stack_next(secd_machine);

  if(tmp_type == NULL) {
  }
  else {
    *uncheck_parameter_number = *uncheck_parameter_number - 1;

    if(STRCMP(tmp_type->content.string, check_type)) {
      printf("except type %s but you give type %s\n", tmp_type->content.string, check_type);
      SECD_MACHINE_NS(type_error)(secd_machine, "type error");
    }
    drop_cell(secd_machine, tmp_type);
  }
}

void check_is_return(struct SECD *secd_machine, int16_t *parameter_number, int16_t *dump_function_number) {
  if(*parameter_number <= 0) {
    struct BaseCell *cell = pop_dump_next(secd_machine);
    *parameter_number = cell->content.integer;
    recover_stack(secd_machine);
    drop_integer(secd_machine, cell);
    *dump_function_number = *dump_function_number - 1;
  }
}

void typecheck_pass(struct SECD *secd_machine) {
  struct BaseCell *current = pop_code_next(secd_machine);
  struct BaseCell *tmp = NULL;
  struct BaseCell *tmp_bottom = NULL;
  char *check_type = NULL;
  int16_t dump_function_number = 0;
  int16_t parameter_number = 0;

  dump_stack(secd_machine);

  while((current != NULL) && (secd_machine->status != SECD_STATUS_NS(ERROR))) {
#ifdef DEBUG
    printf("[pass]:%d,%d\n", parameter_number, dump_function_number);
#endif
    switch(current->type) {
      case UNCHECK_FUNC:
        check_type = "func!";
        goto FUNC;
        break;
      case INTEGER:
        check_type = "int!";
        goto ATOM;
        break;
      case ATOM:
        check_type = "atom!";
        goto ATOM;
        break;
      default:
        check_type = "bottom!";
        goto ATOM;
        break;
    }

FUNC:
    typecheck_function(secd_machine, current, &parameter_number, &dump_function_number);
    goto SETUP;
ATOM:
    typecheck_not_function(secd_machine, check_type, &parameter_number);
SETUP:
    if(secd_machine->status != SECD_STATUS_NS(ERROR)) {
      while((parameter_number <= 0) && (dump_function_number >= 1)) {
        check_is_return(secd_machine, &parameter_number, &dump_function_number);
      }

      if(tmp_bottom == NULL) {
        tmp_bottom = current;
      }

      if(tmp != NULL) {
        tmp->next = current;
      }
      tmp = current;
      current = pop_code_next(secd_machine);
    }
    else {
      current = NULL;
    }
  }

  if(secd_machine->status != SECD_STATUS_NS(ERROR)) {
    secd_machine->code = tmp;
    secd_machine->code_bottom = tmp_bottom;
  }
  else {
    while((parameter_number >= 1) || (dump_function_number >= 1)) {
      for(; parameter_number > 0; parameter_number--) {
        pop_stack_next(secd_machine);
      }
      check_is_return(secd_machine, &parameter_number, &dump_function_number);
    }
  }

  recover_stack(secd_machine);
}

void compile_message(struct SECD *secd_machine) {
  tokenize_pass(secd_machine);
  turn_integer_pass(secd_machine);
  turn_function_pass(secd_machine);
  typecheck_pass(secd_machine);
}

// user command
void ldc(struct SECD *secd_machine, int64_t integer) {
  struct BaseCell *cell = new_integer(secd_machine, integer);
  set_stack_next(secd_machine, cell);
}

// machine command
void send(struct SECD *secd_machine, char *message) {
  int16_t str_size = strlen(message);
  char exit_command[5];

  memset(exit_command, 0, 5);

  SECD_MACHINE_NS(recover_machine)(secd_machine);

  strncpy(exit_command, message, 4);
  if(!strcmp(exit_command, "exit")) {
    secd_machine->status = SECD_STATUS_NS(STOP);
  }
  else {
    memset(secd_machine->tmp_code, ' ', CMD_BUFFER_SIZE);
    strncpy(secd_machine->tmp_code, message, str_size);
  }
}

void recv(struct SECD *secd_machine) {
  compile_message(secd_machine);
  run_code(secd_machine);
  memset(secd_machine->tmp_code, ' ', CMD_BUFFER_SIZE);
}

struct SECD *init_machine(void (*register_function)(struct SECD *)) {
  struct SECD *secd_machine = (struct SECD*)malloc(sizeof(struct SECD));
  secd_machine->s = NULL;
  secd_machine->bottom = NULL;

  secd_machine->code_bottom = NULL;
  secd_machine->code = NULL;
  secd_machine->tmp_code = new_string(CMD_BUFFER_SIZE);

  secd_machine->env = NULL;
  secd_machine->env_bottom = NULL;

  secd_machine->dump = NULL;
  secd_machine->dump_bottom = NULL;

  secd_machine->cell_pool = NULL;
  secd_machine->cell_pool_top = NULL;

  secd_machine->list_pool = NULL;
  secd_machine->list_pool_top = NULL;

  secd_machine->status = SECD_STATUS_NS(CONTINUE);

  secd_machine->send = send;
  secd_machine->recv = recv;
  register_function(secd_machine);
  return secd_machine;
}

void SECD_MACHINE_NS(error)(struct SECD *secd_machine, char *msg) {
  printf("%s\n", msg);
  secd_machine->status = SECD_STATUS_NS(ERROR);

  clean_code(secd_machine);
}

void SECD_MACHINE_NS(type_error)(struct SECD *secd_machine, char *msg) {
  printf("%s\n", msg);
  secd_machine->status = SECD_STATUS_NS(ERROR);

  clean_code(secd_machine);
}

void SECD_MACHINE_NS(recover_machine)(struct SECD *secd_machine) {
  secd_machine->status = SECD_STATUS_NS(CONTINUE);
}

void stop_machine(struct SECD *secd_machine) {
  free_stack(secd_machine->bottom);
  free_stack(secd_machine->code_bottom);
  free_stack(secd_machine->env);

  free_stack(secd_machine->cell_pool);
  free_stack(secd_machine->list_pool);

  free(secd_machine->tmp_code);
  free(secd_machine);
}

void add_primitive(struct SECD *secd_machine,
    char *func_string, int16_t name_size, void (*func)(struct SECD *)) {
  enum COMPILE_PRIMITIVE status = PRIMITIVE_NS(start);
  struct BaseCell *name_cell = NULL;
  struct BaseCell *type_cell = NULL;
  struct BaseCell *func_cell = new_c_function(secd_machine, func);
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
          name_cell = new_atom(secd_machine, atom_string);
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
          next = new_integer(secd_machine, parameter);

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
          next = new_type(secd_machine, atom_string);
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
          next = new_type(secd_machine, atom_string);
          type_cell = new_list(secd_machine, type_cell, next);

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

  struct BaseCell *list = new_list(secd_machine, type_cell, func_cell);
  struct BaseCell *cell = new_function(secd_machine, name_cell, list);
  set_env_next(secd_machine, cell);
}

void run_uncheck_function(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseList *item = NULL;
  struct BaseCell *cdr = NULL;

  item = cell->content.list;
  cdr = item->cdr;
  item->cdr = NULL;
    
  item = cdr->content.list;
  cdr = item->cdr;

  item = cdr->content.list;
  cdr = item->cdr;
  cdr->content.func(secd_machine);
}

bool run_integer(struct SECD *secd_machine, struct BaseCell *cell, char *error_msg) {
  if(cell == NULL) {
    SECD_MACHINE_NS(error)(secd_machine, error_msg);
    return false;
  }
  else if(cell->type == UNCHECK_FUNC) {
    if(secd_machine->status == SECD_STATUS_NS(CONTINUE)) {
      run_uncheck_function(secd_machine, cell);
    }
    drop_cell(secd_machine, cell);
  }
  else if(cell->type == ATOM) {
    drop_atom(secd_machine, cell);
    SECD_MACHINE_NS(error)(secd_machine, "not integer");
    return false;
  }
  else if(cell->type == NIL) {
    drop_integer(secd_machine, cell);
    SECD_MACHINE_NS(error)(secd_machine, error_msg);
    return false;
  }
  else if(cell->type == INTEGER) {
    set_stack_next(secd_machine, cell);
  }
  else if(cell->type == LIST) {
    drop_list(secd_machine, cell);
    SECD_MACHINE_NS(error)(secd_machine, error_msg);
    return false;
  }

  if(secd_machine->status == SECD_STATUS_NS(ERROR)) {
    return false;
  }
  else {
    return true;
  }
}

void run_code(struct SECD *secd_machine) {
#ifdef DEBUG
  printf("[run]:start\n");
  debug_code(secd_machine);
  debug_stack(secd_machine);
#endif
  struct BaseCell *current = pop_code_next(secd_machine);

  while((current != NULL) && (secd_machine->status != SECD_STATUS_NS(ERROR))) {
    if(current->type == UNCHECK_FUNC) {
      if(secd_machine->status == SECD_STATUS_NS(CONTINUE)) {
        run_uncheck_function(secd_machine, current);
      }
      drop_cell(secd_machine, current);
    }
    else if(current->type == NIL) {
      printf("[Error]:nil\n");
      drop_integer(secd_machine, current);
    }
    else if(current->type == INTEGER) {
      printf("[Error]:%" PRId64 " is integer\n", current->content.integer);
      drop_integer(secd_machine, current);
    }
    else if(current->type == LIST) {
      printf("[Error]:list\n");
      drop_list(secd_machine, current);
    }

    current = pop_code_next(secd_machine);
  }

  clean_code(secd_machine);
#ifdef DEBUG
  printf("[run]:done\n");
#endif
}
