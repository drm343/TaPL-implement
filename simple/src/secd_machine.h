#include "secd_struct.h"

#define SECD_MACHINE_NS(name) SECD_MACHINE_##name

// kernel command
struct BaseCell *new_nil(void);
void set_stack_next(struct BaseCell *);
struct BaseCell *pop_code_next(void);
struct BaseCell *pop_stack_next(void);
void drop_atom(struct BaseCell *);
void drop_integer(struct BaseCell *);
void drop_list(struct BaseCell *);
void drop_cell(struct BaseCell *);

// user command
void ldc(int64_t);

// machine command
void add_function(char *, int16_t, void (*)(void));
void init_machine(void);
void SECD_MACHINE_NS(error)(char *);
void SECD_MACHINE_NS(recover_machine)(void);
void stop_machine(void);
void debug_stack(void);
void debug_code(void);
bool run_integer(struct BaseCell *, char *);
void run(void);
bool find_function(char *);
