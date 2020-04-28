import os
import random
txt = r"C:\Users\lxr\Desktop\Bank\test_data.txt"

with open(txt, 'w') as f:
    for i in range(11):
        for k in range(11):
            if i!= k :
                f.write(str(i+1)+","+str(k+1)+",1\n")
