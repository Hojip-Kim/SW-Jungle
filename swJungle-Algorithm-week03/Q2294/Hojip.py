import sys
import math
n, k = map(int,sys.stdin.readline().split())

DPtable = [math.inf] * (k+1)
DPtable[0] = 0
coinList = []

for i in range(n) :
    coinList.append(int(sys.stdin.readline().strip()))

for coin in coinList :
    for i in range(coin, k+1) :
        DPtable[i] = min(DPtable[i], DPtable[i-coin] + 1)

if DPtable[k] == math.inf :
    print(-1)
else :
    print(DPtable[k])