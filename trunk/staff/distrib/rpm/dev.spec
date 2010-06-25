%define _topdir  __topdir__

Summary: staff
Name: staff-dev
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Requires: staff rise-dev
Provides: staff-dev

%description
Web service framework staff-__version__ development headers and tools.

%files
%defattr(-,root,root)
/usr/local/staff/include/
/usr/local/staff/bin/
/usr/local/staff/lib/codegen/

%pre

%preun

%post
ldconfig

%postun
