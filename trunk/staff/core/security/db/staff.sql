--
-- PostgreSQL database dump
--

SET SESSION AUTHORIZATION 'postgres';

--
-- TOC entry 3 (OID 2200)
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


SET search_path = public, pg_catalog;

--
-- TOC entry 16 (OID 249665)
-- Name: context; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE context (
    contextid integer DEFAULT nextval('context_sequence'::text) NOT NULL,
    userid integer
) WITHOUT OIDS;
ALTER TABLE context SET MAC TO '{0,0}';


--
-- TOC entry 4 (OID 249668)
-- Name: context_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE context_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 17 (OID 249670)
-- Name: users; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE users (
    userid integer DEFAULT nextval('users_sequence'::text) NOT NULL,
    username character varying,
    "password" character varying,
    description character varying
) WITHOUT OIDS;
ALTER TABLE users SET MAC TO '{0,0}';


--
-- TOC entry 6 (OID 249676)
-- Name: users_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE users_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 18 (OID 249678)
-- Name: session; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE "session" (
    sessionid integer DEFAULT nextval('session_sequence'::text) NOT NULL,
    sid character varying(32) DEFAULT md5((now())::text) NOT NULL,
    contextid integer NOT NULL,
    "time" timestamp without time zone DEFAULT now() NOT NULL,
    extraid integer DEFAULT 0 NOT NULL
) WITHOUT OIDS;
ALTER TABLE "session" SET MAC TO '{0,0}';


--
-- TOC entry 8 (OID 249683)
-- Name: objects_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE objects_sequence
    START WITH 11
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 19 (OID 249685)
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
ALTER TABLE objects SET MAC TO '{0,0}';


--
-- TOC entry 20 (OID 249695)
-- Name: groups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE groups (
    groupid integer DEFAULT nextval('groups_sequence'::text) NOT NULL,
    groupname character varying NOT NULL,
    description character varying
) WITHOUT OIDS;
ALTER TABLE groups SET MAC TO '{0,0}';


--
-- TOC entry 10 (OID 249701)
-- Name: groups_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE groups_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 21 (OID 249703)
-- Name: usertogroups; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE usertogroups (
    id integer DEFAULT nextval('usertogroups_sequence'::text) NOT NULL,
    userid integer NOT NULL,
    groupid integer NOT NULL
) WITHOUT OIDS;
ALTER TABLE usertogroups SET MAC TO '{0,0}';


--
-- TOC entry 12 (OID 249706)
-- Name: usertogroups_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE usertogroups_sequence
    INCREMENT BY 1
    NO MAXVALUE
    MINVALUE 0
    CACHE 1;


--
-- TOC entry 14 (OID 249708)
-- Name: session_sequence; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE session_sequence
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


--
-- TOC entry 22 (OID 249752)
-- Name: objecttypes; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE objecttypes (
    objecttypeid integer NOT NULL,
    typename character varying NOT NULL
) WITHOUT OIDS;
ALTER TABLE objecttypes SET MAC TO '{0,0}';


--
-- Data for TOC entry 35 (OID 249665)
-- Name: context; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY context (contextid, userid) FROM stdin;
5	2
6	9
0	0
1	1
13	11
\.


--
-- Data for TOC entry 36 (OID 249670)
-- Name: users; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY users (userid, username, "password", description) FROM stdin;
2	user2	234567	\N
9	dem	test	\N
1	guest	guest	anonymous user
0	root	root	administrator
11	test	test	test user
\.


--
-- Data for TOC entry 37 (OID 249678)
-- Name: session; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY "session" (sessionid, sid, contextid, "time", extraid) FROM stdin;
5	100000	6	2019-02-09 11:46:00.110414	0
1	00000000000000000000000000000000	1	9999-12-31 23:59:59.999999	0
8	10	5	2009-02-11 09:43:53.927396	0
9	5960ba7b645c50951210f2815556b67b	13	2009-02-27 16:58:42.860637	0
10	5d6010dd1848e047ab13561b749fedf1	13	2009-03-02 11:05:17.125827	1
11	7ac3c9a6d8c920be072179d037264170	13	2009-03-02 11:17:36.524184	2
12	1fc8093088302fae0487eb167ce8056f	13	2009-03-02 11:17:42.562901	3
\.


--
-- Data for TOC entry 38 (OID 249685)
-- Name: objects; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objects (objectid, name, "type", description, userid, groupid, parentobjectid, permission) FROM stdin;
0	root	0	root object	0	0	0	480
1	ROOTCOMPONENT	1	root component	0	0	0	511
3	CalcService	2	\N	9	0	2	456
4	SubService	2	\N	9	0	2	448
5	Add	3	\N	9	0	3	511
6	Sub	3	\N	9	0	3	448
7	staff	1	staff component	0	0	1	511
2	calc	1	\N	9	0	1	511
8	ROOTWIDGET	4	root widget	0	0	0	511
9	Button	4	Кнопка	0	0	8	511
10	Edit	4	Строка ввода	0	0	8	511
11	MapListView	4	Отображение карты	0	0	8	511
12	DummyUnknown	4	DummyUnknown\n	0	0	8	511
\.


--
-- Data for TOC entry 39 (OID 249695)
-- Name: groups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY groups (groupid, groupname, description) FROM stdin;
0	root	\N
1	guest	\N
2	group1	\N
\.


--
-- Data for TOC entry 40 (OID 249703)
-- Name: usertogroups; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY usertogroups (id, userid, groupid) FROM stdin;
0	0	0
1	1	1
2	2	2
4	9	0
5	9	1
6	1	0
\.


--
-- Data for TOC entry 41 (OID 249752)
-- Name: objecttypes; Type: TABLE DATA; Schema: public; Owner: postgres
--

COPY objecttypes (objecttypeid, typename) FROM stdin;
0	root
1	component
2	service
3	operation
4	widget
\.


--
-- TOC entry 25 (OID 249710)
-- Name: userid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users
    ADD CONSTRAINT userid PRIMARY KEY (userid);


--
-- TOC entry 24 (OID 249712)
-- Name: userid_unique; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT userid_unique UNIQUE (userid);


--
-- TOC entry 26 (OID 249714)
-- Name: username_unique; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY users
    ADD CONSTRAINT username_unique UNIQUE (username);


--
-- TOC entry 23 (OID 249716)
-- Name: contextid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT contextid PRIMARY KEY (contextid);


--
-- TOC entry 27 (OID 249718)
-- Name: sessionid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT sessionid PRIMARY KEY (sessionid);


--
-- TOC entry 29 (OID 249720)
-- Name: pk_object; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT pk_object PRIMARY KEY (objectid);


--
-- TOC entry 31 (OID 249722)
-- Name: pk_groupid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT pk_groupid PRIMARY KEY (groupid);


--
-- TOC entry 32 (OID 249724)
-- Name: uni_groupname; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY groups
    ADD CONSTRAINT uni_groupname UNIQUE (groupname);


--
-- TOC entry 33 (OID 249726)
-- Name: pk_id; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT pk_id PRIMARY KEY (id);


--
-- TOC entry 30 (OID 249728)
-- Name: uni_objectnametypeparentid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT uni_objectnametypeparentid UNIQUE (name, "type", parentobjectid);


--
-- TOC entry 34 (OID 249757)
-- Name: pk_objecttypeid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objecttypes
    ADD CONSTRAINT pk_objecttypeid PRIMARY KEY (objecttypeid);


--
-- TOC entry 28 (OID 257098)
-- Name: uni_extraid_contextid; Type: CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT uni_extraid_contextid UNIQUE (extraid, contextid);


--
-- TOC entry 42 (OID 249732)
-- Name: userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY context
    ADD CONSTRAINT userid FOREIGN KEY (userid) REFERENCES users(userid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 43 (OID 249736)
-- Name: contextid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY "session"
    ADD CONSTRAINT contextid FOREIGN KEY (contextid) REFERENCES context(contextid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 44 (OID 249740)
-- Name: fk_parent; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_parent FOREIGN KEY (parentobjectid) REFERENCES objects(objectid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 46 (OID 249744)
-- Name: fk_userid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT fk_userid FOREIGN KEY (userid) REFERENCES users(userid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 47 (OID 249748)
-- Name: fk_groupid; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY usertogroups
    ADD CONSTRAINT fk_groupid FOREIGN KEY (groupid) REFERENCES groups(groupid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 45 (OID 249759)
-- Name: fk_objecttype; Type: FK CONSTRAINT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY objects
    ADD CONSTRAINT fk_objecttype FOREIGN KEY ("type") REFERENCES objecttypes(objecttypeid) ON UPDATE RESTRICT ON DELETE RESTRICT;


--
-- TOC entry 5 (OID 249668)
-- Name: context_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('context_sequence', 13, true);


--
-- TOC entry 7 (OID 249676)
-- Name: users_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('users_sequence', 11, true);


--
-- TOC entry 9 (OID 249683)
-- Name: objects_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('objects_sequence', 11, false);


--
-- TOC entry 11 (OID 249701)
-- Name: groups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('groups_sequence', 1, true);


--
-- TOC entry 13 (OID 249706)
-- Name: usertogroups_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('usertogroups_sequence', 6, true);


--
-- TOC entry 15 (OID 249708)
-- Name: session_sequence; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('session_sequence', 12, true);


--
-- TOC entry 2 (OID 2200)
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: postgres
--

COMMENT ON SCHEMA public IS 'Standard public schema';


