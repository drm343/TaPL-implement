#ifndef SECD_STRUCT

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define SECD_STATUS_NS(name) SECD_STATUS_##name
#define PRIMITIVE_NS(name) PRIMITIVE_##name

// compile type
enum COMPILE_PRIMITIVE {
  PRIMITIVE_NS(start) = 0,
  PRIMITIVE_NS(find_type_start),
  PRIMITIVE_NS(skip_space),
  PRIMITIVE_NS(input_type),
  PRIMITIVE_NS(return_type),
  PRIMITIVE_NS(find_type_stop),
  PRIMITIVE_NS(end)
};

// machine type
struct BaseCell;
struct SECD;

struct BaseList {
  struct BaseCell *car;
  struct BaseCell *cdr;
};

enum Type {
  NIL = 0,
  INTEGER,
  ATOM,
  UNCHECK_FUNC,
  DUMP,
  FUNC,
  C_FUNC,
  TYPE,
  LIST
};

union Content {
  int64_t integer;
  struct BaseList *list;
  struct BaseCell *item;
  char *string;
  void (*func)(struct SECD *);
};

struct BaseCell {
  enum Type type;
  union Content content;
  struct BaseCell *next;
};

enum SECD_STATUS {
  SECD_STATUS_NS(STOP) = 0,
  SECD_STATUS_NS(CONTINUE),
  SECD_STATUS_NS(ERROR)
};

struct SECD {
  struct BaseCell *s;
  struct BaseCell *bottom;
  struct BaseCell *code;
  struct BaseCell *code_bottom;
  char *tmp_code;
  struct BaseCell *env;
  struct BaseCell *env_bottom;
  struct BaseCell *dump;
  struct BaseCell *dump_bottom;

  struct BaseCell *cell_pool;
  struct BaseCell *cell_pool_top;

  struct BaseCell *list_pool;
  struct BaseCell *list_pool_top;

  enum SECD_STATUS status;

  void (*send)(struct SECD *, char *);
  void (*recv)(struct SECD *);
};

// interpreter type
enum StopType {
  CONTINUE = 0,
  FIND,
  TOO_MORE
};

#define SECD_STRUCT 1
#endif
