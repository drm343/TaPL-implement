#include "secd_struct.h"

#define TYPE_BOTTOM 1878

int64_t easy_hash_value(char *);
void extendtype(struct SECD *, char *);
void basetype(struct SECD *);
char *type_of(struct SECD *, int64_t);
int64_t type_of_value(struct SECD *, char *);
