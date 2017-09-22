#!/bin/bash
echo "" >/home/chris/testmemory.txt;
free -m |tee -a /home/chris/testmemory.txt ;
vmstat -a 1 1 |tee -a /home/chris/testmemory.txt ;
cat /proc/meminfo |tee -a /home/chris/testmemory.txt ;
echo "" >> /home/chris/testmemory.txt;
echo "Start runing read : " >> /home/chris/testmemory.txt;
echo "" >> /home/chris/testmemory.txt; 
time /home/chris/GitHub/Data_Process.github.io/read/r1S |tee -a /home/chris/testmemory.txt &
vmstat -a 2 30|tee -a /home/chris/testmemory.txt ;
echo "" >> /home/chris/testmemory.txt;
echo "Start runing read : " >> /home/chris/testmemory.txt;
echo "" >> /home/chris/testmemory.txt; 
free -m |tee -a /home/chris/testmemory.txt ;
cat /proc/meminfo |tee -a /home/chris/testmemory.txt ;



