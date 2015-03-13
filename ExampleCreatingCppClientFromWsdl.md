<h1>Table of Contents</h1>


# Introduction #

This document describe the steps to create C++ client from WSDL.

Will be used StockQuote service from http://webservicex.net/ws/WSDetails.aspx?CATID=2&WSID=9 .

# Requirements #

WSF Staff should be installed and configured as described in InstallationManual.


# Description #

## 1. Generating client project ##

  1. Create a directory where StockQuote client will be placed;
  1. Enter this directory and start codegenerator to produce client project from wsdl:
```
  staff_codegen -pwsdl -tclient_all http://www.webservicex.net/stockquote.asmx?WSDL
```
  1. Will be generated set of files as follows:
| **File**                      | **Description**                                   |
|:------------------------------|:--------------------------------------------------|
| stockquote.asmx             | Downloaded WSDL                                 |
| Makefile                    | Makefile for GNU Make utility                   |
| stockquote\_client.pro       | Qt project file (for qmake and Qt-Creator IDE)  |
| stockquote\_client.vcproj    | Visual Studio Project file                      |
| codegen.cmd                 | Codegeneration script for VS                    |
| src/stockquote.h            | StockQuote service interface                   |
| src/main.cpp                | Client implementation stub                      |


## 2. Implementing client ##

To implement client you can use:
  * Microsoft Visual Studio 2005 or greater;
  * Qt-Creator (any version);
  * Any text editor.

You can use any of methods below:

#### Opening client project in Visual Studio ####

Double click the `stockquote_client.vcproj` to open project in Visual Studio.

> | _**Note:** If .vcproj is not associated with VS or you get an error while starting VS by double clicking on that file, you can start Visual Studio firstly then select "File" => "Open...". Navigate the stockquote\_client.vcproj then click "Open"._ |
|:-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|

In Solution Explorer navigate `main.cpp` in "Source Files" folder and open it.


#### Opening client project in Qt-Creator ####

Start Qt-Creator, click "Open Project..." button in "Welcome" mode or select "File" => "Open File or Project..." from main menu.

Navigate the `stockquote_client.pro` then click "Open".

In "Project Setup" window that opens click "Finish".

In Projects pane navigate main.cpp in "Sources/src" folder and open it.


#### Opening client source code in any text editor ####

Open your favorite text editor and open file `main.cpp` from src directory of your StockQuote client directory.


### Implementing client ###

Uncomment source code below the "Invoke Your service here:" comment.

Change line "`staff::Optional<std::string> symbol;`" to:
```
staff::Optional<std::string> symbol("IBM");
```

Now your main function of StockQuote service client should look like:
```

int main(int /*nArgs*/, const char* /*paszArgs*/[])
{
  try
  {
    std::auto_ptr< StockQuote > pStockQuote(::staff::ServiceFactory::Inst().GetService< StockQuote >());

    STAFF_ASSERT(pStockQuote.get(), "Cannot get client for service StockQuote!");

    // Invoke Your service here:

    staff::Optional<std::string> symbol("IBM");
    const staff::Optional<std::string>& GetQuoteResponse = pStockQuote->GetQuote(symbol);
    staff::LogInfo() << "GetQuote result: " << GetQuoteResponse;
  }
  STAFF_CATCH_ALL

  return 0;
}

```


## 3. Compiling and starting client ##


#### Compiling and starting client in Visual Studio ####

Select "Build" => "Configuration Manager..." from main menu.

In window that opens set "Active solution configuration" to "Release", if you using a version from download section (else client can't start because release and debug libraries are incompatible). Click "Close".

To compile your client select "Build" => "Build Solution" from main menu. Your `stockquote_client` should now built.

| _**Note**: If you got an error "LINK : fatal error LNK1104: cannot open file '.\release\stockquoteProxy.obj'", just start build process again._ |
|:------------------------------------------------------------------------------------------------------------------------------------------------|

To start client select "Debug" => "Start without debugging..." from main menu.

Should appear console window with results.


#### Compiling and starting client in Qt-Creator ####

To compile your client select "Build" => "Build All" from main menu.

To start client select "Build" => "Run" from main menu.


#### Compiling and starting client using GNU Make ####

To compile your client start the `make` command (`gmake` in FreeBSD).

To start client enter `make run`.
