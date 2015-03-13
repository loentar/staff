## Overview ##
> This document contains information on the general structure of a model SOA-applications.

## Description ##

> This is a component diagram for distributed applications based on Staff:

> ![![](http://staff.googlecode.com/svn/wiki/img/component_diagram_en.png)](http://staff.googlecode.com/svn/wiki/img/component_diagram_en.png)

> Service of the application may be made either as a service, which is part of the component ("Component"), or service Axis2/C (not shown in the diagram).

> Client may be an application implemented in C++ using the client library Staff ("C++ Client") or an application implemented in JavaScript ("JavaScript Client") or any other application implementing the exchange to service part of the protocol through SOAP, in the format compatible with the Staff.

> The composition of C++ client include:
  * Axis2/C client: implements SOAP protocol over standard protocols, TCP, HTTP/HTTPS, etc.;
  * Proxy is designed to convert the parameters, the functions of the service in SOAP-request and the conversion of the received SOAP-response to result of the function.
  * Proxy Factory is implementing the creation of Proxy implementations for the given services.
  * Client implementation is treated with the service by calling functions of the Proxy. The implementation of the client realized by developer.

> JavaScript client contains the same components as the C++ client, with the exception of Proxy factory. Purpose and general principle of operation of the components of the client JavaScript is similar to C++ client.


> Core is designed to dispatch requests and manage services.
> Appointment of the components of the kernel:
  * Axis2/C - Application Server, provides a transport level and the base dispatching SOAP-requests;
  * Staff security module: checks access rights of authorized clients to components and operations services;
  * Staff dispatcher module: the filtering of requests to Staff services;
  * Staff service: performs the reception, the search service and redirect the request to that service;
  * Service dispatcher: implements a connection, storing information on services, management and search services;
  * Session manager: manages the sessions, frees sessions and bound services instances when session is expired;
  * Instance manager: manages the services instances;
  * Services wrapper factory: stores wrappers to all registered services in system.


> Component contains the implementation of one or more services.
> Component consists of:
  * Implementation of the component - the module responsible for the management of services within the component;
  * Service wrapper - a component that provides the conversion of SOAP-request to call the functions of the service and packaging result;
  * Service implementation contains a set of functions that defines the functionality of the service. The implementation of the service realized by developer.
