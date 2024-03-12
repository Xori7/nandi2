#include "nandi.h"
#include <stdlib.h>

typedef struct {
    U32 length;
    char buffer[1];
} *INStringWrapped;

INStringWrapped nandi_string_wrapped_new(U32 length);
INStringWrapped nandi_string_wrapped_new(U32 length) {
    INStringWrapped stringWrapped = malloc(sizeof(*stringWrapped) + length);
    stringWrapped->length = length;
    stringWrapped->buffer[length] = 0;
    return stringWrapped;
}

NString nandi_string_get_empty() {
    INStringWrapped string;
    string = nandi_string_wrapped_new(0);
    return string->buffer;
}

NString nandi_string_get_from_cstring(char *cstring) {
    INStringWrapped string;
    string = nandi_string_wrapped_new(0);
    return string->buffer;
}
