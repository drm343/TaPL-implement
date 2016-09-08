#include "secd_struct.h"

#define SECD_MACHINE_NS(name) SECD_MACHINE_##name

// kernel command
struct BaseCell *new_nil(void);
void set_stack_next(struct BaseCell *);
struct BaseCell *pop_code_next(void);
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
void debug_top_code(void);
void run(void);
