%define _topdir  __topdir__

Summary: staff
Name: staff-dev
Version: __version__
Release: mcbc
Copyright: 2010
Group: Utilities/System
Requires: staff rise-dev
Provides: staff-dev

%description
Средства разработки под staff

%files
%defattr(-,root,root)
/usr/local/staff/include/
/usr/local/staff/bin/

%pre

%preun

%post
ldconfig

%postun
