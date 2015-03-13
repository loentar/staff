## Code generator plugins list ##

Codegen plugins is designed to read the source files in various formats.

| **plugin name**  | **description** | **example** |
|:-----------------|:----------------|:------------|
| cpp            | default plugin to parse service's interface <br />(C++ header file with datatypes and service description) | `staff_codegen -tserviceimpl Service.h`<br /><font color='darkgreen' size='1'>generate service implementation skeleton</font>|
| wsdl           | plugin to parse wsdl files | `staff_codegen -pwsdl -tinterface Service.wsdl`<br />`staff_codegen -pwsdl -tinterface http://example.com/path/to/Service.wsdl`<br /><font color='darkgreen' size='1'>create service interface from WSDL</font> |
| das            | plugin to get and parse DAS's datasources via DataAccess service | `staff_codegen -pdas -tinterface staff.das.samples.Users`<br /><font color='darkgreen' size='1'>create service interface from datasource, retrieved from DataAccessService</font> |
| dasdatasource  | plugin to parse DAS's datasources from file | `staff_codegen -pdasdatasource -tinterface Users.datasource`<br /><font color='darkgreen' size='1'>create service interface from datasource file</font> |
| protobuf       | plugin to parse Google Protocol Buffers proto files | `staff_codegen -pprotobuf -tinterface Service.proto`<br /><font color='darkgreen' size='1'>create service interface from proto file</font> |

See also:
  * [codegen templates](CodegenTemplates.md)
  * [codegen command line switches](CodegenCommandLine.md)
  * [codegen metacomments description](CodegenMetacomments.md)
  * [codegen definitions](CodegenDefinitions.md)
