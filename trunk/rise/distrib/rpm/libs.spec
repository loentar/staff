%define _topdir  __topdir__

Summary: rise
Name: rise
Version: __version__
Release: libs
Copyright: 2009
Group: Utilities/System
Requires: /usr/lib/libbfd.so
Provides: rise

%description
Многоплатформенная библиотека классов rise

%files
%defattr(-,root,root)
/usr/lib/

#%pre

#%preun

#%post
#ldconfig

#%postun
