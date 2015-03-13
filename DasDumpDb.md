## Codegen Plugin ##

Codegen plugin "dasdumpdb" designed to dump data model from a database.

Plugin can work with [Sql providers](DasProviders.md).

To dump model you should pass database connection parameters. Parameters list is specific for each DBMS.

Common options are:
  * provider - provider's qualified name;
  * rootns - namespace (component name) for generated datasource;

## Generating Datasource ##

Using "datasources" template you can produce datasources file by DB model.

Datasource generation options:
  * singleservice - generate datasources as single service (in other case will be generated service per each table);
  * fullopns - use qualified names for services operations.

### Command lines examples ###

Command lines examples to generate datasources by database model.

**sqlite:**
```
staff_codegen -pdasdumpdb -tdatasources -drootns=das.samples.test,provider=staff.das.Sqlite -i/usr/local/staff/db testdas.db
```

**postgres:**
```
staff_codegen -pdasdumpdb -tdatasources -dprovider=staff.das.Postgres,login=dastestuser,password=dastestuserpassword,host=localhost,port=5432,rootns=das.samples.test testdas
```

**mysql:**
```
staff_codegen -pdasdumpdb -tdatasources -dprovider=staff.das.MySql,login=dastestuser,password=dastestuserpassword,host=localhost,port=3306,rootns=das.samples.test testdas
```


## Generating Datatypes ##

Using "datatypes" template you can produce datatypes file by DB model.
