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
    ""
};
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8
/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)
#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define WSIZE   4
#define DSIZE   8
#define CHUNKSIZE   (1<<13)
#define MAX(x, y)   ((x) > (y)? (x) : (y))
#define PACK(size, alloc)   ((size) | (alloc))
#define GET(p)      (*(unsigned int *)(p)) // 주소 읽기
#define PUT(p, val) (*(unsigned int *)(p) = (val)) // 주소 쓰기
#define GET_SIZE(p)     (GET(p) & ~0x7) //
#define GET_ALLOC(p)    (GET(p) & 0x1) //
#define HDRP(bp)    ((char *)(bp) - WSIZE) // 헤더주소
#define FTRP(bp)    ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) // 푸터주소
/* [[헤드]bp->[페이로드][푸터]] 워드만큼 빼면 지금블럭 헤드가 나오고 더블워드만큼 빼면 이전블록 푸터가 나옴*/
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) // 다음블록 주소
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE))) // 이전블록 주소
static void *coalesce(void *bp);
static void *extend_heap(size_t words);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);
static char* heap_listp;
static char* next_p;
/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1){
        return -1;
    }
    PUT(heap_listp, 0); // 얼라이먼트 패딩
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1)); // 프롤로그헤더
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1)); // 프롤로그푸터
    PUT(heap_listp + (3*WSIZE), PACK(0, 1)); // 에필로그 헤더
    heap_listp += (2*WSIZE);
    next_p = heap_listp;
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL){
        return -1;
    }
    return 0;
}
static void *extend_heap(size_t words){
    char *bp;
    size_t size;
    // printf("연장\n");
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE; //홀짝
    if((long)(bp = mem_sbrk(size)) == -1){
        return NULL;
    }
    //
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
    return coalesce(bp);
}
/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
// void *mm_malloc(size_t size)
// {
    // int newsize = ALIGN(size + SIZE_T_SIZE);
    // void *p = mem_sbrk(newsize);
    // if (p == (void *)-1)
    // return NULL;
    // else {
    //     *(size_t *)p = size;
    //     return (void *)((char *)p + SIZE_T_SIZE);
    // }
// }
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;
    if (size == 0){
        return NULL;
    }
    if (size <= DSIZE){
        asize = 2*DSIZE;
    }else{
        asize = ALIGN(size + DSIZE);
    }
    if((bp = find_fit(asize)) !=NULL){
        place(bp, asize);
        return bp;
    }
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    // printf("요청크기: %d\n",size);
    // printf("말록긑 크기: %d \n",GET_SIZE(HDRP(bp)));
    return bp;
}
/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    size_t size = GET_SIZE(HDRP(ptr));
    // printf("프꼴 and 크기:%d\n",size);
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));
    coalesce(ptr);
}
static void *coalesce(void *bp){
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    if(prev_alloc && next_alloc){ // 왼쪽 오른쪽 둘다 할당된 공간일 때
        return bp;
    }
    else if (prev_alloc && !next_alloc){ // 왼쪽은 할당된공간, 오른쪽은 가용공간일 때
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }
    else if (!prev_alloc && next_alloc){ // 왼쪽은 가용공간, 오른쪽은 할당공간일 때
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    else{ // 왼쪽 오른쪽 둘다 가용공간일 때
        // printf("케3\n");
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    //  printf("꼴끝\n");
    next_p = bp;
    return bp;
}
//first fit
// static void *find_fit(size_t asize){
//     void *bp;
//     for (bp = heap_listp; GET_SIZE(HDRP(bp)) >0; bp = NEXT_BLKP(bp)){
//         if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
//             return bp;
//         }
//     }
//     return NULL;
// }

static void *find_fit(size_t asize){
    void *bp;
    // printf("시작주소: %p\n",next_p);
    for (bp = next_p; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
            return bp;
        }
    }
    // for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
    //     if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
    //         return bp;
    //     }
    // }
        for (bp = heap_listp; bp != next_p; bp = NEXT_BLKP(bp)){
           if (!GET_ALLOC(HDRP(bp)) && (asize<=GET_SIZE(HDRP(bp)))){ // 이 블록이 가용하고(not 할당) 내가 갖고있는 asize를 담을 수 있으면
                return bp; // 내가 넣을 수 있는 블록만 찾는거니까, 그게 나오면 바로 리턴.
            }
    }
    
    return NULL;
}
//
static void place(void *bp, size_t asize){ // asize : 내가 지금 찾아야할 사이즈 (남아있는 가용공간을 통해))
    size_t csize = GET_SIZE(HDRP(bp));
    if((csize - asize) >= (2*DSIZE)){ // 
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(csize - asize, 0));
        PUT(FTRP(bp), PACK(csize - asize, 0));
        next_p = bp;
    }else{
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        next_p = bp;
    }
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
// void *mm_realloc(void *bp, size_t size)
// {
//     void *old_bp = bp;
//     void *new_bp;
//     size_t copySize;
//     new_bp = mm_malloc(size);
//     if (new_bp == NULL){
//         return NULL;
//     }
//     copySize = GET_SIZE(HDRP(old_bp));
//     if (size < copySize){
//         copySize = size;
//     }
//     memcpy(new_bp, old_bp, copySize);
//     mm_free(old_bp);
//     return new_bp;
// }

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