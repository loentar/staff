---------------------------------------------
-- db staff
---------------------------------------------
-- sqlite_version() >= 3.6.19

-- enabling foreign keys
PRAGMA foreign_keys = ON;

BEGIN TRANSACTION;

---------------------------------------------
-- cleanup

-- indexes
DROP INDEX IF EXISTS uni_users_to_groups;
DROP INDEX IF EXISTS uni_session;
DROP INDEX IF EXISTS uni_objects;
DROP INDEX IF EXISTS uni_objects_to_users_acl;
DROP INDEX IF EXISTS uni_objects_to_groups_acl;

-- tables
DROP TABLE IF EXISTS sessions;
DROP TABLE IF EXISTS objects_to_users_acl;
DROP TABLE IF EXISTS objects_to_groups_acl;
DROP TABLE IF EXISTS users_to_groups;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS groups;
DROP TABLE IF EXISTS objects;
DROP TABLE IF EXISTS settings;

---------------------------------------------
---------------------------------------------
-- tables

---------------------------------------------
-- sessions

CREATE TABLE sessions
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  sessionid TEXT(32) NOT NULL DEFAULT (lower(hex(randomblob(16)))),
  userid INTEGER UNIQUE NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  expires INTEGER
);

---------------------------------------------
-- users

CREATE TABLE users
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  password TEXT,
  description TEXT DEFAULT ''
);

---------------------------------------------
-- groups

CREATE TABLE groups
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  description TEXT DEFAULT ''
);

---------------------------------------------
-- users to groups

CREATE TABLE users_to_groups
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  userid INTEGER NOT NULL REFERENCES users(id) ON DELETE CASCADE,
  groupid INTEGER NOT NULL REFERENCES groups(id) ON DELETE CASCADE
);

CREATE UNIQUE INDEX uni_users_to_groups ON users_to_groups(userid, groupid);

---------------------------------------------
-- objects

CREATE TABLE objects
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL,
  description TEXT DEFAULT '',
  parentid INTEGER NOT NULL REFERENCES objects(id) ON DELETE CASCADE
);

CREATE UNIQUE INDEX uni_objects ON objects(name, parentid);

---------------------------------------------
-- acl

-- acl to users

CREATE TABLE objects_to_users_acl
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  objectid INTEGER NOT NULL REFERENCES objects(id) ON DELETE CASCADE,
  userid INTEGER REFERENCES users(id) ON DELETE CASCADE,
  permit INTEGER NOT NULL DEFAULT 0
);

CREATE UNIQUE INDEX uni_objects_to_users_acl ON objects_to_users_acl(objectid, userid);


-- acl to groups

CREATE TABLE objects_to_groups_acl
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  objectid INTEGER NOT NULL REFERENCES objects(id) ON DELETE CASCADE,
  groupid INTEGER REFERENCES groups(id) ON DELETE CASCADE,
  permit INTEGER NOT NULL DEFAULT 0
);

CREATE UNIQUE INDEX uni_objects_to_groups_acl ON objects_to_groups_acl(objectid, groupid);

---------------------------------------------
-- settings

CREATE TABLE settings
(
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  value TEXT
);


---------------------------------------------
---------------------------------------------
-- data

---------------------------------------------
-- users

INSERT INTO users(id, name, password, description) VALUES (0, 'nobody', NULL, 'system user');
INSERT INTO users(id, name, password, description) VALUES (1, 'admin', 'admin', 'Administrator');
INSERT INTO users(id, name, password, description) VALUES (2, 'user', 'user', 'User');
INSERT INTO users(id, name, password, description) VALUES (3, 'guest', 'guest', 'Guest');

UPDATE sqlite_sequence SET seq=100 WHERE name='users';

---------------------------------------------
-- groups

INSERT INTO groups(id, name, description) VALUES (1, 'admin', 'Administrators');
INSERT INTO groups(id, name, description) VALUES (2, 'user', 'Users');
INSERT INTO groups(id, name, description) VALUES (3, 'guest', 'Guests');

UPDATE sqlite_sequence SET seq=100 WHERE name='groups';

---------------------------------------------
-- users to groups

INSERT INTO users_to_groups(userid, groupid) VALUES (1, 1);
INSERT INTO users_to_groups(userid, groupid) VALUES (2, 2);
INSERT INTO users_to_groups(userid, groupid) VALUES (3, 3);

---------------------------------------------
-- sessions

INSERT INTO sessions(id, sessionid, userid, expires) VALUES (0, '00000000000000000000000000000000', 0, 2147483647);

---------------------------------------------
-- objects

INSERT INTO objects(id, name, description, parentid) VALUES (0, 'root', 'Root object', 0);
INSERT INTO objects(id, name, description, parentid) VALUES (1, 'component', 'Components', 0);
INSERT INTO objects(id, name, description, parentid) VALUES (2, 'widget', 'Widgets', 0);

-- component.staff.admin (access to admin services)
INSERT INTO objects(id, name, description, parentid) VALUES (3, 'staff', 'staff component', 1);
INSERT INTO objects(id, name, description, parentid) VALUES (4, 'admin', 'staff.admin component', 3);

-- webapp.admin (access to webapp's admin services)
INSERT INTO objects(id, name, description, parentid) VALUES (5, 'webapp', 'webapp namespace', 0);
INSERT INTO objects(id, name, description, parentid) VALUES (6, 'admin', 'webapp.admin component', 5);

-- widget.admin, webapp.widget.admin (access to admin widgets)
INSERT INTO objects(id, name, description, parentid) VALUES (7, 'admin', 'widget.admin namespace', 2);
INSERT INTO objects(id, name, description, parentid) VALUES (8, 'widget', 'webapp.widget namespace', 5);
INSERT INTO objects(id, name, description, parentid) VALUES (9, 'admin', 'webapp.widget.admin namespace', 8);

-- component.widget
INSERT INTO objects(id, name, description, parentid) VALUES (10, 'widget', 'widget component', 1);
INSERT INTO objects(id, name, description, parentid) VALUES (11, 'WidgetManager', 'widget manager service', 10);
INSERT INTO objects(id, name, description, parentid) VALUES (12, 'AddProfile', '', 11);
INSERT INTO objects(id, name, description, parentid) VALUES (13, 'DeleteProfile', '', 11);
INSERT INTO objects(id, name, description, parentid) VALUES (14, 'SetProfile', '', 11);
INSERT INTO objects(id, name, description, parentid) VALUES (15, 'GetBaseProfiles', '', 11);

---------------------------------------------
-- acl

-- access to root object - deny all, grant admins
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (0, 1, 1);

-- access to components - grant all
INSERT INTO objects_to_users_acl(objectid, userid, permit) VALUES (1, NULL, 1);

-- access to widgets - grant all
INSERT INTO objects_to_users_acl(objectid, userid, permit) VALUES (2, NULL, 1);

-- access to webapp - grant all
INSERT INTO objects_to_users_acl(objectid, userid, permit) VALUES (5, NULL, 1);

-- access to admin components and widgets - deny all, grant admins
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (4, NULL, 0); -- staff.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (6, NULL, 0); -- webapp.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (7, NULL, 0); -- widget.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (9, NULL, 0); -- webapp.widget.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (4, 1, 1); -- staff.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (6, 1, 1); -- webapp.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (7, 1, 1); -- widget.admin
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (9, 1, 1); -- webapp.widget.admin

-- deny guests to create(get base, delete, update) own profiles
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (12, 3, 1);
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (13, 3, 1);
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (14, 3, 1);
INSERT INTO objects_to_groups_acl(objectid, groupid, permit) VALUES (15, 3, 1);


---------------------------------------------
-- settings

INSERT INTO settings(name, value) VALUES ('sessionexpiration', '600');

COMMIT;

