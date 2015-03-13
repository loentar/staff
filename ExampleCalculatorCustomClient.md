<h1>Table of Contents</h1>


## 1. Creating custom client ##

Create a directory for Calculator client source code and name it `calculator_custom_client`.

Enter that directory and start codegen to generate client stub and project files:

Start codegen:
```
staff_codegen -tclient_all -ncalculator_custom_client
```


Will be generated set of files as follows:
| **File name**                         | **Description** |
|:--------------------------------------|:----------------|
| `Makefile`                          | Makefile for GNU Make utility |
| `calculator_custom_client.pro`      | Qt project file (for qmake and Qt-Creator IDE) |
| `calculator_custom_client.vcproj`   | Visual Studio project file |
| `codegen.cmd`                       | Codegeneration script for VS |
| `src/main.cpp`                      | Calculator client implementation |


## 2. Implementing client ##

Open generated project in your preferred IDE.

Open `src/main.cpp` and replace it's content to:
```
#include <staff/utils/Log.h>
#include <staff/common/DataObject.h>
#include <staff/common/Exception.h>
#include <staff/client/ServiceClient.h>

int main(int /*nArgs*/, const char* /*paszArgs*/[])
{
  try
  {
    staff::ServiceClient tClient;

    // initialize client
    tClient.Init("http://localhost:9090/axis2/services/samples.calc.CalcService");

    // build payload for Add operaion
    staff::DataObject tdoPayload("Add");
    tdoPayload.DeclareNamespace("http://tempuri.org/axis2/services/samples.calc.CalcService", "ns1");
    tdoPayload.CreateChild("nA", 1);
    tdoPayload.CreateChild("nB", 2);

    // output request
    std::cout << "Request: \n-------\n" << tdoPayload.ToString() << "\n------\n";

    // invoke service synchronously
    staff::DataObject tdoResult = tClient.Invoke(tdoPayload);


    // output response
    std::cout << "Response: \n-------\n" << tdoResult.ToString() << "\n------\n";

    int nResult = 0;
    tdoResult.GetChildValueByLocalName("addResult", nResult);
    std::cout << "Result: " << nResult << std::endl;
  }
  STAFF_CATCH_ALL

  return 0;
}

```


## 3. Compiling and starting client ##


#### Compiling and starting client in Visual Studio ####

Select "Build" => "Configuration Manager..." from main menu.

In window that opens set "Active solution configuration" to "Release", if you using a version from download section (else client can't start because release and debug libraries are incompatible). Click "Close".

To compile your client select "Build" => "Build Solution" from main menu. Your client should now built.

To start client select "Debug" => "Start without debugging..." from main menu.

Should appear console window with results.


#### Compiling and starting client in Qt-Creator ####

To compile your client select "Build" => "Build All" from main menu.

To start client select "Build" => "Run" from main menu.


#### Compiling and starting client using GNU Make ####

To compile your client start the `make` command (`gmake` in FreeBSD).

To start client enter `make run`.

### Results ###
You can see the results:
```
Request: 
-------
<Add xmlns:ns1="http://tempuri.org/axis2/services/samples.calc.CalcService"><nA>1</nA><nB>2</nB></Add>
------
Response: 
-------
<AddResponse><addResult>3</addResult></AddResponse>
------
Result: 3
```


## 4. Download source code ##

**[Download](http://staff.googlecode.com/files/samples.calculator_custom_client.7z)** source code of Calculator custom client.
