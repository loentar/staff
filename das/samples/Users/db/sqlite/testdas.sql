---------------------------------------------
-- sqlite_version() >= 3.6.19

-- enabling foreign keys
PRAGMA foreign_keys = ON;

BEGIN TRANSACTION;

---------------------------------------------
-- cleanup

-- indexes
DROP INDEX IF EXISTS uni_users_to_groups;

-- tables
DROP TABLE IF EXISTS users_to_groups;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS groups;

---------------------------------------------
---------------------------------------------
-- tables

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
---------------------------------------------
-- data

---------------------------------------------
-- users

INSERT INTO users(id, name, password, description) VALUES (0, 'admin', 'admin', 'Administrator');
INSERT INTO users(id, name, password, description) VALUES (1, 'nobody', NULL, 'system user');
INSERT INTO users(id, name, password, description) VALUES (2, 'user', 'user', 'User');
INSERT INTO users(id, name, password, description) VALUES (3, 'guest', 'guest', 'Guest');

---------------------------------------------
-- groups

INSERT INTO groups(id, name, description) VALUES (0, 'admin', 'Administrators');
INSERT INTO groups(id, name, description) VALUES (1, 'user', 'Users');
INSERT INTO groups(id, name, description) VALUES (2, 'guest', 'Guests');

---------------------------------------------
-- users to groups

INSERT INTO users_to_groups(userid, groupid) VALUES (0, 0);
INSERT INTO users_to_groups(userid, groupid) VALUES (2, 1);
INSERT INTO users_to_groups(userid, groupid) VALUES (3, 2);

COMMIT;

