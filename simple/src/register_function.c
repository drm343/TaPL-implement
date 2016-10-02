#include "secd_struct.h"
#include "secd_machine.h"

#define ADD_FUNCTION(name, func) add_function(name, sizeof(name), func);


// primitive code
void hello(void) {
  printf("hello\n");
}

void nil(void) {
  struct BaseCell *cell = new_nil();
  set_stack_next(cell);
}

void add(void) {
  int64_t a = 0;
  int64_t b = 0;

  struct BaseCell *first = pop_code_next();
  run_integer(first, "first argument is null");

  struct BaseCell *second = pop_code_next();
  run_integer(second, "second argument is null");

  second = pop_stack_next();
  first = pop_stack_next();

  a = first->content.integer;
  b = second->content.integer;

  drop_cell(first);
  drop_cell(second);

  printf("%" PRId64 " + %" PRId64 " = %" PRId64 "\n", a, b, a + b);
  return ldc(a + b);
}

void debug(void) {
  debug_code();
  debug_stack();
}

// main code
void register_function(void) {
  ADD_FUNCTION("nil", nil);
  ADD_FUNCTION("hello", hello);
  ADD_FUNCTION("add", add);
  ADD_FUNCTION("debug", debug);
}

int main(void) {
  init_machine();
  register_function();
  run();
  stop_machine();
  return 1;
}
