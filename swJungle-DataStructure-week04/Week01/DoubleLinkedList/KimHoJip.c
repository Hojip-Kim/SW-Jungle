#include <stdio.h>
#include <stdlib.h>

typedef struct Node{
    struct Node *next;
    struct Node *prev;
    int data;
}Node;

Node* createNode(int data){
    Node *newNode = malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

void Add(Node *head, Node *tail, int data, int *count){

    Node *newNode = createNode(data);

    Node *prev = tail->prev;

    prev->next = newNode;
    newNode->prev = prev;

    newNode->next = tail;
    tail->prev = newNode;

    *count += 1;
}


int Get(Node *head, Node *tail, int idx, int *count){
    if( *count > idx && idx >= 0){ // length내에 idx가 들어가있다면 (정상적인 index를 호출한다면)
        if((float)(*count-1) / 2 <= idx){ // index가 길이 중앙에서 왼쪽에 위치하여 head와 거리가 가깝다면
            Node *cur = head->next;
            
            while(idx > 0){
                cur = cur->next;
                idx -= 1;
            }
            return cur->data;
        }else{
            Node *cur = tail->prev;
            idx = *count-idx;
            while(idx > 0){
                cur = cur->prev;
                idx -= 1;
            }
            return cur->data;
        }
    }else{ // 그렇지않고 정상적인 index를 호출하지 않는다면
        printf("Can't find the index {index : %d}. Please check your index.\n", idx);
        return -1;
    }
}

void Set(Node *head, Node *tail, int idx, int data, int *count){
    if( *count > idx && idx >= 0){ // length내에 idx가 들어가있다면 (정상적인 index를 호출한다면)
        if((float)(*count-1) / 2 <= idx){ // index가 길이 중앙에서 왼쪽에 위치하여 head와 거리가 가깝다면
            Node *cur = head->next;
            
            while(idx > 0){
                cur = cur->next;
                idx -= 1;
            }
            cur->data = data;
        }else{
            Node *cur = tail->prev;

            while(idx > 0){
                cur = cur->prev;
                idx -= 1;
            }
            cur->data = data;
        }
    }else{ // 그렇지않고 정상적인 index를 호출하지 않는다면
        printf("Can't find the index {index : %d}. Please check your index.\n", idx);
    }
}

void Delete(Node *head, Node *tail, int idx, int *count){
    if( *count  > idx && idx >= 0){ // 문제가 없다면(length내에 idx가 들어있다면)
        if((float)((*count -1) / 2) <= idx) { // index가 length기준 나누기 2 해서 왼쪽(head)와 가까이 있거나 가운데라면
            Node *cur = head->next; // head의 next부터 시작.(head부터 시작하게되면 head노드부터 탐색을하게되므로 안됨. (head를 null이라고 생각하고있어야함))

            while(idx > 0){
                cur = cur->next;
                idx--;
            }
            Node *prev = cur->prev; // cur의 앞노드를 tmp화
            Node *next = cur->next;
            prev->next = next;
            next->prev = prev;

            free(cur);
            *count -= 1;
        }else{ // tail과 가까이있다면
            Node *cur = tail->prev;
            while(idx > 0){
                cur = cur->prev;
                idx--;
            }

            Node *prev = cur->prev; // cur의 앞노드를 tmp화
            Node *next = cur->next;
            prev->next = next;
            next->prev = prev;

            free(cur);
            *count -= 1;
        }
    }else{ // idx가 정상범주에서 벗어나있다면
        printf("Can't find the index {index : %d}. Please check your index.\n", idx);
    }
    

}

int main(){

    int length = 0;

    Node *head = createNode(0); // head의 Node 생성(prev, next = NULL(아무도 참조하고있지않음))
    Node *tail = createNode(0); // tail또한 마찬가지.
    head->next = tail; // head와 tail 서로 참조
    tail->prev = head;
    printf("%d\n", length); // 현재 head(LinkedList)에 Node가 들어가있는지.

    Add(head, tail, 1, &length);

    printf("%d\n", length);

    Add(head, tail, 2, &length);

    printf("%d\n", length);

    Add(head, tail, 3, &length);
    Add(head, tail, 4, &length);
    Add(head, tail, 5, &length);
    printf("%d\n", length);

    Delete(head, tail, 1, &length); // 정상작동
    printf("%d\n", length);

    printf("%d\n", Get(head, tail, 3, &length)); // 3이 출력되어야 함

    Set(head, tail, 1, 6, &length);
    printf("%d\n", Get(head, tail, 2, &length));

    return 0;
}
