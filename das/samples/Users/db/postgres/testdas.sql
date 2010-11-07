DROP OWNED BY dastestuser CASCADE;
--DROP DATABASE IF EXISTS testdas;
DROP USER IF EXISTS dastestuser;

CREATE DATABASE testdas;
CREATE USER dastestuser WITH password 'dastestuserpassword';
GRANT ALL PRIVILEGES ON DATABASE testdas TO dastestuser;

\connect testdas dastestuser localhost 5432

DROP SEQUENCE IF EXISTS users_to_groups_sequence, groups_sequence, users_sequence;
DROP TABLE IF EXISTS users_to_groups, groups, users;

CREATE TABLE users
(
  id integer DEFAULT nextval('users_sequence'::text) NOT NULL,
  name VARCHAR,
  "password" VARCHAR,
  description VARCHAR
) WITHOUT OIDS;

ALTER TABLE ONLY users ADD CONSTRAINT pk_users_id PRIMARY KEY (id);
ALTER TABLE ONLY users ADD CONSTRAINT uni_users_name UNIQUE (name);

CREATE SEQUENCE users_sequence INCREMENT BY 1 NO MAXVALUE MINVALUE 1 CACHE 1;


CREATE TABLE groups
(
  id integer DEFAULT nextval('groups_sequence'::text) NOT NULL,
  name character varying NOT NULL,
  description character varying
) WITHOUT OIDS;

ALTER TABLE ONLY groups ADD CONSTRAINT pk_groups_id PRIMARY KEY (id);
ALTER TABLE ONLY groups ADD CONSTRAINT uni_groups_name UNIQUE (name);

CREATE SEQUENCE groups_sequence INCREMENT BY 1 NO MAXVALUE MINVALUE 1 CACHE 1;



CREATE TABLE users_to_groups
(
  id integer DEFAULT nextval('users_to_groups_sequence'::text) NOT NULL,
  userid integer NOT NULL,
  groupid integer NOT NULL
) WITHOUT OIDS;

ALTER TABLE ONLY users_to_groups ADD CONSTRAINT pk_users_to_groups_id PRIMARY KEY (id);
ALTER TABLE ONLY users_to_groups ADD CONSTRAINT uni_users_to_groups_usergroup UNIQUE (userid, groupid);
ALTER TABLE ONLY users_to_groups ADD CONSTRAINT fk_users_to_groups_userid FOREIGN KEY (userid) REFERENCES users(id) ON UPDATE CASCADE ON DELETE CASCADE;
ALTER TABLE ONLY users_to_groups ADD CONSTRAINT fk_users_to_groups_groupid FOREIGN KEY (groupid) REFERENCES groups(id) ON UPDATE CASCADE ON DELETE CASCADE;

CREATE SEQUENCE users_to_groups_sequence INCREMENT BY 1 NO MAXVALUE MINVALUE 1 CACHE 1;


COPY users (id, name, "password", description) FROM stdin;
0	admin	admin	Administrator
1	nobody	\N	System user
2	guest	guest	Guest
3	user	user	User
\.


COPY groups (id, name, description) FROM stdin;
0	admin	Admistrators
1	guest	Guests
2	user	Users
\.


COPY users_to_groups (id, userid, groupid) FROM stdin;
0	0	0
1	2	1
2	3	2
\.


SELECT pg_catalog.setval('users_sequence', 4, true);
SELECT pg_catalog.setval('groups_sequence', 3, true);
SELECT pg_catalog.setval('users_to_groups_sequence', 3, true);

GRANT ALL ON "users", "groups", "users_to_groups" TO dastestuser;
GRANT ALL ON "users_sequence", "groups_sequence", "users_to_groups_sequence" TO dastestuser;

