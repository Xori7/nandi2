#ifndef NANDI_H
#define NANDI_H

#include "../build/config.h"

/*************************
Primitve types
*************************/
#include <stdint.h>
typedef int8_t I8;
typedef uint8_t U8;
typedef int16_t I16;
typedef uint16_t U16;
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;

/*************************
Module: NString
Source: nstring.c
*************************/
typedef char* NString;

NString nandi_string_get_empty();
NString nandi_string_get_from_cstring(char *cstring);

/*************************/

#endif //NANDI_H
