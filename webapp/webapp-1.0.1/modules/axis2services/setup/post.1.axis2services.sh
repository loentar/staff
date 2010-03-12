#!/bin/bash

modname="axis2services"
httpdconf=/www/conf/httpd.conf

grep ${modname} ${httpdconf} -q || cat >> ${httpdconf} <<EOF
LoadModule ${modname}_module modules/mod_${modname}.so
<Location /axis2/services> # ${modname}
  SetHandler ${modname}
</Location> # ${modname}
EOF

