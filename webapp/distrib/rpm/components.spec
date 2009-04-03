%define _topdir  __topdir__

Summary: webapp
Name: webapp
Version: __version__
Release: components
Copyright: 2009
Group: Utilities/System
Requires: staff
Provides: webapp-components

%description
Сервисные компоненты webapp

%files
%defattr(-,root,root)
/usr/local/staff/components/

%pre

%preun

%post

%postun
