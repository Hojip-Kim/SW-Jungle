#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  // color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} bstree;

bstree *new_bstree(void);
void delete_bstree(bstree *);

node_t *bstree_insert(bstree *, const key_t);
node_t *bstree_find(const bstree *, const key_t);
node_t *bstree_min(const bstree *);
node_t *bstree_max(const bstree *);
int bstree_erase(bstree *, node_t *);
void rbtree_rotate_right(bstree *, node_t *);

int bstree_to_array(const bstree *, key_t *, const size_t);

// custom functions
void inorder(node_t *, key_t *, int *, int);
void delete_nodes(node_t *);
void exception(char *);

// * main function
int main() {
  bstree *tree = NULL;

  int n = 10;
  key_t *arr = malloc(n * sizeof(key_t));
  if (arr == NULL) exception("memory allocation error!\n");

  char cmd[10];
  int key;

  while (1) {
    printf("command: ");
    scanf("%s", cmd);

    if (!strcmp(cmd, "end")) {
      break;
    }
    if (strcmp(cmd, "create") && !tree) {
      printf("create tree first\n");
      continue;
    }

    if (!strcmp(cmd, "create")) {
      tree = new_bstree();
      printf("tree created: %p\n", tree);
    } else if (!strcmp(cmd, "insert")) {
      scanf(" %d", &key);
      node_t *new = bstree_insert(tree, key);
      if (!new) continue;
      printf("node inserted: %d %p\n", new->key, new);
    } else if (!strcmp(cmd, "find")) {
      scanf(" %d", &key);
      node_t *found = bstree_find(tree, key);
      if (!found)
        printf("no such key\n");
      else
        printf("node found: %d %p\n", found->key, found);
    } else if (!strcmp(cmd, "min")) {
      node_t *min = bstree_min(tree);
      if (!min) continue;
      printf("min found: %d %p\n", min->key, min);
    } else if (!strcmp(cmd, "max")) {
      node_t *max = bstree_max(tree);
      if (!max) continue;
      printf("min found: %d %p\n", max->key, max);
    } else if (!strcmp(cmd, "erase")) {
      scanf(" %d", &key);
      node_t *tar = bstree_find(tree, key);
      if (tar) {
        bstree_erase(tree, tar);
        printf("erased\n");
      } else {
        printf("no such key to delete\n");
      }
    } else if (!strcmp(cmd, "root")) {
      if (!tree) continue;
      if (!tree->root) continue;
      printf("root %d %p\n", tree->root->key, tree->root);
    } else if (!strcmp(cmd, "rotate")) {
      if (!tree) continue;
      if (!tree->root) continue;
      rbtree_rotate_right(tree, tree->root);
    } else {
      printf("** use command **\n");
      printf("create delete insert find min max erase array\n");
    }
    if (tree && tree->root) {
      int num = bstree_to_array(tree, arr, 10);
      for (int i = 0; i < num; i++) {
        printf("%d ", arr[i]);
      }
      printf("\n");
    }
  }

  // free nodes & tree
  if (tree) {
    delete_bstree(tree);
  }

  free(arr);

  return 0;
}

// * rotate right
void rbtree_rotate_right(bstree *tree, node_t *x) {
  // x를 기준으로 오른쪽으로 로테이션한다
  // y는 x의 left child
  node_t *y = x->left;

  // l sub of x -> r sub of y
  x->left = y->right;
  if (y->right != tree->nil) {
    y->right->parent = x;  // connect if child exist
  }

  y->parent = x->parent;  // connect parent
  if (x->parent == tree->nil) {
    // x is root
    tree->root = y;
  } else if (x == x->parent->left) {
    // x is left child
    x->parent->left = y;
  } else {
    // x is right child
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
  return;
}

// * create tree
bstree *new_bstree(void) {
  bstree *tree = malloc(sizeof(bstree));
  tree->root = NULL;
  tree->nil = NULL;
  return tree;
}

// * insert node
node_t *bstree_insert(bstree *tree, const key_t key) {
  node_t *new = malloc(sizeof(node_t));
  if (new == NULL) exception("memory allocation error");

  new->key = key;
  new->left = NULL;
  new->right = NULL;

  if (tree->root == NULL) {
    tree->root = new;
    new->parent = NULL;
    return new;
  }
  node_t *curr = tree->root;
  while (curr) {
    if (key < curr->key) {
      if (!curr->left) {
        new->parent = curr;
        curr->left = new;
        break;
      }
      curr = curr->left;
    } else {
      if (!curr->right) {
        new->parent = curr;
        curr->right = new;
        break;
      }
      curr = curr->right;
    }
  }
  return new;
}

// * find key
node_t *bstree_find(const bstree *tree, const key_t key) {
  if (!tree) return NULL;
  if (!tree->root) return NULL;

  node_t *curr = tree->root;
  while (curr) {
    if (key > curr->key) {
      curr = curr->right;
    } else if (key < curr->key) {
      curr = curr->left;
    } else {
      // found
      break;
    }
  }
  return curr;
}

// * find min key
node_t *bstree_min(const bstree *tree) {
  if (!tree) return NULL;
  if (!tree->root) return NULL;

  node_t *curr = tree->root;
  while (curr) {
    curr = curr->left;
    if (!curr->left) {
      break;
    }
  }
  return curr;
}

// * find max key
node_t *bstree_max(const bstree *tree) {
  if (!tree) return NULL;
  if (!tree->root) return NULL;

  node_t *curr = tree->root;
  while (curr) {
    curr = curr->right;
    if (!curr->right) {
      break;
    }
  }
  return curr;
}

// * delete node by ptr
int bstree_erase(bstree *tree, node_t *ptr) {
  if (!tree) return -1;
  if (!tree->root) return -1;

  // * CASE1: no child
  if (!ptr->left && !ptr->right) {
    printf("case 1: %d %p\n", ptr->key, ptr);
    // i am leaf node to delete!
    if ((ptr->parent)) {
      // not a root
      if ((ptr->parent)->left == ptr) {  // left
        (ptr->parent)->left = NULL;
      } else if ((ptr->parent)->right == ptr) {  // right
        (ptr->parent)->right = NULL;
      }
    } else {
      // tree points null if root deleted
      tree->root = NULL;
    }
    free(ptr);
    return 0;
  }

  // * CASE2: single child
  if (!ptr->left && ptr->right) {
    printf("case 2(right): %d %p %d\n", ptr->key, ptr, ptr->right->key);
    // i am nonleaf node with single right child
    if (ptr->parent) {
      // not a root: 내가 left인지 right인지 판단해서 parent에 붙여줘야 함
      if ((ptr->parent)->right == ptr) {
        (ptr->parent)->right = ptr->right;
      } else if ((ptr->parent)->left == ptr) {
        (ptr->parent)->left = ptr->right;
      }
    } else {
      // root
      tree->root = ptr->right;
    }
    (ptr->right)->parent = ptr->parent;
    free(ptr);
    return 0;
  }
  if (ptr->left && !ptr->right) {
    printf("case 2(left): %d %p %d\n", ptr->key, ptr, ptr->left->key);
    // i am nonleaf node with single left child
    if (ptr->parent) {
      // not a root: 내가 left인지 right인지 판단해서 parent에 붙여줘야 함
      if ((ptr->parent)->right == ptr) {
        (ptr->parent)->right = ptr->left;
      } else if ((ptr->parent)->left == ptr) {
        (ptr->parent)->left = ptr->left;
      }
    } else {
      // root
      tree->root = ptr->left;
    }
    (ptr->left)->parent = ptr->parent;
    free(ptr);
    return 0;
  }

  // ! i am nonleaf node with two children
  // 1. choose largest in left subtree
  // 2. replace with me
  // 3. delete the largest

  // * CASE3: both children
  node_t *curr = ptr->left;
  while (curr) {
    if (!curr->right) break;
    curr = curr->right;
  }

  printf("case 3: %d %p curr: %d\n", ptr->key, ptr, curr->key);

  ptr->key = curr->key;  // replace

  // curr를 삭제하고, 나머지를 이어준다
  if ((curr->parent)->right == curr) {
    if ((curr->left)) {
      (curr->parent)->right = curr->left;  // can have left
      (curr->left)->parent = curr->parent;
    } else {
      (curr->parent)->right = NULL;  // or not
    }

  } else if ((curr->parent)->left == curr) {
    if ((curr->left)) {
      (curr->parent)->left = curr->left;  // can have left
      (curr->left)->parent = curr->parent;
    } else {
      (curr->parent)->left = NULL;  // or not
    }
  }

  free(curr);
  return 0;
}

// * delete bstree
void delete_bstree(bstree *tree) {
  if (!tree) {
    return;
  }
  // delete nodes first
  delete_nodes(tree->root);

  // and delete tree
  free(tree);
  return;
}

// * delete all nodes
void delete_nodes(node_t *root) {
  if (!root) {
    return;
  }
  delete_nodes(root->left);
  delete_nodes(root->right);
  printf("free %d %p\n", root->key, root);
  free(root);
  return;
}

// * bstree to array
int bstree_to_array(const bstree *tree, key_t *arr, const size_t n) {
  // 오름차순으로 정렬된 arr 반환
  if (!tree) return 0;
  if (!tree->root) return 0;
  int idx = 0;
  inorder(tree->root, arr, &idx, n);
  return idx;  // 길이를 반환
}

// * inorder traverse tree
void inorder(node_t *root, key_t *arr, int *idx, int n) {
  if (!root) {
    return;
  }
  if (*idx == n) {
    return;
  }
  // *(arr + (*idx)++) = root->key;
  inorder(root->left, arr, idx, n);
  *(arr + (*idx)++) = root->key;
  inorder(root->right, arr, idx, n);
}

// * util functions
void exception(char *message) {
  printf("%s\n", message);
  exit(-1);
}