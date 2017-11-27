# nyan-load

code is based on https://github.com/puckipedia/efi-example.

have you ever wanted a moving nyan cat as your bootloader? of course!

now you can.

the rainbow and cat tail moves as of right now, and work is being done to add stars in the background.

![image](https://cloud.githubusercontent.com/assets/6683648/21967660/3929e4be-db59-11e6-93ff-28dc47e85759.png)

to exit the bootloader, enter the konami code.

## installing

`make` will generate the file `nyan-load.efi`.
Make that your default bootloader, and the wonders of nyan cat will appear the next time you boot up.

Alternatively, you can also just try it in a virtual machine (qemu) by running `make go` to open up qemu.

If you do go the virtual machine route, you will need the proper firmware - Open Virtual Machine Firmware (OVMF).

Check out http://www.tianocore.org/ovmf/ for more info and (https://sourceforge.net/projects/edk2/files/OVMF/ or https://github.com/BlankOn/ovmf-blobs) to download.

## TODO

 - add the stars
 - sound???
