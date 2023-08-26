import sys

givenString1 = sys.stdin.readline().strip()
givenString2 = sys.stdin.readline().strip()

len1 = len(givenString1)
len2 = len(givenString2)

dp = [[0] * (len2+1) for _ in range(len1+1)]

resultList = []

