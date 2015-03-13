Use the Calculator service from ExampleCalculatorService.

## 1. Enabling REST support in service ##

To enable REST support for any service you just need to write two special codegen metacomments:

```
// *restMethod: <REST_METHOD>
// *restLocation: <REST_LOCATION>
```

Where:
  * `<REST_METHOD>` is either of GET, PUT, POST or DELETE Http methods;
  * `<REST_LOCATION>` is the part of URL to access to service's operation. If Method is GET, location can contain argument names.

These comments should be added for each service operation, for which you want to include REST support.

## 2. Modifying Calculator service ##

Open samples/calculator/src/Calculator.h and add these lines before Add operation:
```
// *restMethod: GET
// *restLocation: Add
```

Add operation now should look like:
```
...
    // *restMethod: GET
    // *restLocation: Add/a={a},b={b}
    virtual int Add(int a, int b) = 0;

...
```

Clean, compile and install modified service.

Restart Axis2/c.

## 3. Testing RESTful web service using web browser ##

Open URL to invoke RESTful web service samples.Calculator:

http://localhost:9090/axis2/services/samples.Calculator/Add/a=1,b=2

Service execution results must be shown.

## 4. Modify C++ client to enable REST ##

To learn how to modify Calculator client to enable REST, please read ExampleCalculatorRestfulClient

## 5. Download source code ##

[Download](http://staff.googlecode.com/files/samples.calculator_component_rest.7z) source code of the RESTful Calculator service.