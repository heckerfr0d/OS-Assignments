# OS Assignment 2

A simple character device driver as a kernel module for the linux kernel.  

## Instructions

1. Build the kernel module and application program.

    ```console
    $   make
    ```

2. Load the kernel module into the kernel.

    ```console
    #   insmod chardev.ko
    ```

3. Verify that the module is loaded and device is created.

    ```console
    $   lsmod | grep chardev
    $   cat /proc/devices | grep chardev
    $   ls -l /dev | grep chardev
    ```

4. Run the application program.

   ```console
   #    ./test
   ```

5. Follow the system log for messages from the kernel module.

    a. Arch Linux:

     ```console
     #   journalctl -f
     ```

    b. Ubuntu:

     ```console
     #   dmesg
     ```