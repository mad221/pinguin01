make
sudo insmod test.ko
echo "-------------------------"
sudo cat /dev/fortytwo
echo -e "\n-------------------------"

sudo rmmod test.ko
make clean

