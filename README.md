# nvidia-xrun
These utility scripts aim to make the life easier for nvidia cards users.
It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization(in terms of linux drivers)

## Usage:
  1. switch to free tty
  2. login
  3. run `nvidia-xrun [return tty] [app]`
  4. enjoy

## Usage from existing X session:
  1. open a terminal emulator (as Xterm)
  2. run `nvidia-xrun-util start_from_X` you can specify an app with `--exec="[app]"` or the tty number to switch before the nvidia X session finished with `--actualVt=[return tty]` (by default, it switch back to the actual tty, before the script was run)
  3. enjoy
  
  **This version needs no sudo right for the current user**

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
* **/usr/bin/nvidia-xrun** - the executable bash script
* **/usr/bin/nvidia-xrun-util** - the executable binary contains all admin commands
* **/etc/X11/nvidia-xorg.conf** - the main X confing file
* **/etc/X11/xinit/nvidia-xinitrc** - xinitrc config file. Contains the setting of provider output source
* **/etc/X11/xinit/nvidia-xinitrc.d** - custom xinitrc scripts directory
* **/etc/X11/nvidia-xorg.conf.d** - custom X config directory
* **/etc/systemd/system/nvidia-xrun-pm.service** systemd service
* **/etc/default/nvidia-xrun** - nvidia-xrun config file
* **[OPTIONAL] /usr/share/xsession/nvidia-gnome.desktop** - gnome-session entry using nvidia-xrun in the gdm login manager
* **[OPTIONAL] $XDG_CONFIG_HOME/X11/nvidia-xinitrc** - user-level custom xinit script file. You can put here your favourite window manager for example

## Modifications in this repository
(I'm sorry for my bad english, I'm a french student)
	This repository is a fork of the tangxinfa repository (branch "fix-no-sudo") who permit to use nvidia-xrun without sudo rights by separating all sudo commands in the binary "nidia-xrun-util" (run with setuid root).
	I've modified the binary to start "nvidia-xrun" in a new user session in a new tty using **systemd-run** (https://unix.stackexchange.com/questions/554592/how-to-manually-run-init-start-a-xorg-server-on-a-different-vt-tty/554603#554603).
	When the `nvidia-xrun-util start_from_X` command start, it wait one second before switch to the tty8 (to prevent swithing back to the tty1 at first session ending in gdm). After, it will run the nvidia-xrun command in the tty8 as user. When the session finished, it switch back to the previous tty or the tty specified by the user (`--actualVt=[tty number]`).
	For the time, you cannot modify the tty opened by the `nvidia-xrun-util start_from_X` because it is hard coded. That means if you run the command twice, it will wait before the first nvidia X ending before starting a new one. (the classic  `nvidia-xrun` command is no affected because it run in the current tty)
	**To make possible switching back to the previous tty, the nvidia-xrun command has been changed! You must specify the tty number before the app to execute like this `nvidia-xrun 1 xterm`**

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

Note that this prints your bus id in hexadecimal, but the Xorg configuration
script requires that you provide it in decimal, so you'll need to covert it.
You can do this with bash:

    # In this example, my bus id is "3c"
    bash -c "echo $(( 16#3c ))"

Also this way you can adjust some nvidia settings if you encounter issues:

    Section "Screen"
        Identifier "nvidia"
        Device "nvidia"
        #  Option "AllowEmptyInitialConfiguration" "Yes"
        #  Option "UseDisplayDevice" "none"
    EndSection

In order to make power management features work properly, you need to make sure
that bus ids in `/etc/default/nvidia-xrun` are correctly set for both the
NVIDIA graphic card and the PCI express controller that hosts it. You should be
able to find both the ids in the output of `lshw`: the PCIe controller is
usually displayed right before the graphic card.

## Automatically run window manager
For convenience you can create `nano ~/.config/X11/nvidia-xinitrc` and put there your favourite window manager:

    if [ $# -gt 0 ]; then
        $*
    else
        openbox-session
    #   startkde
    fi


With this you do not need to specify the app and you can simply run:

    nvidia-xrun

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

### Xorg cannot start in Debian
You should comment all "files" section in /etc/X11/nvidia-xorg.conf like this:
`#Section "Files"
#  ModulePath "/usr/lib/nvidia"
#  ModulePath "/usr/lib32/nvidia"
#  ModulePath "/usr/lib32/nvidia/xorg/modules"
#  ModulePath "/usr/lib32/xorg/modules"
#  ModulePath "/usr/lib64/nvidia/xorg/modules"
#  ModulePath "/usr/lib64/nvidia/xorg"
#  ModulePath "/usr/lib64/xorg/modules"
#EndSection`

### cannot unload "nvidia-drm" before nvidia-xrun
I don't know why, in my debian loading "nvidia_drm modeset=1" cause nvidia_drm cannot be unloaded without kill all X server (even intel graphic X server). More if the script try to remove the nvidia card at this moment, it cause a kernel bug who cause shutdown infinite loop (you must make a forced outage) and you will not be able to kill the "nvidia-xrun-util turn_off_gpu" process.
I must replace **"nvidia_drm modeset=1"** by **nvidia_drm** in /etc/default/nvidia-xrun
