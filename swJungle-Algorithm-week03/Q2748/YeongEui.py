import sys

n = int(sys.stdin.readline().strip())

result = [0] * (n + 1)

def fibonacci(n):
    if n == 1 or n == 2:
        result[n] = 1
        return 1

    if result[n]:
        return result[n]

    result[n] = fibonacci(n - 1) + fibonacci(n - 2)

    return result[n]

print(fibonacci(n))