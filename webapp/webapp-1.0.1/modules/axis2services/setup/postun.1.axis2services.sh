#!/bin/bash

modname="axis2services"
httpdconf=/www/conf/httpd.conf

grep $modname $httpdconf -v >$httpdconf.tmp && mv -f $httpdconf.tmp $httpdconf

