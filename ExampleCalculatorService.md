<h1>Table of Contents</h1>


## 1. Creating service ##

> | _Service interface is a C++ header file that contain a class (service), which provide set of functions (service operations). Also interface can contain structures, typedefs and enums that will be used to interact with the service._ |
|:----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

Create a directory for Calculator service source code and name it `calculator`.

You can create service interface file manually, but much faster is to generate interface template using codegen:

```
staff_codegen -tinterfacegen -dsvc=samples.Calculator -osrc
```

Will be generated service interface template `Calculator.h` inside directory `src`.

Open generated `Calculator.h` in Visual Studio, Qt-Creator or any text editor. Add service operations into `public` section of class Calculator:
```
virtual int Add(int a, int b) = 0;
virtual int Sub(int a, int b) = 0;
```

Now your service class should look like:
```
  //! Calculator service
  class Calculator: public staff::IService
  {
  public:
    virtual int Add(int a, int b) = 0;
    virtual int Sub(int a, int b) = 0;
  };
```

Generate service implementation and project files:
```
staff_codegen -tcomponent_all -isrc Calculator.h
```

Will be generated set of files as follows:
| **File name**              | **Description** |
|:---------------------------|:----------------|
| `Makefile`               | Makefile for GNU Make utility |
| `samples_service.vcproj` | Visual Studio project file |
| `codegen.cmd`            | Codegeneration script for VS |
| `install.cmd`            | Installation script for VS |
| `src/CalculatorImpl.h`   | Calculator service implementation (header) |
| `src/CalculatorImpl.cpp` | Calculator service implementation |


## 2. Implementing the service ##

To implement service you can use:
  * Microsoft Visual Studio 2005 or greater;
  * Qt-Creator (any version);
  * Any text editor.

You can use any of methods below:

#### Opening service project in Visual Studio ####

Double click the `samples_service.vcproj` to open project in Visual Studio.

> | _**Note:** If .vcproj is not associated with VS or you get an error while starting VS by double clicking on that file, you can start Visual Studio firstly then select "File" => "Open...". Navigate the samples\_service.vcproj then click "Open"._ |
|:-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

In Solution Explorer navigate `CalculatorImpl.cpp` in "Source Files" folder and open it.


#### Opening service project in Qt-Creator ####

Start Qt-Creator, click "Create Project..." button in "Welcome" mode or select "File" => "New File or Project..." from main menu. Choose the template "Other Project" on the left pane and "Import Existing Project" on the right pane, click "Choose...".

Name the project as "calculator" and select the directory that contain generated `Makefile` in "Location", click "Next `>`",  "Next `>`", "Finish".

To enable completion you can add a line into `calculator.includes` that contain a path to Staff include directory (usually `c:/ws/staff/include` in Windows and `/usr/local/staff/include` in Linux, FreeBSD and Mac OS X).

In Projects pane navigate `CalculatorImpl.cpp` in "Sources/src" folder and open it.


#### Opening service source code in any text editor ####

Start your favorite text editor and open file `CalculatorImpl.cpp` from `src` directory of your Calculator service directory.


### Implementing the Calculator service ###

Implement `Add` and `Sub` operations as follows:
```
int CalculatorImpl::Add(int a, int b)
{
  return a + b;
}

int CalculatorImpl::Sub(int a, int b)
{
  return a - b;
}
```

## 3. Compiling and installing service ##


#### Compiling and installing Calculator service in Visual Studio ####

Select "Build" => "Configuration Manager..." from main menu.

In window that opens set "Active solution configuration" to "Release", if you using a version from download section (else service can't start because release and debug libraries are incompatible). Click "Close".

To compile your service select "Build" => "Build Solution" from main menu. Your service should now built.

Start `install.cmd` from project directory to install the service.


#### Compiling and installing Calculator service in Qt-Creator ####

To compile your service select "Build" => "Build All" from main menu.

To install your service start:
```
sudo -E make install
```


#### Compiling and installing Calculator service using GNU Make ####

To compile your service start the `make` command (`gmake` in FreeBSD).

To install your service start:
```
sudo -E make install
```


## 4. Starting ##

Start the Axis2/C from AXIS2C\_HOME/bin directory.

Open the Web browser and go to address: http://localhost:9090/axis2/services

Your new service must shown as:

|<font face='courier'><h3><u>samples.Calculator</u></h3><p>Calculator service <sup><a><code>[</code><a href='http://localhost:9090/axis2/services/staff.wsdl.Wsdl/get/samples/samples.Calculator.wsdl'>wsdl</a><code>]</code></a></sup></p><i>Available Operations</i> <ul><li>Add</li><li>Sub</li></ul></font>|
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|


## 5. Create Calculator client ##

To create Calculator client please see ExampleCalculatorClient.

## 6. Download source code ##

**[Download](http://staff.googlecode.com/files/samples.calculator_component.7z)** source code of the Calculator service.
