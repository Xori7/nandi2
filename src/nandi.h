#ifndef NANDI_H
#define NANDI_H

#include "../build/config.h"

/* Nandi String */
#ifndef NSTRING_IMPL
typedef char* NString;
#endif // NSTRING_IMPL

NString nandi_string_new();

#endif //NANDI_H
