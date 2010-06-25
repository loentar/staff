%define _topdir  __topdir__

Summary: webapp
Name: webapp-httpd
Version: __version__
Release: mcbc
License: Apache License 2.0
Group: Utilities/System
Provides: webapp-httpd

%description
webapp modules

%files
%defattr(-,root,root)
/www/conf/mods-enabled/
/www/modules/*

%pre

%preun

%post
grep -q 'Include conf/mods-enabled/\*' /www/conf/httpd.conf || echo -e '\nInclude conf/mods-enabled/*\n' >> /www/conf/httpd.conf
ps -C httpd >/dev/null && /www/bin/apachectl graceful
true

%postun
ps -C httpd >/dev/null && /www/bin/apachectl graceful
true
