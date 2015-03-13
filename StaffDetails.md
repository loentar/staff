<h1> Staff details </h1>

<h1>Table of Contents</h1>


## What is it? ##
Staff is Web Service Framework for C++ based on Apache Axis2/C. WSF Staff provide C++ interface to most used Apache Axis2/C functions, and also it provide many additional features.

## Main features ##

Main features provided by WSF Staff:
  * stateful [Web Services implementation](ExampleCalculatorService.md) in C++ with instance support;
  * [synchronous](ExampleCalculatorClient.md) and [asynchronous](ExampleCalculatorNonBlocking.md) client implementation in C++, [AJAX](ExampleCalculatorAjaxClient.md) and [J2ME](ExampleCalculatorJ2meClient.md);
  * service-component [architecture implementation](SoaApplicationStruct.md);
  * to interact with the service you can use: simple types (number, string, enum, ...), complex types (struct, typedef, list, ...) and raw xml (DataObject);
  * support for RESTful [web services](ExampleCalculatorRestfulService.md) and [clients](ExampleCalculatorRestfulClient.md);
  * [WSDL](ExampleCreatingCppClientFromWsdl.md) support with xsd import/export;
  * [J2ME client](ExampleCalculatorJ2meClient.md) support is based on ksoap;
  * [code generator](CodegenTemplates.md) to generate any sort of Web Services related source code (not only in C++);
  * access to relational databases via [DAS](ExampleDasDataSource.md) (data access service). _Supported DBMS are: Postgresql, MySQL, SQLite3._;
  * [traffic compression](TrafficCompression.md) support in J2ME and AJAX.
  * security support (users, groups, sessions, ACLs, etc.);

### Service interface ###

Service interface is a C++ header file that contain a class (service), which provide set of functions (service operations). Also interface can contain structures, typedefs and enums that will be used to interact with the service.

In the Service interface:
  * STL containers supported: list, vector, map;
  * Structure inheritance is supported;
  * Structures can be nested;
  * Structures may have struct, typedef and template fields;
  * Typedefs is supported;
  * Enums is supported;
  * C++ Namespaces are treated as component's name.

### WSDL ###
WSDL extensions supported by WSF Staff are:
  * Dynamic types (Abstract);
  * Nillable;
  * Optional;
  * SOAP Arrays;
  * hexBinary and base64Binary.


### Code generator (codegen) ###

For more information about codegen's templates see the CodegenTemplates wiki page.

Code generator can produce code following :
  * complete client;
  * complete service;
  * web service interface generation from WSDL;
  * WSDL generation from web service interface;
  * BPEL support: codegen supports WSDL generation compatible with BPEL;
  * Google Protocol Buffers's proto files can be generated from web service interface;
  * web service interface can be generated from Google Protocol Buffers's proto files;
  * internal files generation (Proxies, Wrappers, etc);

Most of new templates can be created without changing codegen's source code.

### Security ###
Security model is based on ACL and rules access users and groups to following security object:
  * components;
  * services;
  * services operations;
  * any other objects, that service can implement.
Security DB is on the SQLite 3.
<p /><p />

## Portability and Platform Support ##
Currently WSF Staff has been ported to following platforms:
  * Linux;
  * Microsoft Windows;
  * Mac OS X;
  * FreeBSD;
  * Android;
  * SunOS;
  * Solaris.

Tested on x86 and x86\_64 platforms under Ubuntu 9.10-11.10, Debian Lenny, Debian Squeeze, RHEL 5, Windows XP SP2, Windows Vista, Windows 7, Mac OS X 10.5.5, FreeBSD 8.1, SunOS 5.10.

Android version tested on: Galaxy Tab 10.1, Google Nexus S, Emulator.

Tested on ARMv5 and MIPS32-v1 platforms on Busybox v1.18.


Windows binaries can be built using MSVC or GCC (MinGW) compilers.

## License ##
WSF Staff is released with Apache License V2.0.