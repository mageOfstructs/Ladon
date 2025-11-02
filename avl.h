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
typedef void (*avl_print_t)(void *);

enum TREE_SIDE { LEFT, RIGHT };

#define TREE_NODE_DEF(type, name)                                              \
  typedef struct tree_node_##name {                                            \
    struct tree_node_##name *left;                                             \
    struct tree_node_##name *right;                                            \
    type val;                                                                  \
  } tree_node_##name##_t;                                                      \
  typedef int (*avl_cmp_##name)(type *, type *);                               \
  typedef void (*avl_print_##name)(type *);

// void append_ordered(tree_node_t *root, tree_node_t *n,
//                     int (*cmp)(void *, void *));
// void dbg_tree(tree_node_t *root, int tabs, void (*tostr)(void *));

#define TREE_NODE_INIT                                                         \
  (tree_node_t) { NULL, NULL, NULL }

#define TREE_NODE_GEN_INIT(name, defval)                                       \
  (tree_node_##name##_t) { NULL, NULL, defval }

#endif // !AVL_H
