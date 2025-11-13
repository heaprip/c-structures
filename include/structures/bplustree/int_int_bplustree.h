#include "structures.h"
#include "bplustree.h"
#include<uv.h>

struct IntIntBPlusTree
{
  int key;
  int value;
};

static inline int int_cmp(const int a, const int b)
{
  return (a > b) - (a < b);
}
#define CMP_INT(a, b) int_cmp((a), (b))
DEFINE_BTREE(intinttree, struct IntIntBPlusTree, int, key, 4, CMP_INT)

STRUCTURES_EXTERN void printIntIntBPlusTree(struct IntIntBPlusTree *, void *);
