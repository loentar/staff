## Update datasources to `r675` ##

How to update Datasources while updating staff from `revision 674` (and below) to `revision 675` (and above).

In [revision 675](https://code.google.com/p/staff/source/detail?r=675) parameters are now bound using DBMS prepared queries.

To fix your datasources you should remove apostrophes in bound parameters.

For example you should change that:
```
<execute>INSERT INTO "groups"("name", "description") VALUES('$(sName)', '$(sDescr)')</execute>
```

to this:

```
<execute>INSERT INTO "groups"("name", "description") VALUES($(sName), $(sDescr))</execute>
```


To make this automatically you could start that script from your datasources root (Don't forget to make a backup!!!):

```
find -type f -exec sed -i "s/'\(\$([^)]\+)\)'/\1/gm" {} \;
```