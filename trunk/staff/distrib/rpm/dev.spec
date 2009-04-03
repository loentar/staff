%define _topdir  __topdir__

Summary: staff
Name: staff-dev
Version: __version__
Release: mcbc
Copyright: 2009
Group: Utilities/System
Requires: staff rise-dev postgresql-devel
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
