#!/bin/bash
# /proc/sys/vm/drop_caches -- mean now set
# 0 -- open cache
# 1 --release cache(nobody use)
# 2 --release dentry, inode cache
# 3 = 1 + 2 

sync;sync;
echo 1 > /proc/sys/vm/drop_caches
sync;sync;
echo 2 > /proc/sys/vm/drop_caches
sync;sync;
swapoff -a;swapon -a
sync;sync;
