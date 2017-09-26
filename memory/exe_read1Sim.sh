#!/bin/bash
outputFileName="/home/chris/testMMAPBig.txt"
exeFileName="/home/chris/GitHub/Data_Process.github.io/read/rM"

echo "" > ${outputFileName};
free -m |tee -a ${outputFileName};
vmstat -a 1 1 |tee -a ${outputFileName};
cat /proc/meminfo |tee -a /${outputFileName};
echo "" >> ${outputFileName};
echo "Start runing read : " >> ${outputFileName};
echo "" >> ${outputFileName}; 

time ${exeFileName} |tee -a ${outputFileName} &

vmstat -a 10 15|tee -a ${outputFileName} ;
echo "" >> /${outputFileName};
echo "Start runing read : " >> ${outputFileName};
echo "" >> ${outputFileName}; 
free -m |tee -a ${outputFileName};
cat /proc/meminfo |tee -a ${outputFileName};



