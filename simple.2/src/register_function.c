#include "secd_struct.h"
#include "secd_machine.h"
#include "secd_debug.h"
#include "secd_memory.h"

#define ADD_FUNCTION(machine, name, func) add_primitive(machine, name, sizeof(name), func);
#define RUN_INTEGER(machine, cell, msg) \
  if (!run_integer(machine, cell, msg)) {\
    return;\
  }
#define RUN_MACHINE(machine) machine->run(machine)
#define SEND_MACHINE(machine, message) machine->send(machine, message)
#define RECV_MACHINE(machine) machine->recv(machine)


// primitive code
void hello(struct SECD *machine) {
  printf("hello\n");
}

void nil(struct SECD *machine) {
  struct BaseCell *cell = new_nil(machine);
  set_stack_next(machine, cell);
}

void is_atom(struct SECD *machine) {
  struct BaseCell *cell = pop_code_next(machine);

  if(cell == NULL) {
    SECD_MACHINE_NS(error)(machine, "no value");
    return;
  }
  else if(cell->type == ATOM) {
    printf("%s is atom\n", cell->content.string);
    drop_atom(machine, cell);
  }
  else if(cell->type == NIL) {
    drop_integer(machine, cell);
    SECD_MACHINE_NS(error)(machine, "not atom");
    return;
  }
  else if(cell->type == INTEGER) {
    drop_integer(machine, cell);
    SECD_MACHINE_NS(error)(machine, "not atom");
    return;
  }
  else if(cell->type == LIST) {
    drop_list(machine, cell);
    SECD_MACHINE_NS(error)(machine, "not atom");
    return;
  }
}

void add(struct SECD *machine) {
  int64_t a = 0;
  int64_t b = 0;

  struct BaseCell *first = pop_code_next(machine);
  RUN_INTEGER(machine, first, "first argument is not exist");

  struct BaseCell *second = pop_code_next(machine);
  if (!run_integer(machine, second, "second argument is not exist")) {
    drop_cell(machine, pop_stack_next(machine));
    return;
  }

  second = pop_stack_next(machine);
  first = pop_stack_next(machine);

  a = first->content.integer;
  b = second->content.integer;

  drop_cell(machine, first);
  drop_cell(machine, second);

  printf("%" PRId64 " + %" PRId64 " = %" PRId64 "\n", a, b, a + b);
  ldc(machine, a + b);
  return;
}

void debug(struct SECD *machine) {
  debug_code(machine);
  debug_stack(machine);
}

// main code
void register_function(struct SECD *machine) {
  ADD_FUNCTION(machine, "nil: [ -> bottom! ]", nil);
  ADD_FUNCTION(machine, "atom?: [ any! -> bool! ]", is_atom);
  ADD_FUNCTION(machine, "hello: [ -> bottom! ]", hello);
  ADD_FUNCTION(machine, "add: [ int! int! -> int! ]", add);
  ADD_FUNCTION(machine, "debug: [ -> bottom! ]", debug);
}

void run(struct SECD *secd_machine) {
  char command[CMD_BUFFER_SIZE];
  memset(command, 0, CMD_BUFFER_SIZE);

  while(secd_machine->status) {
    memset(command, 0, CMD_BUFFER_SIZE);
    printf(")> ");

    /* Required on Windows. */
    fflush(stdout);
    fgets(command, CMD_BUFFER_SIZE, stdin); 

    for(int16_t next = 0; next < CMD_BUFFER_SIZE; next++) {
      if(command[next] == '\n') {
        command[next] = ' ';
      }
    }

    SEND_MACHINE(secd_machine, command);
    if(secd_machine->status != SECD_STATUS_NS(STOP)) {
      RECV_MACHINE(secd_machine);
    }
  }
}

int main(void) {
  struct SECD *machine = init_machine(register_function);
  run(machine);
  stop_machine(machine);
  return 0;
}