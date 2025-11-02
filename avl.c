#include "avl.h"
#include "log.h"
#include "utils.h"

static bool inline is_leaf(tree_node_t *node) {
  return !node->left && !node->right;
}

int leaf_cnt(tree_node_t *root) {
  if (!root)
    return 0;
  if (is_leaf(root))
    return 1;
  return leaf_cnt(root->left) + leaf_cnt(root->right);
}

void dbg_tree(tree_node_t *root, int tabs, avl_print_t tostr) {
  for (int i = 0; i < tabs; i++)
    printf("\t");
  printf(" ");
  if (!tostr)
    printf("{%p}\n", root->val);
  else
    tostr(&root->val);
  if (root->left) {
    printf("L");
    dbg_tree(root->left, tabs + 1, tostr);
  }
  if (root->right) {
    printf("R");
    dbg_tree(root->right, tabs + 1, tostr);
  }
}

tree_node_t *__get_side(tree_node_t *node, enum TREE_SIDE dir) {
  switch (dir) {
  case LEFT:
    return node->left;
  case RIGHT:
    return node->right;
  }
}

tree_node_t *__set_side(tree_node_t *node, enum TREE_SIDE dir,
                        tree_node_t *newchild) {
  switch (dir) {
  case LEFT:
    return node->left = newchild;
  case RIGHT:
    return node->right = newchild;
  }
}

void append(tree_node_t *root, tree_node_t *new, enum TREE_SIDE dir) {
  if (!__get_side(root, dir)) {
    __set_side(root, dir, new);
    return;
  }

  append(__get_side(root, dir), new, dir);
}

void append_ordered(tree_node_t *root, tree_node_t *new, avl_cmp_t cmp) {
  if (!root || !new)
    return;
  int comp_res = cmp(&root->val, &new->val);
  if (comp_res < 0) {
    if (root->right) {
      append_ordered(root->right, new, cmp);
    } else
      root->right = new;
  } else {
    if (root->left) {
      append_ordered(root->left, new, cmp);
    } else
      root->left = new;
  }
}

/**
expects a sorted binary tree
**/
tree_node_t *get_min(tree_node_t *root) {
  if (root->left)
    return get_min(root->left);
  else
    return root;
}

/**
expects a sorted binary tree
**/
tree_node_t *get_max(tree_node_t *root) {
  if (root->right)
    return get_max(root->right);
  else
    return root;
}

void remove(tree_node_t *prev, tree_node_t *node_to_remove, avl_cmp_t cmp) {
  if (prev->left == node_to_remove) {
    prev->left = node_to_remove->left;
    append_ordered(prev->left, node_to_remove->right, cmp);
  } else if (prev->right == node_to_remove) {
    prev->right = node_to_remove->left;
    append_ordered(prev->right, node_to_remove->right, cmp);
  }

  // prev didn't have a pointer to node_to_remove
  KASSERT(false);
}

tree_node_t *get_smallest_tree(tree_node_t *root, enum TREE_SIDE dir) {
  tree_node_t *ret = root;
  tree_node_t *cur = __get_side(root, dir);
  while (__get_side(cur, dir)) {
    ret = cur;
    cur = __get_side(cur, dir);
  }
  return ret;
}

tree_node_t *avl(tree_node_t *root) {
  if (!root)
    return NULL;
  if (root->left)
    root->left = avl(root->left);
  if (root->right)
    root->right = avl(root->right);

  tree_node_t *ret = root;
  if (root->left && root->left->left && !root->left->right) {
    root->left->right = root;
    ret = root->left;
    root->left = NULL;
    root = ret;
  }

  if (root->right && root->right->right && !root->right->left) {
    root->right->left = root;
    ret = root->right;
    root->right = NULL;
    return ret;
  }
  return ret;
}

// void test_avl() {
//   tree_node_t root = TREE_NODE_INIT, l1 = TREE_NODE_INIT, l2 =
//   TREE_NODE_INIT,
//               r1 = TREE_NODE_INIT, r2 = TREE_NODE_INIT, r3 = TREE_NODE_INIT;
//   root.val = (void *)1;
//   l1.val = (void *)0;
//   l2.val = (void *)-1;
//   r1.val = (void *)2;
//   r2.val = (void *)3;
//   r3.val = (void *)4;
//   root.left = &l1;
//   l1.left = &l2;
//   root.right = &r1;
//   r1.right = &r2;
//   r2.right = &r3;
//   dbg_tree(&root, 0, NULL);
//   tree_node_t *nroot = avl(&root);
//   log("After AVL\n");
//   dbg_tree(nroot, 0, NULL);
// }
