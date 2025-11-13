#include "structures/bplustree/int_int_bplustree.h"

void printIntIntBPlusTree(struct IntIntBPlusTree *o, void *_) {
  printf("k=%d v=%d\n", o->key, o->value);
}
