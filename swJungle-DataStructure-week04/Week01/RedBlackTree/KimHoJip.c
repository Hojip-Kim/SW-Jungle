#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  t->nil = nil;
  t->root = nil;
  // TODO: initialize struct if needed
  return t;
}

void post_order(rbtree *t, node_t *node){
  if (node == t->nil){
    return;
  }
  post_order(t, node->left);
  post_order(t, node->right);
  free(node);
  node = NULL;
  return;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  if(t == NULL){
    return;
  }
  post_order(t, t->root);
  free(t->nil); // nil까지 메모리 반납
  t->nil = NULL;
  
  free(t); // rbtree까지 메모리 반납
  t = NULL;
  return;
}

void left_rotate(rbtree *t, node_t *x){
  node_t *y = x->right;
  x->right = y->left;
  if(y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;

  if(x->parent == t->nil){
    t->root = y;
  }else if(x == x->parent->left){
    x->parent->left = y;
  }else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *x){
  node_t *y = x->left;
  x->left = y->right;
  if(y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if(x->parent == t->nil){
    t->root = y;
  }else if(x == x->parent->right){
    x->parent->right = y;
  }else{
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
}

void rbtree_insert_fixAll(rbtree *t, node_t *z){
/* 
1. 현재 insert된 노드의 parent color가 RBTREE_RED인 경우
  1) 현재 insert된 노드의 uncle node가 RBTREE_RED인경우
    ->recoloring
  2) 현재 insert된 노드의 uncle node가 RBTREE_BLACK인경우
    1))직선형 형태로 노드가 구성이 되어있을 시
    2))삼각형 형태로 노드가 구성이 되어있을 시
2. 현재 insert된 노드의 parent color가 RBTREE_BLACK인경우
  ->recoloring
*/
while(z->parent->color == RBTREE_RED){ // 만약 z의 parentNode의 색상이 red라면
  if(z->parent == z->parent->parent->left){ // 만약 z의 parent가 z의 할아버지노드의 왼쪽이라면
    node_t *uncle = z->parent->parent->right;
    if(uncle->color == RBTREE_RED){ // 1) uncle node가 RBTREE_RED인경우
      uncle->color = RBTREE_BLACK;
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      z = z->parent->parent;
    }
    else{ // 2) uncle node가 RBTREE_BLACK인경우
      if(z == z->parent->right){ // 삼각형일경우
        z = z->parent;
        left_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      right_rotate(t, z->parent->parent);
    }
  }
  else{
    node_t *uncle = z->parent->parent->left;
    if(uncle->color == RBTREE_RED){
      uncle->color = RBTREE_BLACK;
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      z = z->parent->parent;
    }else{
      if(z == z->parent->left){ // 삼각형일경우
        z = z->parent;
        right_rotate(t, z);
      }
      z->parent->color = RBTREE_BLACK;
      z->parent->parent->color = RBTREE_RED;
      left_rotate(t, z->parent->parent);
     }
   }
  }
  t->root->color = RBTREE_BLACK;
  return;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *tmp_parent = t->nil; // tmp_parent는 현재 노드의 부모노드를 나타내는데, 루트에서는 nil노드(현재노드의 부모))
  node_t *tmp_child = t->root; // tmp_child는 현재 노드
  while(tmp_child != t->nil){
    tmp_parent = tmp_child;
    if(key < tmp_child->key){
      tmp_child = tmp_child->left;}
    else{
      tmp_child = tmp_child->right;}
  }
  node_t *z = (node_t *)malloc(sizeof(node_t)); // 현재 node의 위치를 저장해줄 node
  z->parent = tmp_parent;
  z->key = key;
  if(tmp_parent == t->nil){ // root가 없을경우.
    t->root = z;
  }else if(key < tmp_parent->key){
    tmp_parent->left = z;
  }else{
    tmp_parent->right = z;
  }
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  rbtree_insert_fixAll(t, z);
  return z;
  
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *cur = t->root;

  while(cur != t->nil){

    if(key < cur->key){
      cur = cur->left;
    }else if(key > cur->key){
      cur = cur->right;
    }else{
      return cur;
    }
  }
    return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t *cur = t->root;
  while(cur->left != t->nil){
    cur = cur->left;
  }
  // TODO: implement find
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  node_t *cur = t->root;
  while(cur->right != t->nil){
    cur = cur->right;
  }
  // TODO: implement find
  return cur;
}

void rb_delete_fixAll(rbtree *t, node_t *x){
  node_t* w;
  while(x != t->root && x->color == RBTREE_BLACK) // x가 nil이 아닐때(x가 최하단 nil이 아닐때는 fix해줄게 없으므로.), x가 root가 아닐때, x의 컬러가 black이 아닐때 while문 반복.
  {
    if(x == x->parent->left) // x가 x의 parent의 왼쪽자식노드일 때
    {
      w = x->parent->right; // x의 parent의 오른쪽자식을 w로 가리킴.(형제노드임)
      if(w->color == RBTREE_RED){ // case1. 형제노드가 RED라면
        w->color = RBTREE_BLACK; // w의 color를 black으로 만들어주고
        x->parent->color = RBTREE_RED; // x와 w의 parent color를 red로 만들어 주고
        left_rotate(t, x->parent); // w를 x의 parent의 오른쪽자식으로 설정.
        w = x->parent->right;
      } // case1 해결의 경우 case 2, 3, 4로 넘어감.
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ // case2. x의 형제노드가 black이고, x의 형제노드의 오른쪽 자식이 red일때
        w->color = RBTREE_RED; // w의 color를 red로 만들고
        x = x-> parent; // x를 x의 parent로 만든다.(이 때 까지는 x의 parent는 red임.)
      }
      else{ // w의 형제의 color중 어느한쪽만 black인 경우
        if(w->right->color == RBTREE_BLACK){ // case3. x의 형제노드(w)가 black이고, x의 형제노드의 오른쪽자식의 color가 black일 경우
          w->left->color = RBTREE_BLACK; // w의 왼쪽자식노드의 색도 BLACK으로 만들어주고,
          w->color =RBTREE_RED; // w의 컬러는 red로 만들어준다(bh 보존)
          right_rotate(t, w);
          w = x->parent->right;
        }
        //case 4. 만약  x의 형제노드(w)가 black이고, x의 형제노드의 왼쪽자식의 color가 black일 경우
        w->color = x->parent->color; // w의 color가 x의 parent color를 받고
        x->parent->color = RBTREE_BLACK; // x의 parent가 black이 되고,
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root; // 4번상태에서 rotate를 하면 bh가 만족한다. 라고 암기하면됨. => x를 root로 보내면 끝.
      }
    }
    else{ // x가 x의 parent의 오른쪽 자식노드일때 (위의경우와 반대되는경우임(대칭) => 다 반대로만 해주면됨.)
        w = x->parent->left;

        if(w->color == RBTREE_RED){ // case1. 형제노드가 RED라면
        w->color = RBTREE_BLACK; // w의 color를 black으로 만들어주고
        x->parent->color = RBTREE_RED; // x와 w의 parent color를 red로 만들어 주고
        right_rotate(t, x->parent); // w를 x의 parent의 오른쪽자식으로 설정. //
        w = x->parent->left;
      }
      if(w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){ // case2. x의 형제노드가 black이고, x의 형제노드의 오른쪽 자식이 red일때
        w->color = RBTREE_RED; // w의 color를 red로 만들고
        x = x-> parent; // x를 x의 parent로 만든다.(이 때 까지는 x의 parent는 red임.)
      }
      else{ // w의 형제의 color중 어느한쪽만 black인 경우 ********TODO
        if(w->left->color == RBTREE_BLACK){ // case3. x의 형제노드(w)가 black이고, x의 형제노드의 오른쪽자식의 color가 black일 경우
          w->right->color = RBTREE_BLACK; // w의 왼쪽자식노드의 색도 BLACK으로 만들어주고,
          w->color =RBTREE_RED; // w의 컬러는 red로 만들어준다(bh 보존)
          left_rotate(t, w);
          w = x->parent->left;
        }
        //case 4. 만약  x의 형제노드(w)가 black이고, x의 형제노드의 왼쪽자식의 color가 black일 경우
        w->color = x->parent->color; // w의 color가 x의 parent color를 받고
        x->parent->color = RBTREE_BLACK; // x의 parent가 black이 되고,
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root; // 4번상태에서 rotate를 하면 bh가 만족한다. 라고 암기하면됨. => x를 root로 보내면 끝.
      }
    }
  }
  x->color = RBTREE_BLACK;
  return;
  }

void transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left) 
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  node_t *x; // fix_up의 기준이 될 노드
  node_t *y = p; // 삭제될 노드
  color_t origin_y_color = y->color;

  if(p->left == t->nil){ // 지울 노드의 왼쪽자식이 nil이라면(비어있다면)
    x = p->right; // x는 p의 오른쪽 자식으로 임시저장

    transplant(t, p, p->right);
    // p의 연결고리를 끊어놨으니 p를 free해주어야함.

  }else if(p->right == t->nil){ // 삭제할 노드의 오른쪽 자식이 nil이라면
    x = p->left;
    transplant(t, p, p->left);
  }else{ // p에게 두 자식이 있다면 *******
    node_t *tmp = p->right;
    node_t *tmp_parent;
    while(tmp != t->nil){ // 왼쪽 서브트리에서 가장 오른쪽(가장 큰 노드를 찾는과정)
      tmp_parent = tmp;
      tmp = tmp->left;
    }
    y = tmp_parent; // y에 왼쪽 서브트리에서 가장 큰노드가 붙었음.
    origin_y_color = y->color; // 원래 y의 color를 서브트리에서 가장 큰노드의 색으로 바꿔준다.
    x = y->right; // y의 자식노드 => 있으면 left노드, 없으면 nil노드가 나올것임. x에 넣어줌.
    if(y->parent == p){ // y의 parent가 p라면 (왼쪽서브트리에서 가장 큰 노드의 parent가 삭제하려는 노드라면 -> 즉, 삭제하려는 부모의 왼쪽자식이 가장 큰 노드라면.)
      x->parent = y; // x의 부모는 y이다.
    }else{ // 그렇지않고 y의 parent가 p가 아니라면(p와 y사이에 추가로 노드가 몇개 들어있다는 뜻.
      transplant(t, y, y->right);
      y->right = p->right; // 이제 p의 자리를 y가 대체하게되었음.
      y->right->parent = y; // y의 왼쪽자식노드의 부모를 y로 설정해줌.
    }
    transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }
  free(p);
  p = NULL;

  if(origin_y_color == RBTREE_BLACK){
      rb_delete_fixAll(t, x); // 최하단 서브트리에서 삭제되는 가장 큰노드(y)의 왼쪽자식(x)에서부터 fixup을 해가야함.
  }
  return 0;
}

void node_to_array(node_t *t, node_t *nil, key_t *arr, int *index, int n){
  if(t == nil){
    return;
  }
  if (*index == n) return;

  node_to_array(t->left, nil, arr, index, n);
  // *(arr + (*index)++) = t ->key;
  arr[(*index)++] = t->key;
  
  node_to_array(t->right, nil, arr , index, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  
  int *index = calloc(1, sizeof(int));
  node_to_array(t->root, t->nil, arr, index, n);
  free(index);

  return 0;
}
/*
  - RB tree의 내용을 *key 순서대로* 주어진 array로 변환
  - array의 크기는 n으로 주어지며 tree의 크기가 n 보다 큰 경우에는 순서대로 n개 까지만 변환
  - array의 메모리 공간은 이 함수를 부르는 쪽에서 준비하고 그 크기를 n으로 알려줍니다.
*/