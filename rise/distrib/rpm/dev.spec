%define _topdir  __topdir__

Summary: rise
Name: rise-dev
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Requires: rise
Provides: rise-dev

%description
Multiplatform utils library headers.

%files
%defattr(-,root,root)
/usr/include/rise*

#%pre

#%preun

#%post

#%postun
