%define _topdir  __topdir__

Summary: rise
Name: rise
Version: __version__
Release: mcbc
Copyright: 2009
Group: Utilities/System
#Requires: 
Provides: rise

%description
Многоплатформенная библиотека классов rise

%files
%defattr(-,root,root)
/usr/lib/

#%pre

#%preun

%post
test -f /usr/lib/libbfd.so || ln -s /usr/lib/libbfd-2.14.90.0.6.so /usr/lib/libbfd.so
#ldconfig

#%postun
