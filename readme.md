
# 1. Linux IO Request Counter

This is a system for returning the number of pending and issued I/O
requests from a particular disk device. It requires that you load a
kernel module and link your application with a user library that
will interact with this module. It assumes the devices you are using
Linux's MQ Scheduler. In recent Linux systems, this will probably be
the case.

# 2. Dependencies

Linux 3.6+  
cmake 3.10 or higher  
C11 compiler  
build-essential  

# 3. Building

> cd /path/to/linux-io-rq-counter  
> mkdir build  
> cd build  
> cmake ../  
> make -j4  
> make build-km  

# 4. Usage

## 4-1. Install the kerenel module

> make insert-km  

You can use the following to see if the module is successfully installed:  
> make check-km  

If it was not listed, you can use the following to view the kernel log:  
> make check-klog  

You can use the following to clean the kernel log to make it more readable:
> make clean-klog  

## 4-2. Associate storage devices with the kernel module

> ./mount-counter /dev/...  

NOTE: You should use the device file and not partitions. For
example, use /dev/sda instead of /dev/sda1.  

## 4-3. Link with the user library

Building this application will create a file called:

> liblinux-io-rq-counter-um.so

You can link with this file to interact with the kernel module from
your application. The library API is as follows:

> void init_counter_syscalls(void);  
> int get_num_io_rqs(char *dev);  

You must call that init function to establish a link between your user
application and the kernel module. Assuming you did step 4-2 successfully,
the kernel module will be associated with a bunch of devices. You will
call get_num_io_rqs() with one of the devices you mounted a counter to.
For example, "/dev/sda".

This is helpful to see if the kernel module goes wrong.

## 4-4. Remove the kernel module

> make remove-km  

# 5. Test

You can run the following code to test the counter:  
> ./test-counter /dev/...  

The first result will likely be a very large number. The counters store
the number of I/O requests since the start of the computer. After the
first iteration, the counter will be more accurate.

