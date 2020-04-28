import os
import sys


st = []
st_ = []


with open(r"C://Users//lxr//Desktop//hw_contest//result.txt") as test:
    line = test.readline()
    while(line):
        st.append(line)
        line = test.readline()


with open(r"C://Users//lxr//Desktop//result.txt") as test_:
    line = test_.readline()
    while (line):
        st_.append(line)
        line = test_.readline()

if len(st)==len(st_): print("行数相同")
else:
    print("行数不同")
    print(len(st))
    print(len(st_))
    sys.exit(1)

for i in range(len(st)):
    if st[i] != st_[i]:
        print("行数:"+str(i+1))
        print(st[i])
        print(st_[i])
print("Ok")