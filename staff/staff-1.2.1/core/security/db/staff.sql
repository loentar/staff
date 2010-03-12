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
-- Name: context; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE context (
    contextid integer DEFAULT nextval('context_sequence'::text) NOT NULL,
    userid integer
) WITHOUT OIDS;


--
-- Name: context; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE context FROM PUBLIC;
GRANT ALL ON TABLE context TO staffdbuser;


--
-- Name: context_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE context_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: context_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE context_sequence FROM PUBLIC;
GRANT ALL ON TABLE context_sequence TO staffdbuser;


--
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE users (
    userid integer DEFAULT nextval('users_sequence'::text) NOT NULL,
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
    NO MINVALUE
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
    sessionid integer DEFAULT nextval('session_sequence'::text) NOT NULL,
    sid character varying(32) DEFAULT md5((now())::text) NOT NULL,
    contextid integer NOT NULL,
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
    START WITH 12
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
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
    objectid integer DEFAULT nextval('objects_sequence'::text) NOT NULL,
    name character varying NOT NULL,
    "type" integer NOT NULL,
    description character varying,
    userid integer DEFAULT 0 NOT NULL,
    groupid integer DEFAULT 0 NOT NULL,
    parentobjectid integer DEFAULT 0 NOT NULL,
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
    groupid integer DEFAULT nextval('groups_sequence'::text) NOT NULL,
    groupname character varying NOT NULL,
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
    START WITH 3
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- Name: groups_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE groups_sequence FROM PUBLIC;
GRANT ALL ON TABLE groups_sequence TO staffdbuser;


--
-- Name: usertogroups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE usertogroups (
    id integer DEFAULT nextval('usertogroups_sequence'::text) NOT NULL,
    userid integer NOT NULL,
    groupid integer NOT NULL
) WITHOUT OIDS;


--
-- Name: usertogroups; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE usertogroups FROM PUBLIC;
GRANT ALL ON TABLE usertogroups TO staffdbuser;


--
-- Name: usertogroups_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE usertogroups_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- Name: usertogroups_sequence; Type: ACL; Schema: public; Owner: postgres
--

REVOKE ALL ON TABLE usertogroups_sequence FROM PUBLIC;
GRANT ALL ON TABLE usertogroups_sequence TO staffdbuser;


--
-- Name: session_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE session_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
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
    objecttypeid integer NOT NULL,
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
    START WITH 4
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
-- Name: context; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY context (contextid, userid) FROM stdin;
0	0
1	1
2	2
3	3
\.


--
-- Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY users (userid, username, "password", description) FROM stdin;
0	root	root	Administrator
1	guest	guest	Anonymous user
2	test	test	Test user
3	admin	admin	Administrator
\.


--
-- Name: session; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "session" (sessionid, sid, contextid, "time", extraid) FROM stdin;
1	00000000000000000000000000000000	1	9999-12-31 23:59:59.999999	0
\.


--
-- Name: objects; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objects (objectid, name, "type", description, userid, groupid, parentobjectid, permission) FROM stdin;
0	root	0	root object	0	0	0	480
1	ROOTCOMPONENT	1	root component	0	0	0	511
2	ROOTWIDGET	4	root widget	0	0	0	511
3	staff	1	Staff component	0	0	1	511
4	admin	1	Staff administration component	0	0	3	504
5	AccountAdmin	2	Account Admin service	0	0	4	504
7	GetGroups	3	Enable operation for all	0	0	5	511
6	GetUsers	3	Enable operation for all	0	0	5	511
9	webapp	1	webapp	0	0	1	511
10	FileUploader	2	file uploader	0	0	9	504
11	webapp.widget.FileUploader	4	File uploader widget	0	0	2	504
\.


--
-- Name: groups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY groups (groupid, groupname, description) FROM stdin;
0	root	Admistrators
1	guest	Guests
2	user	Users
\.


--
-- Name: usertogroups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY usertogroups (id, userid, groupid) FROM stdin;
0	0	0
1	1	1
2	3	0
3	2	2
\.


--
-- Name: objecttypes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objecttypes (objecttypeid, name, description) FROM stdin;
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
    ADD CONSTRAINT userid PRIMARY KEY (userid);


--
-- Name: userid_unique; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT userid_unique UNIQUE (userid);


--
-- Name: username_unique; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users
    ADD CONSTRAINT username_unique UNIQUE (username);


--
-- Name: contextid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT contextid PRIMARY KEY (contextid);


--
-- Name: sessionid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT sessionid PRIMARY KEY (sessionid);


--
-- Name: pk_object; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT pk_object PRIMARY KEY (objectid);


--
-- Name: pk_groupid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT pk_groupid PRIMARY KEY (groupid);


--
-- Name: uni_groupname; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT uni_groupname UNIQUE (groupname);


--
-- Name: pk_id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT pk_id PRIMARY KEY (id);


--
-- Name: uni_objectnametypeparentid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT uni_objectnametypeparentid UNIQUE (name, "type", parentobjectid);


--
-- Name: pk_objecttypeid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objecttypes
    ADD CONSTRAINT pk_objecttypeid PRIMARY KEY (objecttypeid);


--
-- Name: uni_extraid_contextid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT uni_extraid_contextid UNIQUE (extraid, contextid);


--
-- Name: uni_usergroup; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT uni_usergroup UNIQUE (userid, groupid);


--
-- Name: userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT userid FOREIGN KEY (userid) REFERENCES users(userid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: contextid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT contextid FOREIGN KEY (contextid) REFERENCES context(contextid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_parent; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_parent FOREIGN KEY (parentobjectid) REFERENCES objects(objectid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT fk_userid FOREIGN KEY (userid) REFERENCES users(userid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_groupid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT fk_groupid FOREIGN KEY (groupid) REFERENCES groups(groupid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_objecttype; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_objecttype FOREIGN KEY ("type") REFERENCES objecttypes(objecttypeid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: fk_userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_userid FOREIGN KEY (userid) REFERENCES users(userid) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- Name: context_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('context_sequence', 3, true);


--
-- Name: users_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('users_sequence', 3, true);


--
-- Name: objects_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('objects_sequence', 12, false);


--
-- Name: groups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('groups_sequence', 3, false);


--
-- Name: usertogroups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('usertogroups_sequence', 3, true);


--
-- Name: session_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('session_sequence', 6, true);


--
-- Name: objecttypes_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('objecttypes_sequence', 4, false);


--
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA public IS 'Standard public schema';


