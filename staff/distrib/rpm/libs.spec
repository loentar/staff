%define _topdir  __topdir__

Summary: staff
Name: staff
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Requires: rise axis2c
Provides: staff

%description
Web service framework staff-__version__ based on Axis2/C.

%files
%defattr(-,root,root)
/etc/profile.d/staff.sh
/etc/ld.so.conf.d/staff.conf
/usr/local/staff/lib/*.so
/usr/local/staff/components/
/usr/local/staff/db/
/usr/local/staff/samples/
/usr/local/axis2c/

%pre

%preun

%post
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig
/etc/init.d/axis2c status >/dev/null && /etc/init.d/axis2c restart || true

%postun
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig
/etc/init.d/axis2c status >/dev/null && /etc/init.d/axis2c restart || true
rm -Rf /usr/local/staff
