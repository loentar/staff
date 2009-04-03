%define _topdir  __topdir__

Summary: staff
Name: staff
Version: __version__
Release: mcbc
Copyright: 2009
Group: Utilities/System
Requires: rise postgresql-libs
Provides: staff

%description
staff

%files
%defattr(-,root,root)
/etc/profile.d/axis2c.sh
/etc/profile.d/staff.sh
/etc/ld.so.conf.d/axis2c.conf
/etc/ld.so.conf.d/staff.conf
/usr/local/staff/lib/
/usr/local/staff/components/
/usr/local/staff/staff.xml
/usr/local/staff/db/
/usr/local/staff/samples/
/usr/local/axis2c/

%pre

%preun
echo -n "Удаление БД..."
cd /usr/local/staff/ && psql -q -U postgres template1 < db/uninstdb.sql && echo "     ОК"

%post
ldconfig
echo -n "Установка БД..."
cd /usr/local/staff/ && psql -q -U postgres template1 < db/instdb.sql >/dev/null && echo "     ОК"
cat << CONFIG_MESSAGE_END
Для завершения настройки staff:
 1) добавьте следующие строки в /usr/local/axis2c/axis2.xml:
 
    <module ref="staff"/>
    <module ref="staff_security"/>
    
    следом за строкой:
    <module ref="addressing"/>

 2) Отредактируйте файл настроект доступа к БД:
 
    mcedit /var/lib/pgsql/data/pg_hba.conf

    закоментируйте строку, добавив в её начало символ '#'

    #local  all      all        127.0.0.1    255.255.255.255   pam
    
    добавьте строку
    
    local  all      all        127.0.0.1    255.255.255.255   md5

CONFIG_MESSAGE_END

%postun
