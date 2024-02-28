#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"
/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "malloc",
    /* First member's email address */
    "ㄹㅇㅋㅋ",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 6) // best 9
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) ((size) | (alloc))                 // 값 할당
#define GET(p) (*(unsigned int *)(p))                        // 주소 읽기.
#define PUT(p, val) (*(unsigned int *)(p) = (val))           // 주소에 값 할당.
#define GET_SIZE(p) (GET(p) & ~0x7)                          // 현재 헤더의 size
#define GET_ALLOC(p) (GET(p) & 0x1)                          // 현재 헤더의 allocation 여부
#define HDRP(bp) ((char *)(bp)-WSIZE)                        // 현재 bp의 헤더주소
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // 현재 bp의 푸터주소
/* [[헤드]bp->[페이로드][푸터]] 워드만큼 빼면 지금블럭 헤드가 나오고 더블워드만큼 빼면 이전블록 푸터가 나옴*/
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE((char *)(bp)-WSIZE)) // 다음블록 (payload)주소
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-DSIZE))   // 이전블록 (payload)주소

#define PRE_EXPLICIT(bp) (*(void **)(bp)) // 현재 bp의 prev
#define NEX_EXPLICIT(bp) (*(void **)(bp + WSIZE)) // 현재 bp의 next

#define BUDDY_SIZE (20)                                                 // 버디사이즈 20 (루트 갯수)
#define GET_ROOT(class) (*(void **)((char *)(heap_listp) + (WSIZE * class))) // GET_ROOT(1)을 하면 1번인덱스의 루트를 얻어냄.
/*
heap_listp + 2WSIZE = 1 BYTE
heap_listp + 3WSIZE = 2 BYTE
heap_listp + 4WSIZE = 4 BYTE
heap_listp + 5WSIZE = 8 BYTE
heap_listp + 6WSIZE = 16 BYTE
heap_listp + 7WSIZE = 32 BYTE
heap_listp + 8WSIZE = 64 BYTE
heap_listp + 9WSIZE = 128 BYTE
heap_listp + 10WSIZE = 256 BYTE
heap_listp + 11WSIZE = 512 BYTE
heap_listp + 12WSIZE = 1,024 BYTE
heap_listp + 13WSIZE = 2,048 BYTE
heap_listp + 14WSIZE = 4,096 BYTE
heap_listp + 15WSIZE = 8,192 BYTE
heap_listp + 16WSIZE = 16,384 BYTE 
heap_listp + 17WSIZE = 32,768 BYTE 
heap_listp + 18WSIZE = 65,534 BYTE 
heap_listp + 19WSIZE = 131,068 BYTE 
heap_listp + 20WSIZE = 262,136 BYTE // 상한선

*/
static void *coalesce(void *bp);
static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static void insertSegregateList(void *bp);
static void deleteExplicit(void *bp);
size_t findPosition(size_t value);
size_t my_pow(size_t val);

static char *heap_listp;

/*
 * mm_init - initialize the malloc package.
 */

// collese할 떄 자신의 왼쪽이나 오른쪽에 같은 크기가 있는지만확인해서 합치면 됨.
// 재귀 or 반복문으로 해당 가용크기의 free를 분할해주는 함수 짜야함
// free를 분할해줌과 동시에 여러 리스트로 할당이 들어가도록 해야함. insert를 잘 사용하면될듯.
// extend_heap할 때 chunk보다 큰 사이즈가 들어가면 chunk보다 한단계 큰 2의 배수를 extend해줌.

int mm_init(void) // buddy_size = 16
{
    if ((heap_listp = mem_sbrk((BUDDY_SIZE + 4) * WSIZE)) == (void *)-1)
    {
        return -1;
    }
    PUT(heap_listp, 0);                                                    // 얼라이먼트 패딩
    PUT(heap_listp + (1 * WSIZE), PACK((BUDDY_SIZE + 2) * WSIZE, 1)); // 프롤로그헤더
    for (int i = 0; i < BUDDY_SIZE; i++) // heap_listp + 2WSIZE에서부터 시작(heap_listp + 2WSIZE에는 가용공간 1이 들어가도록.(사실상 없음 => 네번째 루트까지는 안쓰임.))
    {
        PUT(heap_listp + (2 + i) * WSIZE, NULL);
    }
    PUT(heap_listp + (BUDDY_SIZE + 2) * WSIZE, PACK((BUDDY_SIZE + 2) * WSIZE, 1)); // 프롤로그푸터
    PUT(heap_listp + (BUDDY_SIZE + 3) * WSIZE, PACK(0, 1));                             // 에필로그 헤더

    heap_listp += 2 * WSIZE; // firstExplicit으로 payload를 가리킴.

    if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    {
        return -1;
    }

    return 0;
}

size_t my_pow(size_t val){ // 12가 들어오면
    size_t tmp = 1;
    for(int i = 0; i < val; i++){
        tmp = tmp * 2;
    }
    return tmp;
}

static void *extend_heap(size_t words) // 완
{
    char *bp;
    size_t size;
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE; // 홀짝

    size_t tmp = findPosition(size); 
    if(size % my_pow(tmp) == 0){ // 예를들어 size가 4096이라면 => tmp = 12
        if ((long)(bp = mem_sbrk(my_pow(tmp))) == -1)
        {
            return NULL;
        }
        
        PUT(HDRP(bp), PACK(my_pow(tmp), 0));
        PUT(FTRP(bp), PACK(my_pow(tmp), 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
        return coalesce(bp);
    }else{
        if ((long)(bp = mem_sbrk(my_pow(tmp+1))) == -1)
        {
            return NULL;
        }
        
        PUT(HDRP(bp), PACK(my_pow(tmp+1), 0));
        PUT(FTRP(bp), PACK(my_pow(tmp+1), 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
        return coalesce(bp);
    }

   
}//extendHeap 완료 => 2의배수로 나누어떨어지면 해당 사이즈를 할당, 그렇지않으면 해당 사이즈보다 작은 배수보다 2를 곱해서 할당.
//ex) 82000을 할당해달라고 하면 131,072를 할당해줌.

void *mm_malloc(size_t size)
{   
   
    size_t asize;
    size_t extendsize;
    char *bp;
    if (size == 0)
    {
        return NULL;
    }
    asize = ALIGN(size + DSIZE);

    if ((bp = find_fit(asize)) != NULL)
    {
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}
/*
 * mm_free - Freeing a block does nothing.
 */


//TODO * free는 나중에 건들기.
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));

    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    coalesce(ptr);
}

size_t findPosition(size_t value) // value가 16
{
    if (value == 0)
        return -1; // 값이 0인 경우 예외 처리

    int tmp = 31 - __builtin_clz(value);
    if (tmp > BUDDY_SIZE-3) // 262,144가 상한선임.
    {
        return BUDDY_SIZE-3;
    }
    else
    {
        return tmp;
    }
}

static void insertSegregateList(void *bp)
{   

    int class = findPosition(GET_SIZE(HDRP(bp)));

    NEX_EXPLICIT(bp) = GET_ROOT(class);
    if (GET_ROOT(class) != NULL)
    {
        PRE_EXPLICIT(GET_ROOT(class)) = bp;
    }
    GET_ROOT(class) = bp;
}

// static void insertExplicit(void* bp){ // 첫 시작은 firstExplicit으로 고정.

//     NEX_EXPLICIT(bp) = NEX_EXPLICIT(heap_listp);
//     PRE_EXPLICIT(bp) = heap_listp;
//     PRE_EXPLICIT(NEX_EXPLICIT(heap_listp)) = bp;
//     NEX_EXPLICIT(heap_listp) = bp;

// }

static void deleteExplicit(void *bp)
{   
    int class = findPosition(GET_SIZE(HDRP(bp)));

    if (bp == GET_ROOT(class))
    {
        GET_ROOT(class) = NEX_EXPLICIT(GET_ROOT(class));
        return;
    }

    NEX_EXPLICIT(PRE_EXPLICIT(bp)) = NEX_EXPLICIT(bp);
    if (NEX_EXPLICIT(bp) != NULL)
    {
        PRE_EXPLICIT(NEX_EXPLICIT(bp)) = PRE_EXPLICIT(bp);
    }
}

static void *coalesce(void *bp)
{

    insertSegregateList(bp); // 우선 현재 bp를 freelist에 넣음.
    size_t csize = GET_SIZE(HDRP(bp)); // 현재 블록의 사이즈
    void *root = heap_listp + (BUDDY_SIZE + 1) * WSIZE; // 실제 메모리블록이 시작하는 위치 (프롤로그블록의 바로 뒤부터 시작.)
    void *left_buddy; // 왼쪽버디의 bp
    void *right_buddy; // 오른쪽버디의 bp

    while(1){
        if((bp-root) & csize){ // 현재 블록에서 root까지의 size와 csize가 중복되는 비트가 있다면(즉, size만큼 move했다면 현재 block은 right buddy임)
            left_buddy = bp - csize; // left_buddy는 bp-csize임.(즉, left_buddy는 PREV_BLKP(bp)와 같음.)
            right_buddy = bp; // right_buddy는 bp와 같음.
        }else{ // 만약 중복되는 비트가 없다면 move를 하지않았다는것이므로, left buddy임.
            right_buddy = bp+csize; // right_buddy = NEXT_BLKP(bp)와 같음.
            left_buddy = bp; // left_buddy는 현재 bp가 가리키는 블록포인트임.
        }

        if(!GET_ALLOC(HDRP(left_buddy)) && !GET_ALLOC(HDRP(right_buddy)) && GET_SIZE(HDRP(left_buddy)) == GET_SIZE(HDRP(right_buddy))){
            //left buddy의 alloc이 0(가용 가능공간), right buddy의 alloc이 0(가용 가능공간), left buddy와 right buddy의 size가 같다면 **TODO -> size가 같은지는 안봐도될듯

            deleteExplicit(left_buddy); // left_buddy의 클래스에서 해당 버디 제거
            deleteExplicit(right_buddy); // right_buddy의 클래스에서 해당 버디 제거
            csize <<= 1; // 2를 곱한다.(csize에)
            PUT(HDRP(left_buddy), PACK(csize, 0)); // left_buddy의 헤더포인트에 가용공간 설정을 해준다.
            PUT(FTRP(right_buddy), PACK(csize, 0));
            insertSegregateList(left_buddy);
            bp = left_buddy;
        }
        else{
            break;
        }
    }

    return bp;
}


static void *find_fit(size_t asize)
{
    int class = findPosition(asize);
    void *bp;

    while (class < BUDDY_SIZE) // 현재 탐색하는 클래스가 범위 안에 있는 동안 반복
    {
        bp = GET_ROOT(class);
        while (bp != NULL)
        {
            if ((asize <= GET_SIZE(HDRP(bp)))) // 적합한 사이즈의 블록을 찾으면 반환
                return bp;
            bp = NEX_EXPLICIT(bp); // 다음 가용 블록으로 이동
        }
        class += 1;
    }
    return NULL;
}
//
static void place(void *bp, size_t asize)
{ // 현재 가용할 수 있는 블록의 주소 bp
    size_t csize = findPosition(GET_SIZE(HDRP(bp))); // 17
    printf("%d\n", GET_SIZE(HDRP(bp)));
    deleteExplicit(bp); // 사용하여야하므로 free list에서 지움.
    size_t targetClass = findPosition(asize); // target class임. // 11

    while(csize > targetClass){ // 현재 블록이 target class가 되기 전까지 크기를 줄여가며 오른쪽놈들을 free list에 집어넣음.
        csize -= 1;
        PUT(HDRP(bp), PACK(my_pow(csize), 0));
        PUT(FTRP(bp), PACK(my_pow(csize), 0));
        // 주의 : bp의 위치를 바꾸면 안됨.
        PUT(HDRP(NEXT_BLKP(bp)), PACK(my_pow(csize), 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(my_pow(csize), 0));
        insertSegregateList(NEXT_BLKP(bp));
    }
    PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
    PUT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
    
}

// 1. 오른쪽 연장

void *mm_realloc(void *bp, size_t size)
{
    void *old_bp = bp; // 현재 bp를 old_bp에 저장.
    void *new_bp; // new_bp 생성
    size_t copySize; 
    new_bp = mm_malloc(size); // 현재 만들고싶은만큼의 사이즈를 갖는놈을 malloc해서 생성해냄 (new_bp)
    if (new_bp == NULL){ // malloc 생성 실패하면 NULl Return
        return NULL;
    }
    copySize = GET_SIZE(HDRP(old_bp)); // 현재 bp의 사이즈를 copySize에 저장
    if (size < copySize){ // 새로 들어올 size가 copySize보다 작다면
        copySize = size; // copySize = size (작은놈으로 바뀜))
    }
    memcpy(new_bp, old_bp, copySize); // 
    mm_free(old_bp);
    return new_bp;
}