## Code generator parameters description ##

Codegen's command line is:
<pre>
staff_codegen[ <source files>][ -t<template>][ -p<plugin_name>][ -i<inputdir>][ -i<outputdir>][ -c<chagedir>][ -u][ -e][ -n<prj_name>][ -x]<br>
</pre>

`*` Parameters, quoted with `[]` is an optional.

### Detailed parametes description: ###
| **parameter**           | **description** | **example** |
|:------------------------|:----------------|:------------|
|  `-t<template>`       | Use given template to generate code | `staff_codegen -tserviceimpl Service.h`<br /><font color='darkgreen' size='1'>generate service implementation skeleton</font>|
|  `-p<plugin_name>`    | Use parser `<`plugin\_name`>` to read source code (default: cpp) | `staff_codegen -pwsdl -tinterface Service.wsdl`<br /><font color='darkgreen' size='1'>create service interface from WSDL</font> |
|  `-i<inputdir>`       | Set input directory | `staff_codegen -iwsdl_files -tinterface -pwsdl Service1.wsdl Service2.wsdl` <br /><font color='darkgreen' size='1'>generate interface files, from WSDL files located in <code>wsdl_files</code> into current directory</font> |
|  `-o<outputdir>`      | Set output dir | `staff_codegen -o../wsdl_files -twsdl Service1.h Service2.h` <br /><font color='darkgreen' size='1'>generate WSDL files into directory <code>../wsdl_files</code> from interface files located in current directory</font> |
|  `-c<changedir>`      | Set input and output dirs, equivalent to -i`<`changedir`>` -o`<`changedir`>`  | `staff_codegen -csrc -tcomponent Service1.h` <br /><font color='darkgreen' size='1'>generate component and service wrapper from directory src into the same directory</font> |
|  `-u`                 | Update: generate only missing files, update existing files by date | `staff_codegen -u -tclient Service.h`<br /><font color='darkgreen' size='1'>generate client proxy if proxy files is missing or older than service interface</font> |
|  `-e`                 | Supress warning if Interface file does not contain service | `staff_codegen -e -twsdl Types.h`<br /><font color='darkgreen' size='1'>generate xsd-schema for header file without service declaration</font> |
|  `-l[t|p]`            | Display parsers(p) and/or templates(t) lists | `staff_codegen -lt `<br /><font color='darkgreen' size='1'>display the templates list</font> |
|  `-d<var>[=<value>]`  | Define variables: -dvar1=value1,var2=2,var3 | `staff_codegen -twsdl -dserviceuri=http://services.org/MyService1`<br /><font color='darkgreen' size='1'>generate wsdl file, set service location uri to given</font> |
|  `-n<prj_name>`       | Set project name. | `staff_codegen -x -ntest Service1.h` <br /><font color='darkgreen' size='1'>generate internal xml with given name</font>  |
|  `-x`                 | Write internal xml description.<br /> XML file name is in format: "`<`project\_name`>`.xml" | _Please see example above_ |

See also:
  * [codegen plugins list](CodegenPlugins.md)
  * [codegen metacomments description](CodegenMetacomments.md)
  * [codegen templates list](CodegenTemplates.md)
  * [codegen definitions](CodegenDefinitions.md)
