make
echo "-------------------------"
insmod main.ko
echo "-------------------------"
ls -la /sys/kernel/debug/
echo "-------------------------"
ls -la /sys/kernel/debug/fortytwo

sudo cat /sys/kernel/debug/fortytwo/jiffies
echo "-------------------------"
rmmod main.ko
echo "-------------------------"
ls -la /sys/kernel/debug/
echo "-------------------------"

make clean