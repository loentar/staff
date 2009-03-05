
create user staffdbuser with password 'staffdbuserpassword';
create database staff;
grant all privileges on database staff to staffdbuser;
\c staff
\i db/staff.sql
grant all on "context", "users", "session", "objects", "groups", "usertogroups", "objecttypes" to staffdbuser;
