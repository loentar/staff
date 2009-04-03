%define _topdir  __topdir__

Summary: staff
Name: staff
Version: __version__
Release: libs
Copyright: 2009
Group: Utilities/System
Requires: rise
Provides: staff-libs

%description
staff

%files
%defattr(-,root,root)
/etc/profile.d/axis2c.sh
/etc/profile.d/staff.sh
/etc/ld.so.conf.d/axis2c.conf
/etc/ld.so.conf.d/staff.conf
/usr/local/staff/lib/
/usr/local/staff/components/
/usr/local/staff/staff.xml
/usr/local/staff/samples/
/usr/local/axis2c/

%pre

%preun

%post
ldconfig

%postun
