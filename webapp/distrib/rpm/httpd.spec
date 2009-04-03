%define _topdir  __topdir__

Summary: webapp
Name: webapp
Version: __version__
Release: httpd
Copyright: 2009
Group: Utilities/System
Requires: sgod webapp
Provides: webapp-httpd

%description
Прокси-модуль для вызова сервисов axis2c/staff через Apache2

%files
%defattr(0755,root,root)
/www/conf/mods-enabled/
/www/modules/

%pre

%preun

%post
grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n' >> /www/conf/httpd.conf

%postun
