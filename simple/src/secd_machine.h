#include "secd_struct.h"

#define SECD_MACHINE_NS(name) SECD_MACHINE_##name

// kernel command
void set_stack_next(struct BaseCell *);
struct BaseCell *pop_code_next(void);
struct BaseCell *pop_stack_next(void);

// user command
void ldc(int64_t);

// machine command
void add_primitive(char *, int16_t, void (*)(void));
void init_machine(void);
void SECD_MACHINE_NS(error)(char *);
void SECD_MACHINE_NS(type_error)(char *);
void SECD_MACHINE_NS(recover_machine)(void);
void stop_machine(void);
bool run_integer(struct BaseCell *, char *);
void run(void);
