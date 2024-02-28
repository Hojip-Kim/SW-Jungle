#include <stdlib.h>

#include "rbtree.h"

// min or max
typedef enum { MIN, MAX } ext;

// rotate
void rbtree_rotate_left(rbtree *, node_t *);
void rbtree_rotate_right(rbtree *, node_t *);

// fixup and transplant
void rbtree_insert_fixup(rbtree *, node_t *);
void rbtree_delete_fixup(rbtree *, node_t *);
void rbtree_transplant(rbtree *, node_t *, node_t *);

// minmax
node_t *rbtree_minmax(const rbtree *, node_t *, ext);

// traverse
void postorder(const rbtree *, node_t *);
void inorder(const rbtree *, node_t *, key_t *, int *, int);

// * create new
rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));

  node_t *nil = malloc(sizeof(node_t));
  nil->color = RBTREE_BLACK;

  p->root = nil;
  p->nil = nil;
  return p;
}

// * free all
void delete_rbtree(rbtree *t) {
  if (!t) return;

  // delete nodes first
  postorder(t, t->root);

  // and delete tree
  free(t->nil);
  free(t);
}

// * insert node
node_t *rbtree_insert(rbtree *t, const key_t key) {
  node_t *new = malloc(sizeof(node_t));
  new->key = key;

  node_t *curr = t->root;
  node_t *prev = t->nil;
  while (curr != t->nil) {
    prev = curr;
    if (key < curr->key) {
      curr = curr->left;
    } else {
      curr = curr->right;
    }
  }

  new->parent = prev;
  if (prev == t->nil) {
    // means not repeated: new is root
    t->root = new;
  } else if (new->key < prev->key) {
    prev->left = new;
  } else {
    prev->right = new;
  }
  new->left = t->nil;
  new->right = t->nil;
  new->color = RBTREE_RED;
  rbtree_insert_fixup(t, new);
  return new;
}

// * find node
node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (!t) return NULL;
  if (!t->root) return NULL;

  node_t *curr = t->root;
  while (curr != t->nil && curr->key != key) {
    if (key > curr->key) {
      curr = curr->right;
    } else {
      curr = curr->left;
    }
  }
  if (curr != t->nil) {
    return curr;
  } else {
    return NULL;
  }
}

// * find min node
node_t *rbtree_min(const rbtree *t) {
  if (!t) return NULL;
  if (!t->root) return NULL;

  return rbtree_minmax(t, t->root, MIN);
}

// * find max node
node_t *rbtree_max(const rbtree *t) {
  if (!t) return NULL;
  if (!t->root) return NULL;

  return rbtree_minmax(t, t->root, MAX);
}

// * erase node
int rbtree_erase(rbtree *t, node_t *z) {
  if (!t) return -1;
  if (!t->root) return -1;

  node_t *y = z;             // y는 삭제할 노드를 지정
  node_t *x = y->right;      // y의 자리를 대체하는 노드
  color_t color = y->color;  // 삭제된 색을 체크

  if (z->left == t->nil) {  // child가 1개이거나 없는 경우
    x = z->right;
    rbtree_transplant(t, z, x);  // 당긴다
  } else if (z->right == t->nil) {
    x = z->left;
    rbtree_transplant(t, z, x);  // 당긴다
  } else {                       // child가 2개
    y = rbtree_minmax(t, z->left, MAX);
    color = y->color;
    x = y->left;  // y의 자식은 x가 유일 (nil일 수 있음)

    if (y->parent == z)
      x->parent = y;  // 이외의 경우에는 모두 y.parent
    else {
      rbtree_transplant(t, y, x);
      y->left = z->left;
      y->left->parent = y;
    }
    rbtree_transplant(t, z, y);

    y->right = z->right;
    y->right->parent = y;
    y->color = z->color;
  }
  if (color == RBTREE_BLACK) {
    rbtree_delete_fixup(t, x);
  }
  free(z);
  return 0;
}

// * rbrtree to array
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // 오름차순으로 정렬된 arr 반환
  if (!t) return 1;
  if (!t->root) return 1;
  int idx = 0;
  inorder(t, t->root, arr, &idx, n);
  return 0;
}

// * min max
node_t *rbtree_minmax(const rbtree *t, node_t *root, ext code) {
  if (!t) return NULL;
  if (!t->root) return NULL;

  node_t *curr = root;
  if (code == MIN) {
    while (curr) {
      if (curr->left == t->nil) break;
      curr = curr->left;
    }
  } else {
    while (curr) {
      if (curr->right == t->nil) break;
      curr = curr->right;
    }
  }
  return curr;
}

// * insert fixup
void rbtree_insert_fixup(rbtree *tree, node_t *z) {
  // z를 삽입하고 나서 recoloring & rotation 수행
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left) {
      node_t *u = z->parent->parent->right;

      if (u->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;

        z = z->parent->parent;
      } else if (z->parent->right == z) {
        // i am left child
        z = z->parent;
        rbtree_rotate_left(tree, z);
      } else {
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_rotate_right(tree, z->parent->parent);
      }

    } else {
      node_t *u = z->parent->parent->left;

      if (u->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;

        z = z->parent->parent;
      } else if (z->parent->left == z) {
        // i am left child
        z = z->parent;
        rbtree_rotate_right(tree, z);
      } else {
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        rbtree_rotate_left(tree, z->parent->parent);
      }
    }
  }
  tree->root->color = RBTREE_BLACK;
}

// * delete fixup
void rbtree_delete_fixup(rbtree *t, node_t *x) {
  // y를 대체하는 노드 x가 rb의 성질을 깬다면, 수정한다
  node_t *w = x->parent->right;  // 초기화

  while (x != t->root && x->color == RBTREE_BLACK) {
    if (x->parent->left == x) {
      w = x->parent->right;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rbtree_rotate_left(t, x->parent);
        w = x->parent->right;

      } else if (w->left->color == RBTREE_BLACK &&
                 w->right->color == RBTREE_BLACK) {
        // w의 자식이 BB
        w->color = RBTREE_RED;
        x = x->parent;

      } else if (w->right->color == RBTREE_BLACK) {
        // w의 자식이 RB
        w->color = RBTREE_RED;
        w->left->color = RBTREE_BLACK;
        rbtree_rotate_right(t, w);
        w = x->parent->right;

      } else {
        // w의 자식이 RR 혹은 BR
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        rbtree_rotate_left(t, x->parent);
        x = t->root;  // extra black resolved
      }
    } else {
      w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rbtree_rotate_right(t, x->parent);
        w = x->parent->left;
      } else if (w->left->color == RBTREE_BLACK &&
                 w->right->color == RBTREE_BLACK) {
        // w의 자식이 BB
        w->color = RBTREE_RED;
        x = x->parent;
      } else if (w->left->color == RBTREE_BLACK) {
        // w의 자식이 RB
        w->color = RBTREE_RED;
        w->right->color = RBTREE_BLACK;
        rbtree_rotate_left(t, w);
        w = x->parent->left;
      } else {
        // w의 자식이 RR 혹은 BR
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        rbtree_rotate_right(t, x->parent);
        x = t->root;  // extra black resolved
      }
    }
  }
  x->color = RBTREE_BLACK;
}

// * transplant
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (!t) return;
  if (!t->root) return;

  // u의 자리에 v를 옮긴다
  if (u->parent == t->nil) {
    t->root = v;  // u는 root
  } else if (u->parent->right == u) {
    u->parent->right = v;  // u는 left child
  } else if (u->parent->left == u) {
    u->parent->left = v;  // u는 right child
  }
  v->parent = u->parent;
  return;
}

// * rotate left
void rbtree_rotate_left(rbtree *tree, node_t *x) {
  // x를 기준으로 왼쪽으로 로테이션한다
  // y는 x의 right child
  node_t *y = x->right;

  // r sub of x -> l sub of y
  x->right = y->left;
  if (y->left != tree->nil) {
    y->left->parent = x;  // connect if child exist
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
  y->left = x;
  x->parent = y;
  return;
}

// * rotate right
void rbtree_rotate_right(rbtree *tree, node_t *x) {
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

// * postorder traverse to delete all
void postorder(const rbtree *t, node_t *root) {
  if (root == t->nil) {
    return;
  }
  postorder(t, root->left);
  postorder(t, root->right);
  free(root);
  return;
}

// * inorder traverse tree to sort
void inorder(const rbtree *t, node_t *root, key_t *arr, int *idx, int n) {
  if (root == t->nil) {
    return;
  }
  if (*idx == n) {
    return;
  }
  inorder(t, root->left, arr, idx, n);
  *(arr + (*idx)++) = root->key;
  inorder(t, root->right, arr, idx, n);
}