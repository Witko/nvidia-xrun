Name: nvidia-xrun-pm
Version: 20181128
Release: 1%{?dist}
Summary: Utility to run separate X with discrete nvidia graphics with full performance.
License: GNU GPLv2
Source0: nvidia-xrun
Source1: nvidia-xorg.conf
Source2: nvidia-xinitrc
Source3: nvidia-xrun-pm.service
buildroot: %{_tmppath}/%{name}-root
BuildArch: noarch
Conflicts: nvidia-xrun

%description
This fork provides the benefits of nvidia-xrun without the extra dependency of bumblebee for power management. These utility scripts aim to make the life easier for nvidia cards users. It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization.

%install
mkdir -p %{buildroot}/%{_bindir}
install -pm 755 %{SOURCE0} %{buildroot}/%{_bindir}
mkdir -p %{buildroot}/%{_sysconfdir}/X11/xinit/
install -pm 644 %{SOURCE1} %{buildroot}/%{_sysconfdir}/X11/
install -pm 755 %{SOURCE2} %{buildroot}/%{_sysconfdir}/X11/xinit/
mkdir -p %{buildroot}/%{_unitdir}/
install -pm 644 %{SOURCE3} %{buildroot}/%{_unitdir}/

%files
%{_bindir}/nvidia-xrun
%{_sysconfdir}/X11/nvidia-xorg.conf
%{_sysconfdir}/X11/xinit
%{_sysconfdir}/X11/xinit/nvidia-xinitrc
%{_unitdir}/nvidia-xrun-pm.service

%changelog
* Thu Jan 31 2019 Luke Short <ekultails@gmail.com> 20181128-1
- Change to ISO date version format
- Mark nvidia-xrun as a conflicting package
- Add nvidia-xrun-pm systemd service file

* Thu Dec 28 2017 Abhiram Kuchibhotla <mailto:7677954+AxelSilverdew@users.noreply.github.com> 1
- Initial RPM spec release
