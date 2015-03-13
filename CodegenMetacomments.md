# Codegen metacomments #

_Notice: names and values of codegen metacomments are case sensitive_

## Interface: ##
| name | description | allowed values | service | client | example |
|:-----|:------------|:---------------|:--------|:-------|:--------|
| targetNamespace | set target namespace for interface | _any uri_ |  | x | // `*`targetNamespace: http://ws.apache.org/axis2/services/echo |

## Service: ##
| name | description | allowed values | service | client | example |
|:-----|:------------|:---------------|:--------|:-------|:--------|
| ! | set service short and detail description (doxygen style) | _any_ | x |  | //! Calculator service<br />/`*`! detailed description of the service `*`/ |
| loadAtStartup | create default service instance on startup | true<br />false | x |  | // `*`loadAtStartup: true |
| engageModule | engage axis2/c module | _Axis2/C module name_ |  | x | // `*`engageModule: addressing |
| serviceUri | set service location | _any uri_ |  | x | // `*`serviceUri: http://localhost:9090/axis2/services/echo |
| targetNamespace | set target namespace for class | _any uri_ |  | x | // `*`targetNamespace: http://ws.apache.org/axis2/services/echo |
| targetNamespacePrefix | set target namespace prefix | _prefix name_ |  | x | // `*`targetNamespacePrefix: ns1 |
| soapVersion | set soap version to use | 1.1<br />1.2 |  | x | // `*`soapVersion: 1.1 |
| soapVersionUri | set soap version uri to use | _soap version uri_ |  | x | // `*`soapVersionUri: http://schemas.xmlsoap.org/wsdl/soap12/ |

## Service operation: ##
| name | description | allowed values | service | client | example |
|:-----|:------------|:---------------|:--------|:-------|:--------|
| ! | set service operation description (doxygen style) | _any_ | x |  | //! Add operation<br />/`*`! detailed description of the operation `*`/ |
| soapAction | soap action for operation | _any_ |  | x | // `*`soapAction: http://ws.apache.org/axis2/c/samples/echo/soap_action |
| requestElement | set request element for soap operation (soap operation name) | _valid xml element name_ | x | x | // `*`requestElement: echoString |
| responseElement | set response element for soap operation | _valid xml element name_ | x | x | // `*`responseElement: echoString |
| resultElement | set result element for soap operation (response subnode) | _valid xml element name_ | x | x | // `*`resultElement: text |
| restEnable | enable or disable rest call style for client | true<br />false |  | x | // `*`restEnable: true |
| restMethod | set HTTP method for RESTful services | _any HTTP REST method, e.g.:_<br /> PUT, POST, GET, DELETE, etc. | x | x | // `*`restMethod: POST |
| restLocation | sets operation's HTTP location for RESTful services | _RESTful operation uri_ | x |  | // `*`restLocation: echo/echoString |
| wsaAction | set WS-Addressing "Action" value | _any_ |  | x | // `*`wsaAction: http://ws.apache.org/axis2/c/samples/echoString |
| wsaTo | set WS-Addressing "To" value | _service uri_ |  | x | // `*`wsaTo: http://localhost:9090/axis2/services/echo |
| wsaReplyTo | set WS-Addressing "ReplyTo" value | _reply to service uri_ |  | x | // `*`wsaReplyTo: http://localhost:6060/axis2/services/__ANONYMOUS_SERVICE__/__OPERATION_OUT_IN__ |
| wsaFrom | set WS-Addressing "From" value | _from service uri_ |  | x | // `*`wsaFrom: http://localhost:6060/axis2/services/echo |
| wsaFaultTo | set WS-Addressing "FaultTo" value | _fault to service uri_ |  | x | // `*`wsaFaultTo: http://localhost:6060/axis2/services/echo/fault |
| wsaUseSeparateListener | use a separate listener for receive channel | true<br />false |  | x | // `*`wsaUseSeparateListener: true |
| timeout | sets the wait time after which a client times out in a blocking scenario | _any integer_ |  | x | // `*`timeout: 30000 |
| mep | sets message exchange pattern for this operation | in-out<br />robust out-only<br />in-only |  | x | // `*`mep: in-out |

## Structure: ##
| name | description | allowed values | applied to | example |
|:-----|:------------|:---------------|:-----------|:--------|
| abstract | mark given structure as abstract for serialization.<br />_Please note, you should declare virtual destructor for this structure._ | true<br />false | client, serivce | // `*`abstract: true |
| choiceArrayItem | mark given structure as item of choice array | true<br />false | client, serivce | // `*`choiceArrayItem: true |

## Structure member: ##
| name | description | allowed values | applied to | example |
|:-----|:------------|:---------------|:-----------|:--------|
| protobufTag | sets protobuf's tag value | _any integer_ | protobuf | // `*`protobufTag: 10 |
| defaultValue | sets default value | _any_ | cpp, wsdl, protobuf | // `*`defaultValue: true |
| isAttribute | serialize/deserialize struct member as attribute (wrapped style) | true<br />false | client, serivce | // `*`isAttribute: true |
| attributeGroupName | specify name for attribute group | ID | client, serivce | // `*`attributeGroupName: myAttrGroup |
| useParentElement | serialize/deserialize struct member to parent element | true<br />false | client, serivce | // `*`useParentElement: true |
| choiceId | set structure member as of wsdl choice type, set id for this choice group | _non negative integer_ | client, serivce | // `*`choiceId: 0 |

## Enum: ##
| name | description | allowed values | applied to | example |
|:-----|:------------|:---------------|:-----------|:--------|
| baseType | specify base type for enum serialization | wsdl type | client, serivce | // `*`baseType: string |

## Typedef: ##
| name | description | allowed values | applied to | example |
|:-----|:------------|:---------------|:-----------|:--------|
| elementName | specify element name for each item of array | ID | client, serivce | // `*`elementName: item |

## Example ##
This is a [echo interface example](http://code.google.com/p/staff/source/browse/trunk/staff/samples/echo/client_blocking_dual/src/Echo.h) to call Axis2/C service "echo" with separate listener in blocking mode.
```
#ifndef _ECHO_H_
#define _ECHO_H_

#include <string>
#include <staff/common/IService.h>

  //! echo service
  // *engageModule: addressing
  // *targetNamespace: http://ws.apache.org/axis2/services/echo
  // *targetNamespacePrefix: ns1
  class Echo: public staff::IService
  {
  public:
    //! echo string
    /*! \param text - input string
        \return resulting string
        */
    // *wsaAction: http://ws.apache.org/axis2/c/samples/echoString
    // *wsaTo: http://localhost:9090/axis2/services/echo
    // *wsaReplyTo: http://localhost:6060/axis2/services/__ANONYMOUS_SERVICE__/__OPERATION_OUT_IN__
    // *wsaUseSeparateListener: true
    // *requestElement: echoString
    // *responseElement: echoString
    // *resultElement: text
    virtual std::string EchoString(const std::string& text) = 0;
  };

#endif // _ECHO_H_
```

## More examples ##
More examples are located in `staff/samples` directory of source code tarball.

Or You can [browse samples online](http://code.google.com/p/staff/source/browse/#svn/trunk/staff/samples).
