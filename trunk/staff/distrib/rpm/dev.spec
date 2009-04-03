%define _topdir  __topdir__

Summary: staff
Name: staff
Version: __version__
Release: dev
Copyright: 2009
Group: Utilities/System
Requires: staff-libs
Provides: staff-dev

%description
Средства разработки под staff

%files
%defattr(0755,root,root)
/usr/local/staff/include/
/usr/local/staff/bin/

%pre

%preun

%post
ldconfig

%postun
