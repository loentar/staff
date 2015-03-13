Use Calculator client from ExampleCalculatorClient.

Also, service must be modified to enable REST: ExampleCalculatorRestfulService.

## 1. Enabling REST support in client ##
To enable REST support for client you need to write three special codegen metacomments:

```
// *restEnable: <REST_ENABLE>
// *restMethod: <REST_METHOD>
// *restLocation: <REST_LOCATION>
```

Where:
  * `<REST_ENABLE>` is a flag to enable or disable using of REST to call given operation (`true` or `false`);
  * `<REST_METHOD>` is either of GET, PUT, POST, DELETE as declared in the service;
  * `<REST_LOCATION>` is a part of URL to access service operation. It should be same as in the service.

## 2. Modifying Calculator client ##
Open Calculator/client/src/Calculator.h and add these lines before Add operation:
```
// *restEnable: true
// *restMethod: GET
// *restLocation: Add/a={a},b={b}
```

Clean and compile client.

## 3. Starting client ##

Start client as in ExampleCalculatorClient. You should get similar results.

## 4. Dynamic modifying REST client options ##

To learn how to modify REST client options dynamically, please look to:

http://code.google.com/p/staff/source/browse/#svn/trunk/staff/samples/echo/client_rest/src

## 5. Download source code ##

[Download](http://staff.googlecode.com/files/samples.calculator_client_rest.7z) source code of the RESTful Calculator service.
