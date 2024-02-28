#include <stdio.h>
#include <stdlib.h> // malloc, free 함수를 사용하기 위해

typedef struct node { // 구조체
    int val; // 값을 받기 위한 변수
    struct node *next; //다음 노드를 가리키기 위한 next 포인터
} node_t; // 새로운 타입인 node_t 생성

void append(node_t **ptr_head, int val, int *count);
void unshift(node_t **ptr_head, int val, int *count);
int shift(node_t **ptr_head);
int remove_by_index(node_t **ptr_head, int index);
void print_list(node_t *head);
int pop(node_t **ptr_head);
void delete_list(node_t **ptr_head);

int main() {
    node_t *head = NULL; // 연결리스트가 비어있음
    int length = 0;
    append(&head,1, &length);
    append(&head,2, &length); // 연결리스트의 끝에 각각 순서대로 1,2,3 값을 가지는 노드 추가
    append(&head,3, &length);
    printf("%d\n", length); // 3
    unshift(&head,4, &length);
    unshift(&head,5, &length); // 연결리스트 시작 부분에 각각 순서대로 4,5 값을 가지는 노드 추가 (맨 앞 노드 생성)
    printf("%d\n", length); // 5

    printf("나온 숫자: %d\n",shift(&head)); // 첫 번째 노드 제거 후 해당 노드 출력
    printf("나온 숫자: %d\n", remove_by_index(&head,0)); //  해당 인덱스 삭제 및 출력
    printf("나온 숫자: %d\n", remove_by_index(&head,1));
    print_list(head);
    printf("나온 숫자: %d\n",pop(&head)); // 마지막 노드 삭제 후 출력
    print_list(head);
    delete_list(&head); // 연결리스트의 모든 노드를 메모리에서 해제하여 리스트 삭제
}

// print_list
void print_list(node_t *head){
    
    node_t *current = head;

    while (current != NULL){
        printf("{%d}->",current->val);
        current = current -> next;
    }
    printf("NULL");
    printf("\n");
}

// append
void append(node_t **ptr_head, int val, int *count){
    
    if (*ptr_head == NULL){
        *ptr_head = (node_t *)malloc(sizeof(node_t));
        (*ptr_head)->val = val;
        (*ptr_head)->next = NULL;
    }

    node_t *current = *ptr_head;
    while (current->next != NULL)
        current = current->next;

    current->next = malloc(sizeof(node_t));
    current->next->val=val;
    current->next->next = NULL;

    *count += 1;

}

//unshift
void unshift(node_t **ptr_head, int val, int *count){
    node_t *new_node;
    new_node = (node_t*)malloc(sizeof(node_t));
    new_node->val = val;
    new_node->next = *ptr_head;
    *ptr_head = new_node;

    *count += 1;
}

//shift
int shift(node_t **ptr_head){
    node_t *current = *ptr_head;
    if(current==NULL)
        return -1;
    *ptr_head = current->next;
    int retval = current->val;
    free(current);
    return retval;
}

// remove_by_index
int remove_by_index(node_t **ptr_head, int index){
    
    if (*ptr_head == NULL)
        return -1;
    
    if (index == 0)
        return shift(ptr_head);

    node_t *current = *ptr_head;
    node_t *before = *ptr_head;

    current = current->next;

    if (current == NULL)
        return -1;

    for (int i=1; i<index; i++){
        current = current->next;
        before = before->next;
        if (current==NULL)
            return -1;
    }
    before->next = current->next;
    int retval = current->val;
    free(current);
    return retval;
}

// pop
int pop(node_t **ptr_head){
    if (*ptr_head == NULL)
        return -1;

    node_t *current = *ptr_head;
    if (current->next == NULL){
        *ptr_head = NULL;
        int retval = current ->val;
        free(current);
        return retval;
    }

    node_t *before = *ptr_head;
    current = current->next;
    while (current->next != NULL){
        before = before->next;
        current = current->next;
    }
    before->next = NULL;
    int retval = current->val;
    free(current);
    return retval;
}

// delete
void delete_list(node_t **ptr_head){
    node_t *current = *ptr_head;
    *ptr_head = NULL;

    free(current);
}