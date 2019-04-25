%define version 0.3.2
Name: nvidia-xrun-pm
Version: %{version}
Release: 2%{?dist}
Summary: Utility to run separate X with discrete nvidia graphics with full performance.
License: GNU GPLv2
buildroot: %{_tmppath}/%{name}-root
BuildArch: noarch
Conflicts: nvidia-xrun
# This forces the remote source code to be downloaded during the rpmbuild.
# It is ignored on Fedora COPR. A source RPM must be built elsewhere
# before being uploaded to be built as a binary RPM.
%undefine _disable_source_fetch
Source0: https://github.com/michelesr/nvidia-xrun-pm/archive/%{version}.tar.gz
# The "systemd" package is required for the "_unitdir" macro.
BuildRequires: systemd

%description
This fork provides the benefits of nvidia-xrun without the extra dependency of bumblebee for power management. These utility scripts aim to make the life easier for nvidia cards users. It started with a revelation that bumblebee in current state offers very poor performance. This solution offers a bit more complicated procedure but offers a full GPU utilization.

%prep
# Extract the source tarball to ~/rpmbuild/BUILD/.
%setup -q

%install
mkdir -p %{buildroot}/%{_bindir}
install -pm 755 %{_builddir}/nvidia-xrun-pm-%{version}/nvidia-xrun %{buildroot}/%{_bindir}
mkdir -p %{buildroot}/%{_sysconfdir}/X11/xinit/
install -pm 644 %{_builddir}/nvidia-xrun-pm-%{version}/nvidia-xorg.conf %{buildroot}/%{_sysconfdir}/X11/
install -pm 755 %{_builddir}/nvidia-xrun-pm-%{version}/nvidia-xinitrc %{buildroot}/%{_sysconfdir}/X11/xinit/
mkdir -p %{buildroot}/%{_sysconfdir}/default
install -pm 644 %{_builddir}/nvidia-xrun-pm-%{version}/config/nvidia-xrun %{buildroot}/%{_sysconfdir}/default/
mkdir -p %{buildroot}/%{_unitdir}/
install -pm 644 %{_builddir}/nvidia-xrun-pm-%{version}/nvidia-xrun-pm.service %{buildroot}/%{_unitdir}/

%files
%{_bindir}/nvidia-xrun
%{_sysconfdir}/X11/nvidia-xorg.conf
%{_sysconfdir}/X11/xinit
%{_sysconfdir}/X11/xinit/nvidia-xinitrc
%{_sysconfdir}/default/nvidia-xrun
%{_unitdir}/nvidia-xrun-pm.service

%changelog
* Thu Apr 25 2019 Luke Short <ekultails@gmail.com> 0.3.2-2
- Use GitHub release tarballs instead of a git clone

* Wed Apr 24 2019 Luke Short <ekultails@gmail.com> 0.3.2-1
- Use the git version tag for the RPM
- Add the new default nvidia-xrun configuration file

* Thu Jan 31 2019 Luke Short <ekultails@gmail.com> 20181128-1
- Change to ISO date version format
- Mark nvidia-xrun as a conflicting package
- Add nvidia-xrun-pm systemd service file

* Thu Dec 28 2017 Abhiram Kuchibhotla <mailto:7677954+AxelSilverdew@users.noreply.github.com> 1
- Initial RPM spec release
