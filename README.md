# nvidia-xrun
These utility scripts aim to make the life easier for nvidia cards users.
It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization(in terms of linux drivers)

## Usage: 
  1. switch to free tty
  1. login
  1. run `nvidia-xrun [app]`
  1. enjoy

Currently sudo is required as the script needs to wake up GPU, modprobe the nvidia driver and perform cleanup afterwards. For this we use bbswitch.

## Structure
* **nvidia-xrun** - uses following dir structure:
* **/usr/bin/nvidia-xrun** - the executable script
* **/etc/X11/nvidia-xorg.conf** - the main X confing file
* **/etc/X11/xinit/nvidia-xinitrc** - xinitrc config file. Contains the setting of provider output source
* **/etc/X11/xinit/nvidia-xinitrc.d** - custom xinitrc scripts directory
* **/etc/X11/nvidia-xorg.conf.d** - custom X config directory
* **[OPTIONAL] ~/.nvidia-xinitrc** - user-level custom xinit script file. You can put here your favourite window manager for example
 

## Setting the right bus id
Usually the 1:0:0 bus is correct. If this is not your case(you can find out through lspci or bbswitch output mesages) you can create
a conf script for example `nano /etc/X11/nvidia-xorg.conf.d/30-nvidia.conf` to set the proper bus id:

    Section "Device"
        Identifier "nvidia"
        Driver "nvidia"
        BusID "PCI:2:0:0"
    EndSection
    
Also this way you can adjust some nvidia settings if you encounter issues:

    Section "Screen"
        Identifier "nvidia"
        Device "nvidia"
        #  Option "AllowEmptyInitialConfiguration" "Ylinees"
        #  Option "UseDisplayDevice" "none"
    EndSectionline
    
## Automatically run window manager
For convenience you can create `nano ~/.nvidia-xinitrc` and put there your favourite window manager:

    openbox-session
    
With this you do not need to specify the app and you can simply run:

    nvidia-xrun
    
## Aur package
The aur package can be found here: https://aur.archlinux.org/packages/nvidia-xrun/
line

## Troubleshooting
### Steam issues
Yes unfortunately running Steam directly with nvidia-xrun does not work well - I recommend to use some window manager like openbox.

### HiDPI issue
When using openbox on a HiDPI (i.e. 4k) display, everything could be so small that is difficult to read.
To fix, you can change the DPI settings in `~./Xresources` file by adding/changing `Xft.dpi` setting. For example :

```
Xft.dpi: 192
```

### `nouveau` driver conflict
`nouveau` driver should be automatically blacklisted by `nvidia` but in case it is not, `nvidia` might not get access to GPU. Then you need to manually blacklist `nouveau` following Arch wiki https://wiki.archlinux.org/index.php/kernel_modules#Blacklisting.
# 
