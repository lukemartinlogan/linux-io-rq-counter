
# 1. LabStor

LabStor is an object-oriented storage system that does not depend on filesystems
such as EXT4 and BTRFS to store data. In this prototype implementation, we do 
this by writing directly to the device file using O_DIRECT. 

# 2. Dependencies

Linux 3.6+  
cmake 3.10 or higher  
C11 compiler  

# 3. Building

cd /path/to/linux-io-rq-counter  
mkdir build  
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX="/path/to/install"  
make -j4  
make install

# 4. Usage

## 4-1. System environment

### 4-1-1. Disable swap space

This library is designed to handle all storage operations in a storage node.
To guarantee this, disable your swap space by doing:

> swapoff -a

and then commenting out any swap entries in your /etc/fstab. This will make it so 
that no unexpected I/O operations occur.

### 4-1-2. Create a HugePage cache

In order to optimize memory management, we use a cache of HugePages to obtain a 
higher I/O throughput. This cache should be allocated at boot time 
to prevent memory fragmentation. You should make this cache as large as possible. 
Leave just enough space for the OS and storage application to run, and leave the rest 
for I/O.

#### 4-1-2-1. Find the HugePage size

You can find the set of support HugePage sizes by running the following:
> ls /sys/kernel/mm/hugepages

See which sizes are supported. Hopefully something between 2MB to 16MB is 
supported. Note: this will show page sizes in KB, not MB.

#### 4-1-2-2. Edit the kernel boot parameters

For the GRUB bootloader, open the grub config file as follows:

> sudo gedit /etc/default/grub

Find the line that says "GRUB_CMDLINE_LINUX_DEFAULT", and add
the following arguments to the list (without quotations):
* "hugepagesz=S", where S is size of HugePage you want to allocate
    * You can use the following suffixes to indicate scale: [kKmMgG]
    * For example, 2M would represent 2 megabytes
* "hugepages=N", where N is the number of HugePages
 
After that, update the bootloader and reboot:
> sudo update-grub  
> sudo reboot

You can run the command below in order to check if the changes were
made:

> cat /proc/meminfo | grep HugePages

SOURCES:
* https://wiki.ubuntu.com/Kernel/KernelBootParameters,
* https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt

## 4-2. User interface

Our library exposes the following interface:

> todo







# 5. Tests

## 5-1. Compare different I/O approaches

In this test, we will compare the performance difference between different object
stores. They will each use a different approach for interacting with the underlying
hardware. Currently we compare the following approaches:
1. Use underlying filesystems (EXT4/BTRFS/etc..) to store objects  
2. Keep track of objects internally and write directly to the device file  

In order to run this test, execute:
> make test-object-stores









# 6. Preliminary Tests

## 6-1. Get a function graph of POSIX syscalls

This test allows you to see what happens in the kernel when you call read()/write().
This test depends on trace-cmd.

### 6-1-1. Determine the syscall names

Before we can trace what happens with these system calls, we have to 
figure out their names. In order to do this, we will run the following
command:  

> make trace-all-syscalls  

This will create a file in your build directory called trace-all-syscalls.txt.
It is a human-readable text file that contains the system calls used when
executing the program test/transfer/trace-posix.cpp. Look at it and figure
out which system calls correspond to POSIX open, read, write, and close syscalls.

### 6-1-2. Set environment variables
  
Before running this test, you must set the following environment variables:
* SYS_READ
* SYS_WRITE
* TEST_FILE

In my particular case, based on the information from step 1, I set the environment
variables to track pread and pwrite:  

> export SYS_READ=__x64_sys_pread64   
> export SYS_WRITE=__x64_sys_pwrite64  

I also could set these variables to track read() and write():

> export SYS_READ=__x64_sys_read   
> export SYS_WRITE=__x64_sys_write  

The actual names of these system calls may vary depending on your OS. 
SyS_read is also common for the read() syscall for example. You'll
have to look into your trace-all-syscalls.txt file to find out for sure.

You must also set which file you want to perform the reads/writes to.
You can do so as follows:

> export TEST_FILE="/path/to/whatever..."  

### 6-1-3. Get the function graph

Now we can trace the read()/write() or pread()/pwrite() system calls.

> make trace-posix-syscalls  

This will create a file in your build directory called trace-posix-syscalls.txt.

## 6-2. Test different I/O methods

We want to bypass as many overheads introduced by the Linux kernel as possible.
In this experiment, we test multiple approaches for putting data on a disk and
reading data from a disk. We perform the following tests:
* Performance of fwrite
* Performance of fwrite/fflush
* Performance of fwrite with 2MB HugePages
* Performance of fwrite/fflush with 2MB HugePages
* Performance of write with O_DIRECT
* Performance of write with O_DIRECT and 2MB HugePages

### 6-2-1. Set environment variables

First, we must set which device/file we want to perform I/O with and then
set which directory we want to store the output logs from each test.

> export TEST_FILE="/path/to/whatever..."  
> export LOG_DIR="/path/to/log/directory/..."  

NOTE: your log directory MUST end in a trailing slash.

### 6-2-2. Run tests
> make time-fwrite-100M  
> make time-fwrite-flush-100M  
> make time-fwrite-huge-100M  
> make time-fwrite-flush-huge-100M  
> make time-direct-write-100M  
> make time-direct-write-huge-100M  

NOTE: these tests will drop the OSes page cache before each iteration, which
requires root priveliges.

### 6-2-3. Get statistics

This will print out the mean, range, and standard deviation for each test
case.

> make time-transfer-stats 

This will also store a CSV and JSON file in the log directory containing the 
statistics.
