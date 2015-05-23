# nvidia-xrun
These utility scripts aim to make the life easier for nvidia cards users.
It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but a full GPU utilization(in terms of linux drivers)

## Usage: 
  1) switch to free tty
  
  2) login
  
  3) run _nvidia-xrun_ _app_
  
  4) enjoy
  
  
  Currently sudo is required as the Xorg server must be run as root fo ModulePath to be taken into account.
  Also bbswitch is used to prepare the card and then perform a cleanup.
