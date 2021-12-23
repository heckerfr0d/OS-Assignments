# OS Assignment 1

## Instructions

1. Download the latest stable linux kernel from [kernel.org](https://www.kernel.org/).

   ```console
   $  wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.14.1.tar.xz
   ```

2. Extract the kernel source code from the tarball.

   ```console
   $  xz -cd linux-5.14.1.tar.xz | tar xvf -
   ```

3. In the linux source directory, get the configuration of the currently running kernel.

   ```console
   $  zcat /proc/config.gz > .config
   ```

4. Modify the .config file to accept the default values for all new config flags.

   ```console
   $  make olddefconfig
   ```

5. Compile the kernel using as many parallel jobs as you have CPU cores.

   ```console
   $  make -j$(nproc)
   ```

6. Switch over to root user and install the kernel modules.

   ```console
   $  su
   #  make modules_install
   ```

7. Copy the kernel image to /boot directory.

   ```console
   #  cp arch/x86/boot/bzImage /boot/vmlinuz-linux5141
   ```

8. Make initial RAM disk using a modified preset.

   ```console
   #  cp /etc/mkinitcpio.d/linux513.preset /etc/mkinitcpio.d/linux5141.preset
   #  nano /etc/mkinitcpio.d/linux5141.preset
   #  mkinitcpio -p linux5141
   ```

9. Copy System.map to /boot directory.

   ```console
   #  cp System.map /boot/System.map-linux5141
   ```

10. Configure GRUB to create an entry for the new kernel.

    ```console
    #  grub-mkconfig -o /boot/grub/grub.cfg
    ```

11. Reboot the system.

    ```console
    #  reboot
    ```
