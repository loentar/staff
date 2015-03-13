<h1>Table of Contents</h1>


## Introduction ##
Bridge service translates requests from SOAP client to protobuf service.

## Dependencies ##
Must have the following dependencies installed and configured:
  * Axis2/C;
  * WSF Staff;
  * Google protocol buffers;
  * RPC implementation for protocol buffers: [see the list](http://code.google.com/p/protobuf/wiki/ThirdPartyAddOns#RPC_Implementations).

## Creating the Protobuf service ##
If you already have Protobuf service, please go to the section [Creating the bridge service](ExampleProtobufBridge#Creating_the_bridge_service.md).

### Creating the proto-file ###
Create the ProtoCalc.proto file, that describes messages and service.
```
package samples;

message Request
{
  required int32 a = 1;
  required int32 b = 2;
};

message Response
{
  required int32 result = 1;
};


service ProtoCalc
{
  rpc add(Request) returns (Response);
};
```

Put this file into directory "ProtoCalc/protobuf\_service/src".

### Generating the Protobuf service code ###
Generate the protobuf wrapper. Start protoc in "ProtoCalc/protobuf\_service/src" directory:
```
protoc ProtoCalc.proto --cpp_out=.
```

These files will be generated:
```
ProtoCalc.pb.cc  ProtoCalc.pb.h
```

### Implementing the Protobuf service ###
Create the "ProtoCalcImpl.h" file and put it into the same directory:
```
#ifndef _PROTOCALCIMPL_H_
#define _PROTOCALCIMPL_H_

#include "ProtoCalc.pb.h"

namespace samples
{
  class ProtoCalcImpl: public ProtoCalc
  {
  public:
    virtual void Test(::google::protobuf::RpcController* controller,
                       const Request* request,
                       Response* response,
                       ::google::protobuf::Closure* done);

  };
}

#endif
```

Create the "ProtoCalcImpl.cpp" file and put it into the same directory:
```
#include <google/protobuf/service.h>
#include "ProtoCalcImpl.h"

namespace samples
{
  void TestServiceImpl::Test(::google::protobuf::RpcController* controller,
                       const Request* request,
                       Response* response,
                       ::google::protobuf::Closure* done)
  {
    response->set_result(request->a() + request->b());
    done->Run();
  }
}
```

Please implement the RPC transport for this Protobuf service, compile and install the service, according to instructions for the transport.

## Creating the bridge service ##

### Generating the code ###
Create directory "ProtoCalc/staff\_bridge/src" and copy proto file there.

Generate the protobuf wrapper. Start protoc in "ProtoCalc/staff\_bridge/src" directory:
```
protoc ProtoCalc.proto --cpp_out=.
```

Generate the staff interface, service implementation, wrapper and component:
```
# generating the interface
staff_codegen -tinterface -pprotobuf -drootns=<ROOTNS> ProtoCalc.proto
# generating the bridge service implementation
staff_codegen -tprotobufserviceimpl -dusedns=<ROOTNS>,componentns=<COMPONENTNS> ProtoCalc.h
# generating the component and wrappers
staff_codegen -tcomponent ProtoCalc.h -dcomponentns=<COMPONENTNS>
```
Where:
  * `<ROOTNS>` - staff component prefix (for example: "protobuf").<br /> This prefix is needed to prevent staff and protobuf namespaces intersection;<br /><br />
  * `<COMPONENTNS>` - _optional_. Component name to include the service (for example: "protobuf"). This namespace is used to generate protobuf-connector namespace.<br /> This option can be useful when you plan to use one connector class for many services within subcomponents.<br /> The same value for `<COMPONENTNS>` and `<ROOTNS>` is recommended.

### Creating the connector ###
Connector is a RPC transport implementation for bridge service.

Generate the connector implementation template:
```
staff_codegen -tprotobufconnector -dcomponentns=<COMPONENTNS> -e
```

these file will be generated:
```
ProtobufConnector.h    ProtobufConnector.cpp
```

Implement connector's functions depending on the RPC implementation you are using.

Compile and install component as descried in [example](http://code.google.com/p/staff/wiki/ExampleCalculatorService#3._Compiling_and_installing_service).

