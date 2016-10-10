#include "secd_struct.h"

char *debug_new_string(int16_t, char *);
char *new_string(int16_t);
struct BaseCell *new_nil(void);
struct BaseCell *new_integer(int64_t);
struct BaseCell *new_atom(char *);
struct BaseCell *new_type(char *);
struct BaseCell *new_function(struct BaseCell *, struct BaseCell *);
struct BaseCell *new_c_function(void (*)(void));
struct BaseCell *new_dump(struct BaseCell *);
struct BaseCell *new_list(struct BaseCell *, struct BaseCell *);

#ifdef DEBUG
struct BaseCell *new_uncheck_function(struct BaseCell *, struct BaseCell *, int64_t);
struct BaseCell *debug_new_uncheck_function(struct BaseCell *, struct BaseCell *);
#else
struct BaseCell *new_uncheck_function(struct BaseCell *, struct BaseCell *);
#endif

void free_list(struct BaseList *);
void free_uncheck_function(struct BaseList *);
void free_stack(struct BaseCell *);

void drop_atom(struct BaseCell *);
void drop_integer(struct BaseCell *);
void drop_list(struct BaseCell *);
void drop_cell(struct BaseCell *);
