#오답

# import sys

# givenNum = int(sys.stdin.readline())

# givenList = list(map(int,sys.stdin.readline().split()))

# DPList = [[0 for _ in range(len(givenList)+1)] for _ in range(len(givenList)+1)]

# givenListX = givenList
# givenListY = givenList

# for i in range(1, len(givenListX)+1) :
#     for j in range(1, len(givenListY) + 1) :
#         if givenListX[i-1] > givenListY[j-1] and DPList[i-1][j] == DPList[i][j-1]:
#             DPList[i][j] = DPList[i][j-1] + 1
#         else :
#             DPList[i][j] = max(DPList[i-1][j], DPList[i][j-1])
            
# print(DPList[-1][-1])

#-------------------------------------------------------------------------------------

#정답

import sys

givenNum = int(sys.stdin.readline())
givenList = list(map(int, sys.stdin.readline().split()))

DPList = [1] * givenNum  # 모든 값은 적어도 자기 자신만 포함하는 길이 1의 부분수열을 가질 수 있습니다.

for i in range(1, givenNum):
    for j in range(i):
        if givenList[j] < givenList[i]:
            DPList[i] = max(DPList[i], DPList[j] + 1)

print(max(DPList))