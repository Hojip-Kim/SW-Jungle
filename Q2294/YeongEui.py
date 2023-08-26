import sys

input = sys.stdin.readline

# n: 동전 종류의 개수, k: 만들려는 금액
n, k = map(int, input().strip().split())

# 동전 종류별 금액
coins = sorted([int(input().strip()) for _ in range(n)])
coins = set(coins)

# 동전 개수 테이블
nums = [10001] * (k + 1)
nums[0] = 0


for coin in coins:
    for i in range(coin, k + 1):
        nums[i] = min(nums[i], nums[i-coin] + 1)

if nums[k] == 10001:
    print(-1)
else:
    print(nums[k])