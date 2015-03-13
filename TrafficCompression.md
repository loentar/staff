# Introduction #

Traffic compression on the client side is realized by a modified version of the library jazzlib (ported from j2se to j2me)
and modified version of kSOAP2 library (added compression/decompression routines).

Server-side traffic compression is realized by apache2 and mod\_deflate.


# Configure traffic compression #

## Configure Apache2 ##

Apache2 and module "deflate" should be installed and configured.

To enable server-side traffic compression please edit `axis2services.conf`, located in apache2 `mods-enabled` directory
(usualy `/etc/apache2/mods-enabled/axis2services.conf`) as follows:

```
<IfModule mod_axis2services.c>

  <Location /axis2/services>
    SetHandler axis2services
    SetOutputFilter DEFLATE
    SetInputFilter DEFLATE
  </Location>

</IfModule>
```

## Traffic compression within j2me client ##

Traffic compression in modified version kSOAP2 is enabled by default.

To disable traffic compression for newly created clients set:
```
org.ksoap2.transport.HttpTransport.gzipByDefault = false
```



# Download #

Download ported version of the jazzlib: http://code.google.com/p/staff/downloads/list?q=jazzlib

Download modified version of the kSOAP2: http://code.google.com/p/staff/downloads/list?q=ksoap2