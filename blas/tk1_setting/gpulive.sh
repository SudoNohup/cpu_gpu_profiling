echo 852000000 > /sys/kernel/debug/clock/override.gbus/rate
echo 1 > /sys/kernel/debug/clock/override.gbus/state
cat /sys/kernel/debug/clock/override.gbus/rate
cat /sys/kernel/debug/clock/override.gbus/state
cat /sys/kernel/debug/clock/gbus/rate

echo 924000000 > /sys/kernel/debug/clock/override.emc/rate
echo 1 > /sys/kernel/debug/clock/override.emc/state
cat /sys/kernel/debug/clock/override.emc/rate
cat /sys/kernel/debug/clock/override.emc/state
cat /sys/kernel/debug/clock/emc/rate
