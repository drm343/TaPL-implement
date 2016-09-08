#include <inttypes.h>
#include "secd_struct.h"
#include "secd_machine.h"

#define ADD_FUNCTION(name, func) add_function(name, sizeof(name) - 1, func);


int64_t S64(const char *s) {
  int64_t i;
  char c ;
  int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);

  if (scanned == 1) {
    return i;
  }

  if (scanned > 1) {
    // TBD about extra data found
    return i;
  }
  // TBD failed to scan;  
  return 0;  
}

// primitive code
void hello(void) {
  printf("hello\n");
}

void nil(void) {
  struct BaseCell *cell = new_nil();
  set_stack_next(cell);
}

void add(void) {
  struct BaseCell *first = pop_code_next();
  struct BaseCell *second = pop_code_next();
  int64_t a = 0;
  int64_t b = 0;

  if(first == NULL) {
    drop_cell(first);
    drop_cell(second);
    return SECD_MACHINE_NS(error)("first argument is null");
  }
  a = S64(first->content.string);

  if(second == NULL) {
    drop_cell(first);
    drop_cell(second);
    return SECD_MACHINE_NS(error)("first argument is null");
  }
  b = S64(second->content.string);

  printf("%d + %d = %d\n", a, b, a + b);
  return ldc(a + b);
}

// main code
void register_function(void) {
  ADD_FUNCTION("nil", nil);
  ADD_FUNCTION("hello", hello);
  ADD_FUNCTION("add", add);
}

int main(void) {
  init_machine();
  register_function();
  run();
  stop_machine();
  return 1;
}
