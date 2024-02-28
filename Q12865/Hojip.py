import sys

N, K = map(int,sys.stdin.readline().split())

dp= [0] * (K+1) # dp[index] => 무게가 index일떄의 최대 가치


for i in range(N) :
    W, V = map(int,sys.stdin.readline().split())
    for j in range(K, W-1, -1) :
        dp[j] = max(dp[j], dp[j-W] + V)
print(dp[K])
