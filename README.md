# nvidia-xrun-pm
These utility scripts aim to make the life easier for nvidia cards users.
It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization(in terms of linux drivers)

## Usage:
  1. switch to free tty
  1. login
  1. run `nvidia-xrun [app]`
  1. enjoy

Currently sudo is required as the script needs to wake up GPU, modprobe the nvidia driver and perform cleanup afterwards.

The systemd service can be used to completely remove the card from the kernel
device tree (so that it won't even show in `lspci` output), and this will
prevent the nvidia module to be loaded, so that we can take advantage of the
kernel PM features to keep the card switched off.

The service can be enabled with this command:

```
# systemctl enable nvidia-xrun-pm
```

When the nvidia-xrun command is used, the device is added again to the tree so that the nvidia module can be loaded properly: nvidia-xrun will remove the device and enable PM again after the application terminates.

## Structure
* **nvidia-xrun** - uses following dir structure:
* **/usr/bin/nvidia-xrun** - the executable script
* **/etc/X11/nvidia-xorg.conf** - the main X confing file
* **/etc/X11/xinit/nvidia-xinitrc** - xinitrc config file. Contains the setting of provider output source
* **/etc/X11/xinit/nvidia-xinitrc.d** - custom xinitrc scripts directory
* **/etc/X11/nvidia-xorg.conf.d** - custom X config directory
* **/etc/systemd/system/nvidia-xrun-pm.service** systemd service
* **/usr/share/xsessions/nvidia-xrun-openbox.desktop** - xsession file for openbox
* **/usr/share/xsessions/nvidia-xrun-plasma.desktop** - xsession file for plasma
* **[OPTIONAL] ~/.nvidia-xinitrc** - user-level custom xinit script file. You can put here your favourite window manager for example


## Setting the right bus id
Usually the 1:0:0 bus is correct. If this is not your case(you can find out through lspci or bbswitch output mesages) you can create
a conf script for example `nano /etc/X11/nvidia-xorg.conf.d/30-nvidia.conf` to set the proper bus id:

    Section "Device"
        Identifier "nvidia"
        Driver "nvidia"
        BusID "PCI:2:0:0"
    EndSection

You can use this command to get the bus id:

	lspci | grep -i nvidia | awk '{print $1}'

Also this way you can adjust some nvidia settings if you encounter issues:

    Section "Screen"
        Identifier "nvidia"
        Device "nvidia"
        #  Option "AllowEmptyInitialConfiguration" "Yes"
        #  Option "UseDisplayDevice" "none"
    EndSection

## Automatically run window manager
For convenience you can create `nano ~/.nvidia-xinitrc` and put there your favourite window manager:

    if [ $# -gt 0 ]; then
        $*
    else
        openbox-session
    #   startkde
    fi


With this you do not need to specify the app and you can simply run:

    nvidia-xrun

## Aur package
The aur package can be found here: https://aur.archlinux.org/packages/nvidia-xrun/

## COPR Repo for Fedora 27+
### To Install from COPR
* `dnf copr enable axeld/nvidia-xrun`
* `dnf install nvidia-xrun`

## OBS Repo for OpenSUSE, Fedora 27+ and RHEL7
The OBS Repo can be found [here](https://software.opensuse.org//download.html?project=home%3AAxelSilverdew&package=nvidia-xrun)


## Troubleshooting
### Steam issues
Yes unfortunately running Steam directly with nvidia-xrun does not work well - I recommend to use some window manager like openbox.

### HiDPI issue
When using openbox on a HiDPI (i.e. 4k) display, everything could be so small that is difficult to read.
To fix, you can change the DPI settings in `~/.Xresources (~/.Xdefaults)` file by adding/changing `Xft.dpi` setting. For example :

```
Xft.dpi: 192
```

### `nouveau` driver conflict
`nouveau` driver should be automatically blacklisted by `nvidia` but in case it is not, `nvidia` might not get access to GPU. Then you need to manually blacklist `nouveau` following Arch wiki https://wiki.archlinux.org/index.php/kernel_modules#Blacklisting.

### avoid `nvidia` driver to load on boot
`nvidia` driver may load itself on boot, then `nvidia-xrun` will fail to start Xorg session.
To avoid that, you should blacklist it (see link above).
Also sometimes, blacklisting is not enough and you should use some hack to really avoid it to load.
For example, adding `install nvidia /bin/false` to `/etc/modprobe.d/nvidia.conf` will make every load to fail.
In that case, you should add `--ignore-install` to `modprobe` calls in `nvidia-xrun` script.

### Vulkan does not work
Check https://wiki.archlinux.org/index.php/Vulkan
* remove package vulkan-intel
* set VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json
