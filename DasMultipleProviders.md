## Declaring providers ##

When using multiple providers you should set identifier for each using `id` attribute.

Provider can be set for `script` or `execute` operators by declaring `providerid` attribute. If `providerid` is not declared, will be used default provider.

Example with multiple providers:
```
<?xml version="1.0" encoding="utf-8" ?>
<datasources>
  <datasource namespace="staff.das.samples" name="Groups" 
      descr="Data source example with multiple providers">

    <!-- Declare first provider. It will be used by default -->
    <provider name="staff.das.Sqlite" id="sqlProvider">
      <connection>
        <db>/some/path/to/database.db</db>
      </connection>
    </provider>

    <!-- Declare second provider -->
    <provider name="staff.das.Services" id="servicesProvider">
      <connection>
        <service>staff.samples.das.services.SomeDataService</service>
      </connection>
    </provider>

    <types>
      <Fields type="struct">
        <col1 type="int"/>
        <col2 type="string"/>
      </Fields>
      <FieldsList type="list" itemtype="Fields" />
    </types>

    <operations>
      <!-- this is a simple operation that is easy to implement using DAS scripting -->
      <operation name="SimpleOperation">
        <params/>
        <return type="FieldsList" />
        <!-- use default sqlProvider -->
        <execute>SELECT "field1", "field1" FROM "someTable"</execute>
      </operation>


      <!-- this is a complex operation that is hard to implement using DAS scripting.
           so we just call SomeDataService -->
      <operation name="SomeComplexOperation">
        <params>
          <param1 type="int" />
          <param2 type="DataObject" />
        </params>
        <!-- you can return xml or some structured data -->
        <return type="DataObject" />
        <!-- use servicesProvider: invoke SomeDataService with SomeComplexLogic(p1, p2) operation -->
        <execute providerid="servicesProvider">SomeComplexLogic(p1=$(param1), p2=$[param2])</execute>
      </operation>

    </operations>

  </datasource>
</datasources>
```

