%define _topdir  __topdir__

Summary: staff
Name: staff
Version: __version__
Release: mcbc
Copyright: 2009
Group: Utilities/System
Requires: rise postgresql-libs postgresql-server
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
echo -n "Удаление БД ПИ..."
/etc/init.d/postgresql status >/dev/null || /etc/init.d/postgresql start
sleep 1 ## database system is starting up
cd /usr/local/staff/ && psql -q -U postgres template1 < db/uninstdb.sql && echo "     ОК"

%post
cat /etc/ld.so.conf.d/*.conf | xargs ldconfig

####################################
echo "Настройка Axis2/C"
patch /usr/local/axis2c/axis2.xml << AXIS2C_PATCH_END
--- axis2.xml	Wed Apr  8 09:28:14 2009
+++ axis2.xml	Wed Apr  8 09:30:30 2009
@@ -83,4 +83,6 @@
     <!-- Comment this to disable Addressing -->
     <module ref="addressing"/>
+    <module ref="staff"/>
+    <module ref="staff_security"/>
 
 
AXIS2C_PATCH_END

####################################
echo "Настройка СУБД"
/etc/init.d/postgresql status >/dev/null || /etc/init.d/postgresql start
sleep 1 ## database system is starting up
patch -f /var/lib/pgsql/data/pg_hba.conf << POSTGRES_PATCH_END
--- pg_hba.conf	Wed Apr  8 09:43:12 2009
+++ pg_hba.conf	Wed Apr  8 09:43:16 2009
@@ -58,2 +58,3 @@ local   all         postgres            
 local   all         all                                             pam
-host    all         all         127.0.0.1         255.255.255.255   pam
+#host    all         all         127.0.0.1         255.255.255.255   pam
+host    all         all         127.0.0.1         255.255.255.255   md5
POSTGRES_PATCH_END
#"mktmp: Permission denied" while sudo -u postgres patch
chown postgres:postgres /var/lib/pgsql/data/pg_hba.conf

/etc/init.d/postgresql reload

####################################
echo -n "Установка БД ПИ..."
cd /usr/local/staff/ && psql -q -U postgres template1 < db/instdb.sql >/dev/null && echo "     ОК"

%postun
####################################
echo "Настройка Axis2/C"
patch /usr/local/axis2c/axis2.xml << AXIS2C_UNPATCH_END
--- axis2.xml	Wed Apr  8 09:30:30 2009
+++ axis2.xml	Wed Apr  8 09:28:14 2009
@@ -83,6 +83,4 @@
     <!-- Comment this to disable Addressing -->
     <module ref="addressing"/>
-    <module ref="staff"/>
-    <module ref="staff_security"/>
 
 
AXIS2C_UNPATCH_END

####################################
patch /var/lib/pgsql/data/pg_hba.conf << POSTGRES_UNPATCH_END
--- pg_hba.conf	Wed Apr  8 09:43:16 2009
+++ pg_hba.conf	Wed Apr  8 09:43:12 2009
@@ -58,3 +58,2 @@ local   all         postgres            
 local   all         all                                             pam
-#host    all         all         127.0.0.1         255.255.255.255   pam
-host    all         all         127.0.0.1         255.255.255.255   md5
+host    all         all         127.0.0.1         255.255.255.255   pam
POSTGRES_UNPATCH_END
#"mktmp: Permission denied" while sudo -u postgres patch
chown postgres:postgres /var/lib/pgsql/data/pg_hba.conf

/etc/init.d/postgresql reload
