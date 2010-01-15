
delete from "session" where "id" <> 0;
select setval('groups_sequence', (select case when (select max("id") from "groups") > 100 then (select max("id") from "groups") else 100 end));
select setval('objects_sequence', (select case when (select max("id") from "objects") > 100 then (select max("id") from "objects") else 100 end));
select setval('objecttypes_sequence', (select max("id") from "objecttypes"));
select setval('session_sequence', (select max("id") from "session"));
select setval('users_sequence', (select case when (select max("id") from "users") > 100 then (select max("id") from "users") else 100 end));
select setval('users_to_groups_sequence', (select max("id") from "users_to_groups"));

