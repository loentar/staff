<h1>Table of Contents</h1>


## 1. Creating client ##

Create a directory for Calculator client source code and name it `calculator_client`.

Client can be created from wsdl and from service interface:

**To create client from wsdl**

Ensure that Axis2/C is on and start codegen:
```
staff_codegen -pwsdl -tclient_all http://localhost:9090/axis2/services/staff.wsdl.Wsdl/get/samples/samples.Calculator.wsdl
```


**To create client from interface**

Create a directory inside `calculator_client` and name it `src`. Copy service interface (`Calculator.h`) there.

Start codegen:
```
staff_codegen -tclient_all -isrc Calculator.h
```


In both cases, it will be generated set of files as follows:
| **File name**                         | **Description** |
|:--------------------------------------|:----------------|
| `samples.Calculator.wsdl`           | Downloaded WSDL (_only when generating from wsdl_) |
| `Makefile`                          | Makefile for GNU Make utility |
| `samples_Calculator_client.pro`     | Qt project file (for qmake and Qt-Creator IDE) |
| `samples_Calculator_client.vcproj`  | Visual Studio project file |
| `codegen.cmd`                       | Codegeneration script for VS |
| `src/Calculator.h`                  | Calculator service interface (_only when generating from wsdl_) |
| `src/main.cpp`                      | Calculator client implementation |


## 2. Implementing client ##

Open generated project in your preferred IDE or text editor.

Open `main.cpp` and add those lines next the comment "Invoke Your service here":
```
std::cout << "1 + 2 = " << pCalculator->Add(1, 2) << std::endl;
std::cout << "1 - 2 = " << pCalculator->Sub(1, 2) << std::endl;
```


## 3. Compiling and starting client ##


#### Compiling and starting client in Visual Studio ####

Select "Build" => "Configuration Manager..." from main menu.

In window that opens set "Active solution configuration" to "Release", if you using a version from download section (else client can't start because release and debug libraries are incompatible). Click "Close".

To compile your client select "Build" => "Build Solution" from main menu. Your client should now built.

| _**Note**: If you got an error "LINK : fatal error LNK1104: cannot open file '.\release\CalculatorProxy.obj'", just start build process again. This error can occur only you started build first time._ |
|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

To start client select "Debug" => "Start without debugging..." from main menu.

Should appear console window with results.


#### Compiling and starting client in Qt-Creator ####

To compile your client select "Build" => "Build All" from main menu.

To start client select "Build" => "Run" from main menu.


#### Compiling and starting client using GNU Make ####

To compile your client start the `make` command (`gmake` in FreeBSD).

To start client enter `make run`.

You can see the results:
|1 + 2 = 3<p />1 - 2 = -1|
|:-----------------------|


## 4. Download source code ##

**[Download](http://staff.googlecode.com/files/samples.calculator_client.7z)** source code of Calculator client.
