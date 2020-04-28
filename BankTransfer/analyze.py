import os
import math
t_map = {}
m_map = {}
m_map_ = {}

with open(r"C://Users//lxr//Desktop//hw_contest//result.txt") as test:
    ft = test.readline()
    ft = test.readline()
    while ft:
        send = [int(x) for x in ft.rstrip("\n").split(",")]
        send = sorted(send)
        send = [str(x) for x in send]
        send = ','.join(send)
        if send in t_map.keys():
            t_map[send] += 1
        else:
            t_map[send] = 1
        ft = test.readline()


with open(r"C://Users//lxr//Desktop//a.txt") as my:
    fm = my.readline()
    fm = my.readline()
    while fm:
        send__ = [int(x) for x in fm.rstrip("\n").split(",")]
        send_ = sorted(send__)
        send_ = [str(x) for x in send_]
        send_ = ','.join(send_)
        if send_ in m_map.keys():
            m_map[send_] += 1
            m_map_[send_].append(fm.rstrip("\n"))
        else:
            m_map[send_] = 1
            m_map_[send_] = [fm.rstrip("\n")]
        fm = my.readline()

print("ok")
