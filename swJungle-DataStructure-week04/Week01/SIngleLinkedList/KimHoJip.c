#include <stdio.h> // 표준 입출력 library (Standard Input/Output Library)
#include <stdlib.h> // 동적 메모리관리를 위한 C언어 표준 라이브러리.


typedef struct Node{
    struct Node *next;
    int data;
}Node;

int sizeOf(Node *node){ // 처음 list의 노드가 가리키는것은 head임.
    int cnt = 0;
    Node *cur = node->next;

    while(cur != NULL){
        cur = cur->next;
        cnt++;
    }

    return cnt;

}

void Add(Node *node, int data){ // 여기서 들어오는 node는 처음 head를 가리킴.
    
    if (node->next == NULL){ // 마지막 node의 next가 NULL을 가리킨다면(마지막요소 이라면)
        Node *newNode = malloc(sizeof(Node)); // newNode라는곳에 들어올 node의 값 할당
        newNode->data = data;
        newNode->next = NULL;

        node->next = newNode;
    }
    else{
        Node *cur = node->next;
        while(cur->next != NULL){
            cur = cur->next;
        }
        Node *newNode = malloc(sizeof(Node));
        newNode->data = data;
        newNode->next = NULL;

        cur->next = newNode; // cur의 next의 주소에 newNode의 주소를 넣음.
    }
}

void Insert(Node *node, int idx, int data){ // 사이즈 2에 index 1, 3을 넣는다고하면
    if(sizeOf(node)-1 >= idx && idx >= 0){
        Node *cur = node; //헤드상태

        while(idx > 0){
            cur = cur->next; // 인덱스0부터 시작
            idx--;
        }
        Node *newNode = malloc(sizeof(Node));
        Node *tmpNode;
        tmpNode = cur->next; // insert할 위치의 index

        newNode->data = data;
        cur->next = newNode;
        newNode->next = tmpNode;
        
        
    }else if(sizeOf(node)-1 < idx){
        Add(node, data);
    }else{
        printf("Can't insert index. please check the index.\n");
    }
}

int Get(Node *node, int idx){ // 예로 idx = 2이라면
    if(sizeOf(node)-1 >= idx && idx >= 0){
        Node *cur = node->next; // 다음것을 가리키고있음 cur이

        while(idx > 0){
            cur = cur->next;
            idx--;
        }
        return cur->data;    

    }else{
        printf("This List not have index: {%d}, Please reWrite!\n", idx);
        return -1;
    }
}

void Set(Node *node, int idx, int data){
    if(sizeOf(node)-1 >= idx && idx >= 0){
        Node *cur = node->next; 

        while(idx > 0){
            cur = cur->next;
            idx--;
        }
        cur->data = data;
    }else{
        printf("Can't update index. please check the index.\n");
    }
}

void Delete(Node *node, int idx){
    if(sizeOf(node)-1 >= idx && idx >= 0){
        Node *cur = node; //헤드상태

        while(idx > 0){
            cur = cur->next; // 인덱스0부터 시작
            idx--;
        }
        if(cur->next != NULL){
            Node *tmpNode;
            tmpNode = cur->next; // 현재 삭제할 노드임.
            cur->next = tmpNode->next;
            free(tmpNode);
        }
    }else{
        printf("Can't delete index. please check the index.\n");
    }
}

int main() {
    Node *list1 = malloc(sizeof(Node));
    list1->next = NULL;

    int size = sizeOf(list1);

    printf("%d\n", size);

    Add(list1, 1);
    Add(list1, 2);
    Add(list1, 3);
    for(int i = 0; i<sizeOf(list1); i++){
        printf("%d ", Get(list1, i));
    }
    printf("\n");
    printf("size : %d\n", sizeOf(list1));

    Insert(list1, 1, 4);
    for(int i = 0; i<sizeOf(list1); i++){
        printf("%d ", Get(list1, i));
    }
    printf("\n");
    printf("size : %d\n", sizeOf(list1));

    return 0;
}
