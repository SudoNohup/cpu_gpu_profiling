#!/bin/sh

#export freq=2300000
#for i in $(seq 0 1 0)
#do
#  echo ${i}
#  cpufreq-set -c ${i} -g userspace
#  cpufreq-set -c ${i} -f ${freq}
#done

#for x in /sys/devices/system/cpu/cpu[0-3]/cpufreq/;do 
#for x in /sys/devices/system/cpu/cpu[0-71]/cpufreq;do 
for i in $(seq 0 1 7)
do
  echo /sys/devices/system/cpu/cpu${i}/cpufreq/scaling_max_freq
  #echo 2501000 #> $x/scaling_max_freq
  #echo 2300000 #> $x/scaling_max_freq
  echo 3400000 > /sys/devices/system/cpu/cpu${i}/cpufreq/scaling_max_freq
done

