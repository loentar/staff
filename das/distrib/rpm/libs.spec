%define _topdir  __topdir__

Summary: staff-das
Name: staff-das
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Requires: rise staff
Provides: staff-das

%description
Data access service for web service framework staff-2.0.

%files
%defattr(-,root,root)
/usr/local/staff/lib/*.so
/usr/local/staff/components/

#%pre

#%preun

%post
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig
/etc/init.d/axis2c status >/dev/null && /etc/init.d/axis2c restart || true

%postun
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig
/etc/init.d/axis2c status >/dev/null && /etc/init.d/axis2c restart || true
