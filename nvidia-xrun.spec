Name: nvidia-xrun		
Version: 1
Release: 1%{?dist}
Summary: Utility to run separate X with discrete nvidia graphics with full performance.	

License: GNU GPL V2
Source0: nvidia-xrun
Source1: nvidia-xorg.conf
Source2: nvidia-xinitrc
BuildRoot: %{_tmppath}/%{name}-root
BuildArch: noarch
%description
These utility scripts aim to make the life easier for nvidia cards users. It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization.

%install
[ "$RPM_BUILD_ROOT" != "/" ] && rm -fr $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT/usr/bin/
install -pm 755 $RPM_BUILD_DIR/nvidia-xrun $RPM_BUILD_ROOT/usr/bin/nvidia-xrun

mkdir -p $RPM_BUILD_ROOT/etc/X11/xinit/
install -pm 644 $RPM_BUILD_DIR/nvidia-xorg.conf $RPM_BUILD_ROOT/etc/X11/nvidia-xorg.conf

install -pm 644 $RPM_BUILD_DIR/nvidia-xinitrc $RPM_BUILD_ROOT/etc/X11/xinit/nvidia-xinitrc


%files
/usr/bin/nvidia-xrun
/etc/X11/nvidia-xorg.conf
/etc/X11/xinit/nvidia-xinitrc

%changelog

