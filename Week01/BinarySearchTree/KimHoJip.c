#include <stdio.h>
#include <stdlib.h>

typedef struct Node{

    int key;
    struct Node *left; // leftChild
    struct Node *right; // rightChild

}Node;

Node* Insert(Node *root, int key, int *length);
void delete(Node *root, int key, int *length);
void printTree(Node *root);
void search(Node *root, int key);

int main(){
    int length = 0;

    Node *root = NULL; // root의 Node생성(left, right 아무값도 갖고있지않음.)

    
    root = Insert(root, 5, &length);
    root = Insert(root, 3, &length);
    root = Insert(root, 7, &length);
    root = Insert(root, 1, &length);
    root = Insert(root, 9, &length);
    root = Insert(root, 10, &length);
    root = Insert(root, 2, &length);


    printf("%d\n", length); // 7이 나와야함.
    delete(root, 3, &length); // 
    printf("%d\n", length); // 6이 나와야함.
    printTree(root);
    printf("\n");
    search(root, 2);

}

Node* Insert(Node *root, int key, int *length){
    Node *depth; // 탐색을 진행할 포인터
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->left = newNode->right = NULL;
    
    if(root == NULL){ // root가 비어있다면
        root = newNode;
        *length += 1;
        return root;
    }
     // root가 비어있지 않다면
    depth = root;

    while(depth != NULL){
        if(key == depth->key){// 중복된 값이 있다면
            printf("이미 Tree안에 중복된 key가 있습니다.");
            return root;
        }else if(key < depth->key){ // key가 현재 depth의 노드보다 작다면
            if(depth->left == NULL){ // + 현재 depth의 노드의 왼쪽이 비어있다면
                depth->left = newNode;
                *length += 1;
                return root;
            }else{ // 그렇지않고 depth의 노드 왼쪽이 비어있지않다면
                depth = depth->left; // depth의 left로 깊게 파고든다.
            }
        }else if(key > depth->key){ // key가 현재 depth의 노드보다 크다면
            if(depth->right == NULL){ // +현재 depth의 노드의 오른쪽이 비어있다면
                depth->right = newNode;
                *length += 1;
                return root;
            }else{ // 비어있지 않다면
                depth = depth->right; // depth의 right으로 깊게 파고든다
            } 
        }
    }

}

void delete(Node *root, int key, int *length){
    Node *depth = NULL; // 삭제할 노드
    Node *parent = NULL; // 삭제할 노드의 부모노드
    Node *child = NULL;

    depth = root;
    while(depth != NULL){
        if(key == depth->key){
            break;
        }
        parent = depth;
        if(key < depth->key){ // depth의 key보다 작다면
            depth = depth->left;
        }else{
            depth = depth->right;
        }
    }//parent와 depth의 위치를 알았음.
    if(depth == NULL){
        printf("이 트리에는 {%d}원소가 없습니다.\n", key);
        return;
    }else if(depth->left == NULL && depth->right == NULL){ // 삭제할 노드가 leaf노드라면
        if(parent != NULL){ // 부모노드가 있다면
            if(parent->left == depth){ // 부모노드의 왼쪽이 현재 지울노드라면
                parent->left = NULL;
            }else{ // 부모노드의 오른쪽이 현재 지울노드라면
                parent->right = NULL;
            }
        }else{ // 부모노드가 없는노드면 root노드인것임.
            root = NULL;
        }
    }
    else if(depth->left != NULL && depth->right != NULL){ // 지울 노드의 왼쪽과 오른쪽 둘다 자식노드가 있다면
        Node *tmpDepth = depth; // depth의 위치 저장
        Node *tmpDepthLeft = depth->left;

        while(tmpDepthLeft->right != NULL){
            tmpDepth = tmpDepthLeft; // 가장 큰 노드의 부모노드(끊어주기 위함)
            tmpDepthLeft = tmpDepthLeft->right; // 가장 큰 노드
        } // tmpDepthLeft가 왼쪽에서 가장 큰 key를 가진 Node를 갖게됨.

        

        tmpDepth->right = tmpDepthLeft->left;        
        depth->key = tmpDepthLeft->key;
        free(tmpDepthLeft);
        *length -= 1;
        return;

    }else{ // 지울 노드의 자식노드가 한개라면
        if(depth->left == NULL){ // 오른쪽노드만 있다면
            child = depth->right;
        }else{
            child = depth->left;
        }

        if(parent != NULL){ // 만약 부모가 있다면
            if(parent->left == depth){ // parent의 leftnode가 지울노드라면
                parent->left = child;
            }else{
                parent->right = child;
            }
        }else{ // 만약 부모가 없다면
            root = child;
        }
    }
    free(depth);
    *length -= 1;
    return;
}

void printTree(Node *root){
    if(root == NULL){
        return;
    }else{
        printTree(root->left);
        printf("%d -> ", root->key);
        printTree(root->right);
    }
}

void search(Node *root, int key){
    if(root == NULL){
        printf("트리가 비어있습니다.\n");
        return;
    }
    while(root != NULL && root->key != key){
        if(key < root->key){ // key가 root보다 작다면
            root = root->left;
        }else{
            root = root->right;
        }
    }
    if(root == NULL){
        printf("해당 key값(%d)을 찾을 수 없습니다.\n", key);
        return;
    }else{
        printf("해당 key값(%d)이 트리내에 존재합니다.\n", key);
        return;
    }
}