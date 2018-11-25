Name: nvidia-xrun		
Version: 1
Release: 1%{?dist}
Summary: Utility to run separate X with discrete nvidia graphics with full performance.	

License: GNU GPL V2
Source0: nvidia-xrun
Source1: nvidia-xorg.conf
Source2: nvidia-xinitrc
Source3: nvidia-toggle
buildroot: %{_tmppath}/%{name}-root
BuildArch: noarch
%description
These utility scripts aim to make the life easier for nvidia cards users. It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization.

%install
mkdir -p %{buildroot}/%{_bindir}
install -pm 755 %{SOURCE0} %{buildroot}/%{_bindir}

mkdir -p %{buildroot}/%{_sysconfdir}/X11/xinit/
install -pm 644 %{SOURCE1} %{buildroot}/%{_sysconfdir}/X11/
install -pm 755 %{SOURCE2} %{buildroot}/%{_sysconfdir}/X11/xinit/

%files
%{_bindir}/nvidia-xrun
%{_bindir}/nvidia-toggle
%{_sysconfdir}/X11/nvidia-xorg.conf
%{_sysconfdir}/X11/xinit
%{_sysconfdir}/X11/xinit/nvidia-xinitrc

%changelog

