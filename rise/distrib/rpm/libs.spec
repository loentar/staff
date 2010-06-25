%define _topdir  __topdir__

Summary: rise
Name: rise
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
#Requires: 
Provides: rise

%description
Multiplatform utils library.

%files
%defattr(-,root,root)
/usr/lib/librise*

#%pre

#%preun

%post
test -L /usr/lib/libbfd.so || ln -s $(ls /usr/lib/libbfd-*.so | head -1) /usr/lib/libbfd.so
#ldconfig

#%postun
