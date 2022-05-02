make
echo "-------------------------"
insmod main.ko
echo "-------------------------"
ls -la /sys/kernel/debug/
echo "-------------------------"
ls -la /sys/kernel/debug/fortytwo
echo "-------------------------"
rmmod main.ko
echo "-------------------------"
ls /sys/kernel/debug/
echo "-------------------------"

make clean