## DAS Providers ##

DAS Provider is a module to get access to specified data.

## Provider types ##

There is two types of providers: Sql and Raw:
  * Sql providers designed to get access to data on DBMS;
  * Raw providers designed to get access to any other data storage.


## Providers list ##

List of Sql DAS Providers:

| **Qualified Name**   | **Description**                                 | **Datasource Example** |
|:---------------------|:------------------------------------------------|:-----------------------|
| staff.das.Sqlite   | Provider to get access to Sqlite3 DBMS data   | [link ](http://staff.googlecode.com/svn/trunk/das/samples/Users/datasources/sqlite/datasources/Users.datasources) |
| staff.das.MySql   | Provider to get access to MySQL DBMS data     | [link](http://staff.googlecode.com/svn/trunk/das/samples/Users/datasources/mysql/datasources/Users.datasources) |
| staff.das.Postgres | Provider to get access to Postgres DBMS data  | [link](http://staff.googlecode.com/svn/trunk/das/samples/Users/datasources/postgres/datasources/Users.datasources) |


List of Raw DAS Providers:

| **Qualified Name**   | **Description**                                                  | **Datasource Example** |
|:---------------------|:-----------------------------------------------------------------|:-----------------------|
| staff.das.Services | Provider to invoke service operations with specified arguments | [link](http://staff.googlecode.com/svn/trunk/das/samples/Users/datasources/services/datasources/Users.datasources) |
| staff.das.Shell    | Provider to call operating system shell commands and scripts   | [link](http://staff.googlecode.com/svn/trunk/das/samples/SysInfo/datasources/datasources/SysInfo.datasources) |


## Multiple providers ##

See [multiple providers wiki page](DasMultipleProviders.md).
