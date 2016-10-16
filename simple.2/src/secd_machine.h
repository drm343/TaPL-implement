#include "secd_struct.h"

#define SECD_MACHINE_NS(name) SECD_MACHINE_##name
#define CMD_BUFFER_SIZE 1024

// kernel command
void set_stack_next(struct SECD *, struct BaseCell *);
struct BaseCell *pop_code_next(struct SECD *);
struct BaseCell *pop_stack_next(struct SECD *);
void compile_code(struct SECD *, int16_t, char *);
void run_code(struct SECD *);

// user command
void ldc(struct SECD *, int64_t);

// machine command
void add_primitive(struct SECD *, char *, int16_t, void (*)(struct SECD *));
struct SECD *init_machine(void (*)(struct SECD *));
void SECD_MACHINE_NS(error)(struct SECD *, char *);
void SECD_MACHINE_NS(type_error)(struct SECD *, char *);
void SECD_MACHINE_NS(recover_machine)(struct SECD *);
void stop_machine(struct SECD *);
bool run_integer(struct SECD *, struct BaseCell *, char *);
