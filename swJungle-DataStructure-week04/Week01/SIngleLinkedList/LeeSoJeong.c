#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node
{
  int data;
  struct _node *link;
} node;

// create & print
node *create_list();
void print_list(node *);

// add (append, insert)
void append_node(int, node *);
void insert_node(int, int, node *);

// get
node *get_node(int, node *);

// set
void set_node(int, int, node *);

// delete (pop, delete)
node *pop_node(node *);
node *delete_node(int, node *);

// size
int size_list(node *);

// create list and return head address
node *create_list()
{
  node *new;
  new = (node *)malloc(1 * sizeof(node));

  if (new == NULL)
  {
    printf("memory allocation error!\n");
    exit(1);
  }
  new->link = NULL;

  return new;
}

// print all list elements
void print_list(node *list)
{
  if (list == NULL)
  {
    return;
  }

  node *curr;
  curr = list->link; // except head
  if (curr == NULL)
  {
    printf("empty list\n");
    return;
  }

  while (curr != NULL)
  {
    printf("%d ", curr->data);
    curr = curr->link;
  }
  printf("\n");
  return;
}

// append data at last
void append_node(int data, node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return;
  }

  node *new;
  new = (node *)malloc(1 * sizeof(node));
  if (new == NULL)
  {
    printf("memory allocation error!\n");
    exit(1);
  }

  new->data = data;
  new->link = NULL;

  node *curr = list;
  while (curr != NULL)
  {
    if (curr->link == NULL)
    {
      // means curr is last elem
      curr->link = new;
      break;
    }
    curr = curr->link;
  }
  return;
}

// insert data at certain idx
void insert_node(int data, int idx, node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return;
  }

  node *new;
  new = (node *)malloc(1 * sizeof(node));
  if (new == NULL)
  {
    printf("memory allocation error!\n");
    exit(1);
  }
  new->data = data;
  new->link = NULL;

  node *curr;
  curr = list;
  for (int i = 0; i < idx; i++)
  {
    curr = curr->link;
    if (curr == NULL)
    {
      // means there is not such idx
      printf("index out of bound!\n");
      return; // could be exit(1)
    }
  }

  node *tmp;
  tmp = curr->link;
  curr->link = new;
  new->link = tmp;

  return;
}

// get node at certain idx
node *get_node(int idx, node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return NULL;
  }

  node *curr = list;
  for (int i = 0; i < idx + 1; i++)
  {
    curr = curr->link;
    if (curr == NULL)
    {
      printf("index out of bound!\n");
      return NULL; // could be exit(1)
    }
  }
  return curr;
}

// set node at certain idx another value
void set_node(int idx, int data, node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return;
  }

  node *target = get_node(idx, list);
  target->data = data;
  return;
}

// pop data at last and return address
node *pop_node(node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return NULL;
  }

  node *pop;
  node *curr = list;

  while (curr)
  {
    if (curr->link->link == NULL)
    {
      pop = curr->link;
      curr->link = NULL;
      break;
    }
    curr = curr->link;
  }

  return pop;
}

// delete data at certain idx and return address
node *delete_node(int idx, node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return NULL;
  }

  node *curr = list;
  if (curr->link == NULL)
  {
    // empty list
    printf("empty list already\n");
    return NULL;
  }

  for (int i = 0; i < idx; i++)
  {
    curr = curr->link;
    if (curr == NULL)
    {
      printf("index out of bound!\n");
      return NULL; // could be exit(1)
    }
  }

  node *delete = curr->link;
  curr->link = delete->link;

  return delete;
}

// get list length
int size_list(node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return -1;
  }

  node *curr = list->link;
  int cnt = 0;
  while (curr)
  {
    curr = curr->link;
    cnt++;
  }
  return cnt;
}

int main()
{
  // create list
  node *list;

  // input sequence
  int n;
  while (1)
  {
    char s[10];
    int n;
    int idx;

    printf("command: ");
    scanf("%s", s);

    if (!strcmp(s, "end"))
    {
      break;
    }
    else if (!strcmp(s, "create"))
    {
      list = create_list();
    }
    else if (!strcmp(s, "append"))
    {
      scanf("%d", &n);
      append_node(n, list);
    }
    else if (!strcmp(s, "insert"))
    {
      scanf("%d %d", &n, &idx);
      insert_node(n, idx, list);
    }
    else if (!strcmp(s, "pop"))
    {
      node *pop = pop_node(list);
      if (pop != NULL)
      {
        printf("%d (%p)\n", pop->data, pop);
        free(pop);
      }
    }
    else if (!strcmp(s, "delete"))
    {
      scanf("%d", &idx);
      node *delete = delete_node(idx, list);
      if (delete != NULL)
      {
        printf("%d (%p)\n", delete->data, delete);
        free(delete);
      }
    }
    else if (!strcmp(s, "get"))
    {
      scanf("%d", &idx);
      node *get = get_node(idx, list);
      if (get != NULL)
      {
        printf("%d (%p)\n", get->data, get);
      }
    }
    else if (!strcmp(s, "set"))
    {
      scanf("%d %d", &idx, &n);
      set_node(idx, n, list);
    }
    else if (!strcmp(s, "size"))
    {
      n = size_list(list);
      printf("%d\n", n);
    }
    else
    {
      printf("**USE COMMANDS!**\n");
      printf("create\n");
      printf("append <int>\n");
      printf("insert <int> <idx>\n");
      printf("pop\n");
      printf("delete <idx>\n");
      printf("get    <idx>\n");
      printf("set    <idx> <int>\n");
      printf("size\n");
      printf("end (free all and exit)\n");
    }
    print_list(list);
  }

  // free all memory
  node *curr = list;
  printf("free ");
  while (curr)
  {
    node *tmp = curr->link;
    printf("%d ", curr->data);
    free(curr);
    curr = tmp;
  }
  printf("\n");
  system("leaks a.out > leaks_result_temp; cat leaks_result_temp | grep leaked && rm -rf leaks_result_temp");

  return 0;
}