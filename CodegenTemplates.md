## Code generator templates list ##

Codegen templates is designed to write resulting files in various formats.

| **template name**  | **description** |
|:-------------------|:----------------|
| client           | template to generate client proxy |
| client\_all       | template to generate complete client: service interface(s), client stub, project files to compile client (Makefile, qmake project, VS project) |
| clientimpl       | template to generate client stub |
| component        | template to generate service wrapper and component wrapper |
| component\_all    | template to generate complete component with services: service interface(s), service skeleton, project files to compile component (Makefile, VS project) |
| interface        | template to generate service interface: for using with plugins |
| interfacegen     | template to generate service interface template from scratch. Example:<br />`staff_codegen -tinterfacegen -dsvc=samples.Calculator` |
| j2meclient       | template to generate j2me client proxy |
| jsclient         | template to generate JavaScript/AJAX client proxy |
| protobuf         | template to generate Google Protocol Buffers proto file |
| serviceimpl      | template to generate service implementation skeleton |
| wsdl             | template to generate wsdl file |

See also:
  * [codegen plugins list](CodegenPlugins.md)
  * [codegen command line switches](CodegenCommandLine.md)
  * [codegen metacomments description](CodegenMetacomments.md)
  * [codegen definitions](CodegenDefinitions.md)
