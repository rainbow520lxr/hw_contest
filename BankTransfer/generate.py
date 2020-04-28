import os
import random
txt = r"C:\Users\lxr\Desktop\hw_contest\BankTransfer\test_data.txt"

# with open(txt, 'w') as f:
#     for i in range(11):
#         for k in range(11):
#             if i!= k :
#                 f.write(str(i+1)+","+str(k+1)+",1\n")


n = 10

N = 55000
f = open(r"C:\Users\lxr\Desktop\Bank\test_data.txt", 'w')
grest = [random.randint(0, n) for item in range(N)]

crest = [n-grest[i] for i in range(N)]

count = 0

# d
for i in range(N):
    while grest[i]:
        go = random.randint(0, N-1)
        if go!=i and crest[go]>0:
            f.write(str(i+1)+","+str(go+1)+",1\n")
            count += 1
            if count > 5000:print('当前转账数: '+ str(count))
            grest[i] -= 1
            crest[go] -= 1


f.close()
print("Ok")