#ifndef AVL_H
#define AVL_H

#include "printf.h"
#include "types.h"
#include <stdbool.h>

typedef struct tree_node {
  struct tree_node *left;
  struct tree_node *right;
  void *val;
} tree_node_t;

typedef int (*avl_cmp_t)(void *, void *);

enum TREE_SIDE { LEFT, RIGHT };
void append_ordered(tree_node_t *root, tree_node_t *n,
                    int (*cmp)(void *, void *));
void dbg_tree(tree_node_t *root, int tabs, void (*tostr)(void *));
void test_avl();

#define TREE_NODE_INIT                                                         \
  (tree_node_t) { NULL, NULL, NULL }

#endif // !AVL_H
