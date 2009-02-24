#!/bin/bash

. /etc/profile.d/staff.sh

createdb -Upostgres staff
psql -Upostgres staff <${STAFF_HOME}/staff.sql && rm -f ${STAFF_HOME}/staff.sql
