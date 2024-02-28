import sys

T = int(sys.stdin.readline())

for i in range(T) :
    N = int(sys.stdin.readline())
    people = []
    for _ in range(N):
        people.append(list(map(int, input().split())))

    people.sort()
    cutLine = people[0][1]  # 서류 1등의 면접 등수

    cnt = 1  # 합격자 cnt (서류 1등은 바로 합격)
    for i in people:
        # 서류만 앞사람보다 낮고, 면접 등수는 높은 사람 합격
        if cutLine > i[1]:
            cnt += 1
            cutLine = i[1]
    print(cnt)