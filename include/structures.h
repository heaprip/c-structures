#pragma once

#define STRUCTURES_EXTERN extern __attribute__((visibility("default")))
#define STRUCTURES_INTERN extern __attribute__((visibility("hidden")))

unsigned int getrandom_uint(void);
