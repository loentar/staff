%define _topdir  __topdir__

Summary: staff-das
Name: staff-das-dev
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Requires: staff-das staff-dev rise-dev
Provides: staff-das-dev

%description
Data access service for web service framework staff-2.0 development headers and tools.

%files
%defattr(-,root,root)
/usr/local/staff/include/
/usr/local/staff/lib/codegen/

%pre

%preun

%post

%postun
