#pragma once

#include "structures.h"
#include "structures/bplustree/bplustree.h"

typedef struct {
  int key;
  int value;
} IntIntBPlusTree;

static inline int int_cmp(const int a, const int b) {
  return (a > b) - (a < b);
}
#define CMP_INT(a, b) int_cmp((a), (b))
DEFINE_BTREE(intinttree, IntIntBPlusTree, int, key, 4, CMP_INT)

STRUCTURES_EXTERN void printIntIntBPlusTree(IntIntBPlusTree *, void *);
