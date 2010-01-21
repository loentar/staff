--
-- PostgreSQL database dump
--

SET SESSION AUTHORIZATION 'postgres';

--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET search_path = public, pg_catalog;

--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE users (
    id integer DEFAULT nextval('users_sequence'::text) NOT NULL,
    username character varying NOT NULL,
    "password" character varying,
    description character varying
) WITHOUT OIDS;


--
-- Name: users; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE users FROM PUBLIC;
GRANT ALL ON TABLE users TO staffdbuser;


--
-- Name: users_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE users_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 100
    CACHE 1;


--
-- Name: users_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE users_sequence FROM PUBLIC;
GRANT ALL ON TABLE users_sequence TO staffdbuser;


--
-- Name: session; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE "session" (
    id integer DEFAULT nextval('session_sequence'::text) NOT NULL,
    sid character varying(32) DEFAULT md5((now())::text) NOT NULL,
    userid integer NOT NULL,
    "time" timestamp without time zone DEFAULT now() NOT NULL,
    extraid integer DEFAULT 0 NOT NULL
) WITHOUT OIDS;


--
-- Name: session; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE "session" FROM PUBLIC;
GRANT ALL ON TABLE "session" TO staffdbuser;


--
-- Name: objects_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE objects_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 100
    CACHE 1;


--
-- Name: objects_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE objects_sequence FROM PUBLIC;
GRANT ALL ON TABLE objects_sequence TO staffdbuser;


--
-- Name: objects; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE objects (
    id integer DEFAULT nextval('objects_sequence'::text) NOT NULL,
    name character varying NOT NULL,
    "type" integer NOT NULL,
    description character varying,
    userid integer DEFAULT 0 NOT NULL,
    groupid integer DEFAULT 0 NOT NULL,
    parentid integer DEFAULT 0 NOT NULL,
    permission integer DEFAULT 0 NOT NULL
) WITHOUT OIDS;


--
-- Name: objects; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE objects FROM PUBLIC;
GRANT ALL ON TABLE objects TO staffdbuser;


--
-- Name: groups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE groups (
    id integer DEFAULT nextval('groups_sequence'::text) NOT NULL,
    name character varying NOT NULL,
    description character varying
) WITHOUT OIDS;


--
-- Name: groups; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE groups FROM PUBLIC;
GRANT ALL ON TABLE groups TO staffdbuser;


--
-- Name: groups_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE groups_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 100
    CACHE 1;


--
-- Name: groups_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE groups_sequence FROM PUBLIC;
GRANT ALL ON TABLE groups_sequence TO staffdbuser;


--
-- Name: users_to_groups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE users_to_groups (
    id integer DEFAULT nextval('users_to_groups_sequence'::text) NOT NULL,
    userid integer NOT NULL,
    groupid integer NOT NULL
) WITHOUT OIDS;


--
-- Name: users_to_groups; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE users_to_groups FROM PUBLIC;
GRANT ALL ON TABLE users_to_groups TO staffdbuser;


--
-- Name: users_to_groups_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE users_to_groups_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- Name: users_to_groups_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE users_to_groups_sequence FROM PUBLIC;
GRANT ALL ON TABLE users_to_groups_sequence TO staffdbuser;


--
-- Name: session_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE session_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- Name: session_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE session_sequence FROM PUBLIC;
GRANT ALL ON TABLE session_sequence TO staffdbuser;


--
-- Name: objecttypes; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE objecttypes (
    id integer NOT NULL,
    name character varying NOT NULL,
    description character varying
) WITHOUT OIDS;


--
-- Name: objecttypes; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE objecttypes FROM PUBLIC;
GRANT ALL ON TABLE objecttypes TO staffdbuser;


--
-- Name: objecttypes_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE objecttypes_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: objecttypes_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE objecttypes_sequence FROM PUBLIC;
GRANT ALL ON TABLE objecttypes_sequence TO staffdbuser;


--
-- Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY users (id, username, "password", description) FROM stdin;
0	admin	admin	Administrator
1	sysadmin	sysadmin	System administrator
100	user	user	User
101	guest	guest	Guest
2	nobody	\N	System user
\.


--
-- Name: session; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "session" (id, sid, userid, "time", extraid) FROM stdin;
0	00000000000000000000000000000000	2	9999-12-31 23:59:59.999999	0
\.


--
-- Name: objects; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objects (id, name, "type", description, userid, groupid, parentid, permission) FROM stdin;
0	root	0	root object	0	0	0	480
1	ROOTCOMPONENT	1	root component	1	1	0	511
2	ROOTWIDGET	4	root widget	1	1	0	511
3	staff	1	Staff component	1	1	1	511
4	admin	1	Staff administration component	1	1	3	504
5	webapp	1	webapp	1	1	1	511
6	admin	1		1	1	5	504
7	webapp.widget.admin.FileUploader	4		0	0	2	504
8	webapp.widget.admin.AccountAdmin	4		0	0	2	504
9	webapp.widget.admin.ActiveServices	4		1	1	2	504
10	webapp.widget.admin.ObjectAdmin	4		1	1	2	504
11	webapp.widget.admin.ProfileAdmin	4		0	0	2	504
12	AccountAdmin	2		1	0	4	504
13	FileUploader	2		1	0	6	504
14	ProfileAdmin	2		1	0	6	504
15	Login	2		1	1	3	511
16	Login	3		2	1	15	448
17	OpenSession	3		2	1	15	448
18	widget	1		1	1	1	511
19	WidgetManager	2		1	2	18	511
20	GetBaseProfiles	3		1	2	19	504
21	AddProfile	3		1	2	19	504
22	DeleteProfile	3		1	2	19	504
23	ObjectAdmin	2		1	0	4	504
\.


--
-- Name: groups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY groups (id, name, description) FROM stdin;
0	admin	Admistrators
2	user	Users
3	guest	Guests
1	sysadmin	System administrators
\.


--
-- Name: users_to_groups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY users_to_groups (id, userid, groupid) FROM stdin;
0	0	0
1	1	0
2	1	1
6	101	3
3	0	2
4	1	2
5	100	2
\.


--
-- Name: objecttypes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objecttypes (id, name, description) FROM stdin;
0	root	root object
1	component	Component
2	service	Service
3	operation	Service operation
4	widget	Widget
\.


--
-- Name: userid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users
    ADD CONSTRAINT userid PRIMARY KEY (id);


--
-- Name: username_unique; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users
    ADD CONSTRAINT username_unique UNIQUE (username);


--
-- Name: sessionid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT sessionid PRIMARY KEY (id);


--
-- Name: pk_object; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT pk_object PRIMARY KEY (id);


--
-- Name: pk_groupid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT pk_groupid PRIMARY KEY (id);


--
-- Name: uni_groupname; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT uni_groupname UNIQUE (name);


--
-- Name: pk_id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users_to_groups
    ADD CONSTRAINT pk_id PRIMARY KEY (id);


--
-- Name: uni_objectnametypeparentid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT uni_objectnametypeparentid UNIQUE (name, "type", parentid);


--
-- Name: pk_objecttypeid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objecttypes
    ADD CONSTRAINT pk_objecttypeid PRIMARY KEY (id);


--
-- Name: uni_extraid_userid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT uni_extraid_userid UNIQUE (extraid, userid);


--
-- Name: uni_usergroup; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users_to_groups
    ADD CONSTRAINT uni_usergroup UNIQUE (userid, groupid);


--
-- Name: fk_parent; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_parent FOREIGN KEY (parentid) REFERENCES objects(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users_to_groups
    ADD CONSTRAINT fk_userid FOREIGN KEY (userid) REFERENCES users(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_groupid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users_to_groups
    ADD CONSTRAINT fk_groupid FOREIGN KEY (groupid) REFERENCES groups(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_objecttype; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_objecttype FOREIGN KEY ("type") REFERENCES objecttypes(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_userid FOREIGN KEY (userid) REFERENCES users(id) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: users_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('users_sequence', 101, true);


--
-- Name: objects_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('objects_sequence', 100, true);


--
-- Name: groups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('groups_sequence', 100, true);


--
-- Name: users_to_groups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('users_to_groups_sequence', 6, true);


--
-- Name: session_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('session_sequence', 0, true);


--
-- Name: objecttypes_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('objecttypes_sequence', 4, true);


--
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA public IS 'Standard public schema';


