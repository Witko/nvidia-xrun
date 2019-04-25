%define nvidia_xrun_pm_version 0.3.2
Name: nvidia-xrun-pm
Version: %{nvidia_xrun_pm_version}
Release: 1%{?dist}
Summary: Utility to run separate X with discrete nvidia graphics with full performance.
License: GNU GPLv2
buildroot: %{_tmppath}/%{name}-root
BuildArch: noarch
Conflicts: nvidia-xrun
# The "systemd" package is required for the %{_unitdir} macro.
BuildRequires: git systemd

%description
This fork provides the benefits of nvidia-xrun without the extra dependency of bumblebee for power management. These utility scripts aim to make the life easier for nvidia cards users. It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization.

%build
rm -rf nvidia-xrun-pm
# SSL verification is turned off to temporarily workaround openSUSE Leap build issues on COPR.
git -c http.sslVerify=false clone --depth 1 --single-branch --branch %{nvidia_xrun_pm_version}\
 https://github.com/michelesr/nvidia-xrun-pm.git

%install
mkdir -p %{buildroot}/%{_bindir}
install -pm 755 nvidia-xrun-pm/nvidia-xrun %{buildroot}/%{_bindir}
mkdir -p %{buildroot}/%{_sysconfdir}/X11/xinit/
install -pm 644 nvidia-xrun-pm/nvidia-xorg.conf %{buildroot}/%{_sysconfdir}/X11/
install -pm 755 nvidia-xrun-pm/nvidia-xinitrc %{buildroot}/%{_sysconfdir}/X11/xinit/
mkdir -p %{buildroot}/%{_sysconfdir}/default
install -pm 644 nvidia-xrun-pm/config/nvidia-xrun %{buildroot}/%{_sysconfdir}/default/
mkdir -p %{buildroot}/%{_unitdir}/
install -pm 644 nvidia-xrun-pm/nvidia-xrun-pm.service %{buildroot}/%{_unitdir}/

%files
%{_bindir}/nvidia-xrun
%{_sysconfdir}/X11/nvidia-xorg.conf
%{_sysconfdir}/X11/xinit
%{_sysconfdir}/X11/xinit/nvidia-xinitrc
%{_sysconfdir}/default/nvidia-xrun
%{_unitdir}/nvidia-xrun-pm.service

%changelog
* Wed Apr 24 2019 Luke Short <ekultails@gmail.com> 0.3.2-1
- Use the git version tag for the RPM
- Add the new default nvidia-xrun configuration file

* Thu Jan 31 2019 Luke Short <ekultails@gmail.com> 20181128-1
- Change to ISO date version format
- Mark nvidia-xrun as a conflicting package
- Add nvidia-xrun-pm systemd service file

* Thu Dec 28 2017 Abhiram Kuchibhotla <mailto:7677954+AxelSilverdew@users.noreply.github.com> 1
- Initial RPM spec release
