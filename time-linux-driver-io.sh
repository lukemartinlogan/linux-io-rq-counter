#!/bin/bash

#ENVIRONMENT VARS:
#TEST_FILE: the file you will use to perform I/O tests
#LOG_DIR: the directory where you will store output logs

CMD=$1
TEST_CASE=$2
SIZE_IN_MB=$3
NUM_ITER=$4

sudo sync
sudo sysctl -w vm.drop_caches=3
sudo insmod ${CMD}

#echo "7" > /proc/sys/kernel/printk
#make build-linux-driver-io
#insmod '/media/lukemartinlogan/Mirror2/Documents/Projects/Labios/LabStor/test/time-linux-driver-io/time-linux-driver-io.ko'
#dmesg | grep "BIO IS COMPLETE!"
#dmesg | grep "labstor"
#rmmod time_linux_driver_io
#hexdump -C -n 10 /dev/sdc
#dmesg --clear