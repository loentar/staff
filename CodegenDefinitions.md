# Codegen definitions #

_Notice: names and values of codegen definitions are case sensitive_

Definitions is used to pass additional parameters to codegenerator.

Generic definition:

`-d<var1>[[=<value1>],<var2>[=<value2>]...]`

If value is ommited string 'true' will be passed as value.

I.e.: `-dnonblocking` is equal to `-dnonblocking=true`.

### Definitions list: ###
| **Name**           | **Value**                 | **Description**                              |
|:-------------------|:--------------------------|:---------------------------------------------|
| serviceuri       | URL to service          | while generating wsdl file, sets service location uri to given |
| c++11            | true, false             | enable c++11 support: generate std::unique\_ptr instead of std::auto\_ptr,<br> add -std=gnu++0x option into Makefiles and .pro files <br>
<tr><td> svc              </td><td> [namespace.]ServiceName </td><td> while generating template of interface file, sets service name and component name </td></tr>
<tr><td> dontunwrap       </td><td> true, false             </td><td> disable request/response unwrapping while generating from wsdl </td></tr>
<tr><td> protobufRequestTimeout </td><td> int value         </td><td> sets timeout for requests while generating protobuf connector template </td></tr>
<tr><td> rootns           </td><td> some.namespace          </td><td> prepend given namespace toexisting namespace to prevent clashing </td></tr>
<tr><td> protobufEncoding </td><td> encoding name           </td><td> sets encoding of strings of protobuf messages </td></tr>
<tr><td> tns              </td><td> URI                     </td><td> sets target namespace while generating WSDL </td></tr>
<tr><td> bpel             </td><td> true, false             </td><td> generate additional namespaces while generating WSDL to be compatible with BPEL </td></tr>
<tr><td> nonblocking      </td><td> true, false             </td><td> generate asynchronous client instead of synchronous while generating C++ client </td></tr>
<tr><td> noimpl           </td><td> true, false             </td><td> don't generate client implementation example (main.cpp) </td></tr></tbody></table>

<br />

See also:<br>
<ul><li><a href='CodegenPlugins.md'>codegen plugins list</a>
</li><li><a href='CodegenMetacomments.md'>codegen metacomments description</a>
</li><li><a href='CodegenTemplates.md'>codegen templates list</a>
</li><li><a href='CodegenCommandLine.md'>codegen command line description</a>