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
echo -n "õÄÁÌÅÎÉÅ âä ðé..."
/etc/init.d/postgresql status >/dev/null || /etc/init.d/postgresql start
sleep 1 ## database system is starting up
cd /usr/local/staff/ && psql -q -U postgres template1 < db/uninstdb.sql && echo "     ïë"

%post
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig

####################################
echo "îÁÓÔÒÏÊËÁ óõâä"
/etc/init.d/postgresql status >/dev/null || /etc/init.d/postgresql start
sleep 1 ## database system is starting up
#patch -f /var/lib/pgsql/data/pg_hba.conf << POSTGRES_PATCH_END
#--- pg_hba.conf	Wed Apr  8 09:43:12 2009
#+++ pg_hba.conf	Wed Apr  8 09:43:16 2009
#@@ -58,2 +58,3 @@ local   all         postgres            
# local   all         all                                             pam
#-host    all         all         127.0.0.1         255.255.255.255   pam
#+#host    all         all         127.0.0.1         255.255.255.255   pam
#+host    all         all         127.0.0.1         255.255.255.255   md5
#POSTGRES_PATCH_END
#"mktmp: Permission denied" while sudo -u postgres patch
#chown postgres:postgres /var/lib/pgsql/data/pg_hba.conf

#/etc/init.d/postgresql reload

####################################
echo -n "õÓÔÁÎÏ×ËÁ âä ðé..."
cd /usr/local/staff/ && psql -q -U postgres template1 < db/instdb.sql >/dev/null && echo "     ïë"

%postun

####################################
#patch /var/lib/pgsql/data/pg_hba.conf << POSTGRES_UNPATCH_END
#--- pg_hba.conf	Wed Apr  8 09:43:16 2009
#+++ pg_hba.conf	Wed Apr  8 09:43:12 2009
#@@ -58,3 +58,2 @@ local   all         postgres            
# local   all         all                                             pam
#-#host    all         all         127.0.0.1         255.255.255.255   pam
#-host    all         all         127.0.0.1         255.255.255.255   md5
#+host    all         all         127.0.0.1         255.255.255.255   pam
#POSTGRES_UNPATCH_END
#"mktmp: Permission denied" while sudo -u postgres patch
#chown postgres:postgres /var/lib/pgsql/data/pg_hba.conf

#/etc/init.d/postgresql reload
