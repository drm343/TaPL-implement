#include "secd_debug.h"

void debug_code(struct SECD *secd_machine) {
  struct BaseCell *cell = secd_machine->code_bottom;

  printf("[all code]: ");
  while(cell != NULL) {
    if(cell->type == NIL) {
      printf("nil:nil! ");
    }
    else if(cell->type == INTEGER) {
      printf("%" PRId64 ":int! ", cell->content.integer);
    }
    else if(cell->type == ATOM) {
      printf("%s:atom! ", cell->content.string);
    }
    else if(cell->type == UNCHECK_VAR) {
      struct BaseList *list = cell->content.list;
      struct BaseCell *car = list->car;
      printf("%s:var! ", car->content.string);
    }
    else if(cell->type == UNCHECK_FUNC) {
      struct BaseList *list = cell->content.list;
      struct BaseCell *func = list->cdr;
      list = func->content.list;
      func = list->car;
      printf("%s:func! ", func->content.string);
    }
    else if(cell->type == DUMP) {
      struct BaseCell *item = cell->content.item;
      printf("%s:dump! ", item->content.string);
    }
    else {
      debug_item(cell, 0);
    }

    cell = cell->next;
  }
  printf("\n");
}

void debug_stack(struct SECD *secd_machine) {
  struct BaseCell *cell = secd_machine->s;

  printf("[all stack]: ");
  while(cell != NULL) {
    if(cell->type == NIL) {
      printf("nil:nil! ");
    }
    else if(cell->type == INTEGER) {
      printf("%" PRId64 ":int! ", cell->content.integer);
    }
    else if(cell->type == ATOM) {
      printf("%s:atom! ", cell->content.string);
    }
    else if(cell->type == TYPE) {
      printf("%s:type! ", cell->content.string);
    }
    else {
      printf("undef:bottom! ");
    }

    cell = cell->next;
  }
  printf("\n");
}

void debug_struct(struct BaseCell *cell) {
#ifdef DEBUG
  printf("[struct]:start\n");
  while(cell != NULL) {
    if(cell->type == NIL) {
      printf("nil:nil!\n");
    }
    else if(cell->type == INTEGER) {
      printf("%" PRId64 ":int!\n", cell->content.integer);
    }
    else if(cell->type == ATOM) {
      printf("%s:atom!\n", cell->content.string);
    }
    else if(cell->type == UNCHECK_FUNC) {
      struct BaseList *list = cell->content.list;
      struct BaseCell *func = list->car;
      printf("%s:func!\n", func->content.string);
    }
    else if(cell->type == DUMP) {
      struct BaseCell *item = cell->content.item;
      printf("%s:dump!\n", item->content.string);
    }
    else {
      debug_item(cell, 0);
    }

    cell = cell->next;
  }
  printf("[struct]:done\n\n");
#endif
}

void debug_item(struct BaseCell *cell, int16_t count) {
#ifdef DEBUG
  if(cell == NULL) {
    printf("%d null\n", count);
  }
  else if(cell->type == NIL) {
    printf("%d nil:nil!\n", count);
  }
  else if(cell->type == INTEGER) {
    printf("%d %" PRId64 ":int!\n", count, cell->content.integer);
  }
  else if(cell->type == ATOM) {
    printf("%d %s:atom!\n", count, cell->content.string);
  }
  else if(cell->type == TYPE) {
    printf("%d %s:type!\n", count, cell->content.string);
  }
  else if(cell->type == UNCHECK_FUNC) {
    struct BaseList *list = cell->content.list;
    struct BaseCell *car = list->car;
    struct BaseCell *cdr = list->cdr;

    printf("%d uncheck_func!\n", count);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("-> car ");
    debug_item(car, count + 1);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("-> cdr ");
    debug_item(cdr, count + 1);
  }
  else if(cell->type == UNCHECK_VAR) {
    struct BaseList *list = cell->content.list;
    struct BaseCell *car = list->car;
    struct BaseCell *cdr = list->cdr;

    printf("%d uncheck_var!\n", count);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("-> car ");
    debug_item(car, count + 1);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("-> cdr ");
    debug_item(cdr, count + 1);
  }
  else if(cell->type == LIST) {
    struct BaseList *list = cell->content.list;
    struct BaseCell *car = list->car;
    struct BaseCell *cdr = list->cdr;

    printf("%d list!\n", count);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("  -> car ");
    debug_item(car, count + 1);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("  -> cdr ");
    debug_item(cdr, count + 1);
  }
  else if(cell->type == FUNC) {
    printf("%d pointer:func! %p\n", count, (void *)(cell->content.func));
  }
  else if(cell->type == VAR) {
    struct BaseList *list = cell->content.list;
    struct BaseCell *car = list->car;
    struct BaseCell *cdr = list->cdr;

    printf("%d pointer:var! %p\n", count, cell->content.pointer);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("  -> car ");
    debug_item(car, count + 1);
    for(int16_t i = 0; i < count; i++) {
      printf("  ");
    }
    printf("  -> cdr ");
    debug_item(cdr, count + 1);
  }
  else {
    printf("%d undef\n", count);
  }
#endif
}
