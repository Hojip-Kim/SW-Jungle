#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _node
{
  int data;
  struct _node *prev;
  struct _node *next;
} node;

void print_list(node *);
node *create_list();
void insert_node(int, int, node *);
node *delete_node(int, node *);
node *get_node(int, node *);

void print_list(node *list)
{
  if (list == NULL)
  {
    printf("create list first\n");
    return;
  }

  if (list->next == NULL && list->prev == NULL)
  {
    printf("empty list\n");
    return;
  }

  // left to right
  node *curr = list->next;
  while (1)
  {
    printf("%d ", curr->data);
    if (curr->next == NULL)
      break;
    curr = curr->next;
  }
  printf("\n");

  // right to left
  node *last = curr;
  while (curr)
  {
    printf("%d ", curr->data);
    if (curr->prev == list)
      break;
    curr = curr->prev;
  }
  printf("\n");
  return;
}

node *create_list()
{
  // why does static alloc not work?
  // 로컬변수는 함수 종료 후 해제된다
  node *head = malloc(sizeof(node));
  if (head == NULL)
  {
    printf("memory allocation error\n");
    exit(1);
  }
  head->next = NULL;
  head->prev = NULL;
  return head;
}

void insert_node(int data, int idx, node *list)
{
  if (list == NULL)
  {
    return;
  }
  node *new = malloc(sizeof(node));
  if (new == NULL)
  {
    printf("memory allocation error\n");
    exit(1);
  }
  new->data = data;
  new->next = NULL;
  new->prev = NULL;

  node *curr = list;
  for (int i = 0; i < idx; i++)
  {
    curr = curr->next;
    if (curr == NULL)
    {
      printf("index out of bound!\n");
      return;
    }
  }

  if (curr->next)
  {
    // 마지막 노드일 경우를 제외하고
    // head만 만들고 tail을 안 만들어서 따로 예외 처리해줌
    (curr->next)->prev = new;
  }
  new->next = curr->next;
  new->prev = curr;
  curr->next = new;

  return;
}

node *delete_node(int idx, node *list)
{
  if (list == NULL)
  {
    return NULL;
  }

  node *curr = list;
  for (int i = 0; i < idx + 1; i++)
  {
    curr = curr->next;
    if (curr == NULL)
    {
      printf("index out of bound!\n");
      return NULL;
    }
  }

  if (curr->next)
  {
    // 마지막 노드일 경우를 제외하고
    // head만 만들고 tail을 안 만들어서 따로 예외 처리해줌
    (curr->next)->prev = curr->prev;
  }
  (curr->prev)->next = curr->next;
  return curr;
}

node *get_node(int idx, node *list)
{
  if (list == NULL)
  {
    return NULL;
  }
  node *curr = list;
  for (int i = 0; i < idx + 1; i++)
  {
    curr = curr->next;
    if (curr == NULL)
    {
      printf("index out of bound!\n");
      return NULL;
    }
}
return curr;
}

int main()
{
  node *list = NULL; // head node

while (1)
{
    char s[10];
    int n, idx;
    printf("command: ");
    scanf("%s", s);
    if (!strcmp(s, "end"))
    break;
    else if (!strcmp(s, "create"))
    {
    list = create_list();
    }
    else if (!strcmp(s, "insert"))
    {
    scanf(" %d %d", &n, &idx);
    insert_node(n, idx, list);
    }
    else if (!strcmp(s, "delete"))
    {
    scanf(" %d", &idx);
      node *delete = delete_node(idx, list);
    if (delete)
    {
        printf("%d (%p)\n", delete->data, delete);
        free(delete);
      }
    }
    else if (!strcmp(s, "get"))
    {
      scanf(" %d", &idx);
      node *target = get_node(idx, list);
      if (target)
      {
        printf("%d (%p)\n", target->data, target);
      }
    }
    else
    {
      printf("**USE COMMAND**\n");
      printf("create\n");
      printf("insert <val> <idx>\n");
      printf("delete <idx>\n");
      printf("get    <idx>\n");
      printf("end to exit program\n");
      continue;
    }
    print_list(list);
  }

  // free all
  node *curr = list;
printf("free ");
while (curr)
{
    node *tmp = curr;
    curr = curr->next;
    printf("%d ", tmp->data);
    free(tmp);
  }
  printf("\n");
  system("leaks a.out > leaks_result_temp; cat leaks_result_temp | grep leaked && rm -rf leaks_result_temp");

  return 0;
}