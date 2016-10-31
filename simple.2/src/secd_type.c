#include "secd_type.h"
#include "secd_memory.h"

int64_t easy_hash_value(char *atom) {
  int8_t str_size = strlen(atom);
  int64_t counter = 0;

  for(int8_t i = 0; i < str_size; i++) {
    counter += atom[i] * i;
  }
  return counter;
}

void set_type_next(struct SECD *secd_machine, struct BaseCell *cell) {
  if(secd_machine->type_pool_bottom == NULL) {
    secd_machine->type_pool_bottom = cell;
  }

  if(secd_machine->type_pool != NULL) {
    secd_machine->type_pool->next = cell;
  }
  secd_machine->type_pool = cell;
}

void extendtype(struct SECD *secd_machine, char *type_string) {
  int16_t str_size = strlen(type_string);
  char *atom_string = new_string(str_size);
  strncpy(atom_string, type_string, str_size);
  struct BaseCell *car = new_integer(secd_machine, easy_hash_value(atom_string));
  struct BaseCell *cdr = new_atom(secd_machine, atom_string);
  struct BaseCell *new_cell = new_list(secd_machine, car, cdr);

  set_type_next(secd_machine, new_cell);
}

void extendtype_with_value(struct SECD *secd_machine, char *type_string, int64_t value) {
  int16_t str_size = strlen(type_string);
  char *atom_string = new_string(str_size);
  strncpy(atom_string, type_string, str_size);
  struct BaseCell *car = new_integer(secd_machine, value);
  struct BaseCell *cdr = new_atom(secd_machine, atom_string);
  struct BaseCell *new_cell = new_list(secd_machine, car, cdr);

  set_type_next(secd_machine, new_cell);
}

void basetype(struct SECD *secd_machine) {
  extendtype_with_value(secd_machine, "bottom!", TYPE_BOTTOM);
  extendtype(secd_machine, "atom!");
  extendtype(secd_machine, "int!");
  extendtype(secd_machine, "func!");
}

char *type_of(struct SECD *secd_machine, int64_t type_value) {
  struct BaseCell *current = secd_machine->type_pool_bottom;
  struct BaseList *list = NULL;
  struct BaseCell *car = NULL;
  struct BaseCell *cdr = NULL;

  while(current != NULL) {
    list = current->content.list;
    car = list->car;
    cdr = list->cdr;

    if(car->content.integer == type_value) {
      return cdr->content.string;
    }
    current = current->next;
  }
  return NULL;
}

int64_t type_of_value(struct SECD *secd_machine, char *type) {
  struct BaseCell *current = secd_machine->type_pool_bottom;
  struct BaseList *list = NULL;
  struct BaseCell *car = NULL;
  int64_t result = easy_hash_value(type);

  while(current != NULL) {
    list = current->content.list;
    car = list->car;

    if(car->content.integer == result) {
      return result;
    }
    current = current->next;
  }
  return 0;
}
