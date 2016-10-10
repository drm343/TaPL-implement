#include "secd_memory.h"

// get memory from os
char *debug_new_string(int16_t input_size, char *msg) {
  printf("%s\n", msg);
  return new_string(input_size);
}

void set_pool_next(struct BaseCell *cell) {
  if(secd_machine.cell_pool == NULL) {
    secd_machine.cell_pool = cell;
    secd_machine.cell_pool_top = cell;
  }
  else {
    secd_machine.cell_pool_top->next = cell;
    secd_machine.cell_pool_top = cell;
  }
}

void set_list_next(struct BaseCell *cell) {
  if(secd_machine.list_pool == NULL) {
    secd_machine.list_pool = cell;
    secd_machine.list_pool_top = cell;
  }
  else {
    secd_machine.list_pool_top->next = cell;
    secd_machine.list_pool_top = cell;
  }
}

struct BaseCell *new_cell(void) {
  if(secd_machine.cell_pool == NULL) {
    return (struct BaseCell*)malloc(sizeof(struct BaseCell));
  }
  else {
    struct BaseCell *new_cell = secd_machine.cell_pool;
    secd_machine.cell_pool = new_cell->next;
    new_cell->next = NULL;
    return new_cell;
  }
}

struct BaseList *new_baselist(void) {
  if(secd_machine.list_pool == NULL) {
    return (struct BaseList*)malloc(sizeof(struct BaseList));
  }
  else {
    struct BaseCell *new_cell = secd_machine.list_pool;
    struct BaseList *new_list = new_cell->content.list;

    new_cell->content.list = NULL;

    if(new_cell->next != NULL) {
      secd_machine.list_pool = new_cell->next;
    }
    else {
      secd_machine.list_pool = NULL;
      secd_machine.list_pool_top = NULL;
    }
    new_cell->type = NIL;
    drop_cell(new_cell);
    return new_list;
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

struct BaseCell *new_nil(void) {
  struct BaseCell *cell = new_cell();
  cell->type = NIL;
  cell->content.integer = 0;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_integer(int64_t integer) {
  struct BaseCell *cell = new_cell();
  cell->type = INTEGER;
  cell->content.integer = integer;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_atom(char *atom_string) {
  struct BaseCell *cell = new_cell();
  cell->type = ATOM;
  cell->content.string = atom_string;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_type(char *atom_string) {
  struct BaseCell *cell = new_cell();
  cell->type = TYPE;
  cell->content.string = atom_string;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_c_function(void (*func)(void)) {
  struct BaseCell *cell = new_cell();
  cell->type = C_FUNC;
  cell->content.func = func;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_function(struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = new_cell();
  struct BaseList *pair = new_baselist();

  pair->car = car;
  pair->cdr = cdr;

  cell->type = FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

#ifdef DEBUG
struct BaseCell *new_uncheck_function(struct BaseCell *name, struct BaseCell *func, int64_t status) {
  struct BaseCell *cell = new_cell();
  struct BaseList *pair = new_baselist();

  pair->car = new_list(name, new_integer(status));
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}

struct BaseCell *debug_new_uncheck_function(struct BaseCell *name, struct BaseCell *func) {
  struct BaseCell *cell = new_cell();
  struct BaseList *pair = new_baselist();

  pair->car = name;
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}
#else
struct BaseCell *new_uncheck_function(struct BaseCell *name, struct BaseCell *func) {
  struct BaseCell *cell = new_cell();
  struct BaseList *pair = new_baselist();

  pair->car = name;
  pair->cdr = func;

  cell->type = UNCHECK_FUNC;
  cell->content.list = pair;
  cell->next = NULL;
  return cell;
}
#endif

struct BaseCell *new_dump(struct BaseCell *item) {
  struct BaseCell *cell = new_cell();

  cell->type = DUMP;
  cell->content.item = item;
  cell->next = NULL;
  return cell;
}

struct BaseCell *new_list(struct BaseCell *car, struct BaseCell *cdr) {
  struct BaseCell *cell = new_cell();
  struct BaseList *pair = new_baselist();

  pair->car = car;
  pair->cdr = cdr;

  cell->type = LIST;
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
    else if(current->type == FUNC) {
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

void drop_atom(struct BaseCell *cell) {
  free(cell->content.string);
  cell->content.string = NULL;
  set_pool_next(cell);
}

void drop_integer(struct BaseCell *cell) {
  set_pool_next(cell);
}

void drop_list(struct BaseCell *cell) {
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;
  struct BaseList *list = NULL;

  list = cell->content.list;

  car = list->car;
  cdr = list->cdr;

  list->car = NULL;
  list->cdr = NULL;

  set_list_next(cell);
  drop_cell(car);
  drop_cell(cdr);
}

void drop_cell(struct BaseCell *cell) {
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;
  struct BaseList *list = NULL;

  if(cell == NULL) {
  }
  if(cell->type == ATOM) {
    drop_atom(cell);
  }
  else if(cell->type == NIL) {
    set_pool_next(cell);
  }
  else if(cell->type == INTEGER) {
    set_pool_next(cell);
  }
  else if(cell->type == LIST) {
    drop_list(cell);
  }
  else if(cell->type == DUMP) {
    struct BaseCell *item = cell->content.item;

    cell->content.item = NULL;
    set_pool_next(cell);
    drop_cell(item);
  }
  else if(cell->type == UNCHECK_FUNC) {
    list = cell->content.list;

    car = list->car;

    list->car = NULL;

    set_list_next(cell);
    drop_cell(car);
  }
  else if(cell->type == TYPE) {
    free(cell->content.string);
    cell->content.string = NULL;
    set_pool_next(cell);
  }
  else if(cell->type == FUNC) {
    list = cell->content.list;

    car = list->car;
    cdr = list->cdr;

    list->car = NULL;
    list->cdr = NULL;

    set_list_next(cell);
    drop_cell(car);
    drop_cell(cdr);
  }
  else if(cell->type == C_FUNC) {
    cell->content.func = NULL;
    set_pool_next(cell);
  }
}
