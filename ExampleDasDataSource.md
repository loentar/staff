## Example: Creating datasource for DAS using sqlite3 provider ##

## 1. Create database ##

Create the sqlite3 database using some gui tool (like SQLite Manager add-on for firefox) or with SQL script and sqlite3 command line tool.

SQL script:
```
  DROP TABLE IF EXISTS accounts;

  CREATE TABLE accounts
  (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    login TEXT NOT NULL UNIQUE,
    email TEXT NOT NULL UNIQUE,
    description TEXT DEFAULT ''
  );

  INSERT INTO accounts(id, login, email, description) VALUES (0, 'admin', 'admin@localhost', 'Administrator');
  INSERT INTO accounts(id, login, email, description) VALUES (1, 'user', 'user@mysite.com', 'User');

```

Copy and paste this script into accounts.sql file.

Create sqlite3 database from script:
```
  sqlite3 accounts.db <accounts.sql
```

## 2. Create datasource file ##

Create directory `$(STAFF_HOME)/components/staff.das/datasources/staff.das.samples`, copy and paste datasource file into this dir.
_datasource file Accounts.datasources:_
```
<?xml version="1.0" encoding="utf-8" ?>
<datasources>
  <datasource namespace="staff.das.samples" name="Accounts" descr="Data source for manipulating with accounts">
    <provider name="staff.das.Sqlite">
      <connection>
        <db>FULL_PATH_TO_DB/accounts.db</db>
      </connection>
    </provider>

   <!-- types declarations -->
    <types>
      <Account type="struct" descr="account information">
        <nId type="int" descr="account id"/>
        <sLogin type="string" descr="login"/>
        <sEMail type="string" descr="e-mail"/>
        <sDescr type="string" descr="description"/>
      </Account>
      <AccountsList type="list" itemtype="Account" descr="Accounts list" />
    </types>

    <!-- data service operation declaration -->
    <operations>

      <operation name="Add" descr="register new account, returns added account id">
        <params>
          <sLogin type="string"/>
          <sEMail type="string"/>
          <sDescr type="string"/>
        </params>
        <return type="int" />
        <script>
          <execute>INSERT INTO "accounts"("login", "email", "description") VALUES('$(sLogin)', '$(sEMail)', '$(sDescr)')</execute>
          <execute>SELECT last_insert_rowid()</execute>
        </script>
      </operation>

      <operation name="Remove" descr="remove account by id">
        <params>
          <nId type="int" />
        </params>
        <script>
          <execute>DELETE FROM "accounts" WHERE "id" = $(nId)</execute>
        </script>
      </operation>

      <operation name="RemoveByLogin" descr="remove account by login">
        <params>
          <sLogin type="string" />
        </params>
        <script>
          <execute>DELETE FROM "accounts" WHERE "login" = '$(sLogin)'</execute>
        </script>
      </operation>

      <operation name="Get" descr="get account by id">
        <params>
          <nId type="int" />
        </params>
        <return type="Account" />
        <execute>SELECT "id", "login", "email", "description" FROM "accounts" WHERE "id" = $(nId)</execute>
      </operation>

      <operation name="GetAll">
        <params/>
        <return type="AccountsList" />
        <execute>SELECT "id", "login", "email", "description" FROM "accounts"</execute>
      </operation>

    </operations>

  </datasource>
</datasources>
```

Edit datasource file, and set full path to account.db file in the tag:
```
   <db>FULL_PATH_TO_DB/accounts.db</db>
```

for example:
```
   <db>/home/user/db/accounts.db</db>
```

or:
```
   <db>c:\db\accounts.db</db>
```

_Optionaly_ You can generate wsdl for this datasource:
From directory `STAFF_HOME/components/staff.das/datasources/staff.das.samples`:
```
  mkdir "../../../staff.das.samples"
  staff_codegen -pdasdatasource -twsdl Accounts.datasources -o../../../staff.das.samples
```

## 3. Start ##

Start Axis2/C server and open this url in the browser:
http://localhost:9090/axis2/services

New datasource service must be shown as:
|<font face='courier'><h3><u>staff.das.samples.Accounts</u></h3><p>Data source for manipulating with accounts <sup><a><code>[</code><a href='http://localhost:9090/axis2/services/staff.das.samples.Accounts?wsdl'>wsdl</a><code>]</code></a></sup></p><i>Available Operations</i> <ul><li>RemoveByLogin</li><li>Get</li><li>Add</li><li>GetAll</li><li>Remove</li></ul></font>|
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

## 4. Create das client ##

Follow [this](ExampleDasClient.md) guide to create client for datasource.

## 5. Download ##

Download datasource, database, client source code [here](http://code.google.com/p/staff/downloads/detail?name=samples.das.Accounts.7z).

## 6. Datasource that uses multiple providers ##

Datasource that uses multiple providers is [here](DasMultipleProviders.md).