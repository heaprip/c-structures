#pragma once

#define STRUCTURES_EXTERN extern __attribute__((visibility("default")))
#define STRUCTURES_INTERN extern __attribute__((visibility("hidden")))

#include "structures/bplustree/int_int_bplustree.h"

unsigned int getrandom_uint(void);
