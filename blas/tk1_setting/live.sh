#!/bin/bash
echo 0 > /sys/devices/system/cpu/cpuquiet/tegra_cpuquiet/enable
echo 1 > /sys/bus/cpu/devices/cpu0/online
echo 1 > /sys/bus/cpu/devices/cpu1/online
echo 1 > /sys/bus/cpu/devices/cpu2/online
echo 1 > /sys/bus/cpu/devices/cpu3/online
echo performance > /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor
