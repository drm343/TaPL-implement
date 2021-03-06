#include "secd_memory.h"
#include "secd_debug.h"

// get memory from os
char *debug_new_string(int16_t input_size, char *msg) {
  printf("%s\n", msg);
  return new_string(input_size);
}

void set_pool_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->cell_pool == NULL) {
    secd_machine->cell_pool = cell;
    secd_machine->cell_pool_top = cell;
  }
  else {
    secd_machine->cell_pool_top->next = cell;
    secd_machine->cell_pool_top = cell;
  }
}

void set_list_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->list_pool == NULL) {
    secd_machine->list_pool = cell;
    secd_machine->list_pool_top = cell;
  }
  else {
    secd_machine->list_pool_top->next = cell;
    secd_machine->list_pool_top = cell;
  }
}

struct BaseCell *new_basecell(struct SECD *secd_machine) {
  struct BaseCell *new_cell = NULL;

  if(secd_machine->cell_pool == NULL) {
    new_cell = (struct BaseCell*)malloc(sizeof(struct BaseCell));
  }
  else {
    new_cell = secd_machine->cell_pool;
    secd_machine->cell_pool = new_cell->next;
    new_cell->next = NULL;
  }
  return new_cell;
}

struct BaseCell *new_baselist(struct SECD *secd_machine) {
  if(secd_machine->list_pool == NULL) {
    struct BaseCell *cell = new_basecell(secd_machine);
    struct BaseList *list = (struct BaseList*)malloc(sizeof(struct BaseList));
    cell->content.list = list;
    return cell;
  }
  else {
    struct BaseCell *new_cell = secd_machine->list_pool;

    if(new_cell->next != NULL) {
      secd_machine->list_pool = new_cell->next;
    }
    else {
      secd_machine->list_pool = NULL;
      secd_machine->list_pool_top = NULL;
    }
    new_cell->type = NIL;
    return new_cell;
  }
}

char *new_string(int16_t input_size) {
  if (input_size > 0) {
    char *result = (char *)malloc(1 + sizeof(char) * input_size);
    if(result == NULL) {
      return result;
    }
    else {
      memset(result, ' ', input_size);
      result[input_size] = '\0';
      return result;
    }
  }
  else {
    return NULL;
  }
}

struct BaseCell *new_nil(struct SECD *secd_machine) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = NIL;
  cell->content.integer = 0;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_integer(struct SECD *secd_machine, int64_t integer) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = INTEGER;
  cell->content.integer = integer;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_atom(struct SECD *secd_machine, char *atom_string) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = ATOM;
  cell->content.string = atom_string;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_type(struct SECD *secd_machine, int64_t value) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = TYPE;
  cell->content.integer = value;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_c_function(struct SECD *secd_machine, void (*func)(struct SECD *)) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = C_FUNC;
  cell->content.func = func;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_function(struct SECD *secd_machine, struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = car;
  pair->cdr = cdr;

  cell->type = FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_variable(struct SECD *secd_machine, struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = car;
  pair->cdr = cdr;

  cell->type = VAR;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

#ifdef DEBUG
struct BaseCell *new_uncheck_function(struct SECD *secd_machine,
    struct BaseCell *name, struct BaseCell *func, int64_t status) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = new_list(secd_machine, name, new_integer(secd_machine, status));
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *debug_new_uncheck_function(struct SECD *secd_machine,
    struct BaseCell *name, struct BaseCell *func) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = name;
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}
#else
struct BaseCell *new_uncheck_function(struct SECD *secd_machine,
    struct BaseCell *name, struct BaseCell *func) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = name;
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}
#endif
struct BaseCell *new_uncheck_variable(struct SECD *secd_machine,
    struct BaseCell *name, struct BaseCell *variable) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = name;
  pair->cdr = variable;

  cell->type = UNCHECK_VAR;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_dump(struct SECD *secd_machine, struct BaseCell *item) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = DUMP;
  cell->content.item = item;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_list(struct SECD *secd_machine, struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;

  pair->car = car;
  pair->cdr = cdr;

  cell->type = LIST;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_pointer(struct SECD *secd_machine, void *pointer) {
  struct BaseCell *cell = new_basecell(secd_machine);
  cell->type = POINTER;
  cell->content.pointer = pointer;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_custom(struct SECD *secd_machine, char *name, void *pointer) {
  struct BaseCell *cell = new_baselist(secd_machine);
  struct BaseList *pair = cell->content.list;
  struct BaseCell *car = new_atom(secd_machine, name);
  struct BaseCell *cdr = new_pointer(secd_machine, pointer);

  pair->car = car;
  pair->cdr = cdr;

  cell->type = CUSTOM;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

// return memory to os
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

void free_uncheck_variable(struct BaseList *current) {
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
    else if(current->type == UNCHECK_VAR) {
      free_uncheck_variable(current->content.list);
    }
    else if(current->type == UNCHECK_FUNC) {
      free_uncheck_function(current->content.list);
    }
    else if(current->type == FUNC) {
      free_list(current->content.list);
    }
    else if(current->type == VAR) {
      free_list(current->content.list);
    }
    else if(current->type == POINTER) {
      free(current->content.pointer);
    }
    else {
#ifdef DEBUG
      printf("other\n");
#endif
    }

    free(current);
    current = NULL;

    if(next != NULL) {
      current = next;
      next = next->next;
    }
  }
}

void drop_atom(struct SECD *secd_machine, struct BaseCell *cell) {
  free(cell->content.string);
  cell->content.string = NULL;
  set_pool_next(secd_machine, cell);
}

void drop_integer(struct SECD *secd_machine, struct BaseCell *cell) {
  set_pool_next(secd_machine, cell);
}

void drop_list(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;
  struct BaseList *list = NULL;

  list = cell->content.list;

  car = list->car;
  cdr = list->cdr;

  list->car = NULL;
  list->cdr = NULL;

  set_list_next(secd_machine, cell);
  drop_cell(secd_machine, car);
  drop_cell(secd_machine, cdr);
}

void drop_cell(struct SECD *secd_machine, struct BaseCell *cell) {
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;
  struct BaseList *list = NULL;

  if(cell == NULL) {
  }
  else if(cell->type == ATOM) {
    drop_atom(secd_machine, cell);
  }
  else if(cell->type == NIL) {
    set_pool_next(secd_machine, cell);
  }
  else if(cell->type == INTEGER) {
    set_pool_next(secd_machine, cell);
  }
  else if(cell->type == LIST) {
    drop_list(secd_machine, cell);
  }
  else if(cell->type == DUMP) {
    struct BaseCell *item = cell->content.item;

    cell->content.item = NULL;
    set_pool_next(secd_machine, cell);
    drop_cell(secd_machine, item);
  }
  else if(cell->type == UNCHECK_VAR) {
    list = cell->content.list;
    car = list->car;

    list->car = NULL;

    set_list_next(secd_machine, cell);
    drop_cell(secd_machine, car);
  }
  else if(cell->type == UNCHECK_FUNC) {
    list = cell->content.list;
    car = list->car;

    list->car = NULL;

    set_list_next(secd_machine, cell);
    drop_cell(secd_machine, car);
  }
  else if(cell->type == TYPE) {
    set_pool_next(secd_machine, cell);
  }
  else if(cell->type == FUNC) {
    list = cell->content.list;

    car = list->car;
    cdr = list->cdr;

    list->car = NULL;
    list->cdr = NULL;

    set_list_next(secd_machine, cell);
    drop_cell(secd_machine, car);
    drop_cell(secd_machine, cdr);
  }
  else if(cell->type == C_FUNC) {
    cell->content.func = NULL;
    set_pool_next(secd_machine, cell);
  }
  else {
    printf("not set\n");
  }
}
