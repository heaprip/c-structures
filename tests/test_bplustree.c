#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/errno.h>
#include <sys/random.h>

#include <cmocka.h>

#include "structures.h"
#include "structures/bplustree/int_int_bplustree.h"

static void test_b_plus_tree_init(void **_) {
  intinttree tree;
  intinttree_init(&tree);

  assert_null(tree.root);
  assert_non_null(&tree);
}

static void test_b_plus_tree_insert(void **_) {
  intinttree tree;
  intinttree_init(&tree);

  IntIntBPlusTree first_entry = {
      .key = 1, .value = 2}; // NOLINT(readability-magic-numbers)
  intinttree_insert(&tree, &first_entry);

  assert_non_null(tree.leaves.next);
}

void intintree_get_entry_string(IntIntBPlusTree *, void *);
void intintree_get_entry_string(IntIntBPlusTree *_, void *counter) {
  ++(*((long long *)counter));
}

static void test_b_plus_tree_iterate(void **_) {
  intinttree tree;
  intinttree_init(&tree);

  IntIntBPlusTree first_entry = {
      .key = 1, .value = 2}; // NOLINT(readability-magic-numbers)
  intinttree_insert(&tree, &first_entry);

  ssize_t count = 0;
  intinttree_iterate(&tree, intintree_get_entry_string, &count);

  assert_int_equal((int)count, 1);
}

// static void test_b_plus_tree_search(void **_) {
//   intinttree tree;
//   intinttree_init(&tree);

//   assert_non_null(&tree);
// }

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_b_plus_tree_init),
      cmocka_unit_test(test_b_plus_tree_insert),
      cmocka_unit_test(test_b_plus_tree_iterate),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
