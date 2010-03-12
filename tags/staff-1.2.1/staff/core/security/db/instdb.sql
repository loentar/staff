
create user staffdbuser with password 'staffdbuserpassword';
create database staff;
grant all privileges on database staff to staffdbuser;
\c staff
\i db/staff.sql
grant all on "users", "session", "objects", "groups", "users_to_groups", "objecttypes" to staffdbuser;
grant all on "users_sequence", "session_sequence", "objects_sequence", "groups_sequence", "users_to_groups_sequence" to staffdbuser;
