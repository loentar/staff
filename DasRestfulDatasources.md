# RESTful datasources #

Datasources can act as REST services.

To enable REST support for datasource you should declare `options` section in each operation you need to be RESTful.

Short example (part of Users.datasources example):

```
  <operation name="GetUser" descr="get user info by user id">
    <options>
      <restMethod>GET</restMethod>
      <restLocation>GetUser/id={nId}</restLocation>
    </options>
    <params>
      <nId type="int" />
    </params>
    <return type="User" />
    <execute>SELECT "id", "name", "description" FROM "users" WHERE "id" = $(nId)</execute>
  </operation>
```


You can access this operation using that URL: http://localhost:9090/axis2/services/staff.das.samples/Users/GetUser/id=1

See also ExampleCalculatorRestfulService.