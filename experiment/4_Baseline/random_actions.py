import random

actions = []
actions.append(random.randint(0, 4))
print(actions)

i = 0
while i < 19:
    x = random.randint(0, 4)
    if actions[i] != x:
        actions.append(x)
    else:
        i = i - 1
    i = i + 1

print(actions)
