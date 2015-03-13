# Example: Creating a data source client #

Please read the ExampleCalculatorClient to understand the process of client creation.

## 1. Directory structure ##

Create a client directory structure:

<b>Under Linux:</b>
```
mkdir -p ~/devel/samples/das_accounts_client
```

<b>Under Windows:</b>

Start Visual Studio and create new solution, named "das\_accounts\_client".

While creating the solution use template Win32=>Win32 Console Application, Click "OK". Click "Next".

In "Application Settings" dialog select "Console Application" and "Empty project", Click "Finish".

Solution is now created.


## 2. Creating a service interface ##
Service's interface for datasource client can be created by three way:
  * by querying DataAccessService using codegen's das plugin
  * from wsdl file, using codegen's wsdl plugin
  * from datasourcefile directly, using codegen's dasdatasource plugin

Enter into das\_accounts\_client directory and create source directory named "src".
Enter into source directory and use one of methods below to create service interface.
Service interface will be generated in Accounts.h.


### 2.1. Creating a service interface, using das plugin ###

Datasource must be installed. Axis2/C application manager must running.
Virtual service staff.das.samples.Accounts must be in the Axis2/C services list.

To generate service interface start codegen:

```
staff_codegen -pdas -tinterface staff.das.samples.Accounts
```


### 2.2. Creating a service interface from wsdl ###

Get wsdl from http://localhost:9090/axis2/services/staff.das.samples.Accounts?wsdl and save it into source directory as staff.das.samples.Accounts.wsdl.

To generate service interface start codegen:
```
staff_codegen -pwsdl -tinterface staff.das.samples.Accounts.wsdl
```


### 2.3. Creating a service interface from datasource file directly ###

To generate service interface start codegen:

```
staff_codegen -pdasdatasource -tinterface FULL_PATH_TO_ACCOUNT_DATASOURCES
```

Where FULL\_PATH\_TO\_ACCOUNT\_DATASOURCES is:

under Windows:
```
%STAFF_HOME%\components\staff.das\datasources\staff.das.samples\Accounts.datasources
```

under Linux:
```
$STAFF_HOME/components/staff.das/datasources/staff.das.samples/Accounts.datasources
```


## 3. Generating client stub ##

To generate client stub enter the source directory and start codegen:
```
staff_codegen -tclientimpl Accounts.h
```

main.cpp will be generated.

## 4. Implementing the client ##

Open generated main.cpp and implement the client as follows:

Add ostream manipulators for easy screen output immediately before function "main":
```
std::ostream& operator<<(std::ostream& rOut, const staff::das::samples::Account& rstAccount)
{
  return rOut << "\t" << rstAccount.nId << "  \t  " << rstAccount.sLogin << "  \t  " << rstAccount.sEMail << "  \t  " << rstAccount.sDescr << std::endl;
}

template<typename Item>
std::ostream& operator<<(std::ostream& rOut, const std::list<Item>& rlsList)
{
  for (typename std::list<Item>::const_iterator itItem = rlsList.begin(); itItem != rlsList.end(); ++itItem)
  {
    rOut << *itItem;
  }
  return rOut;
}
```


Insert these lines below the "Invoke Your service here:" remark.
```
    // remove test account if it already exists
    pAccounts->RemoveByLogin("test");

    // get accounts list
    ::staff::das::samples::AccountsList lsAccounts = pAccounts->GetAll();
    std::cout << "accounts list: " << std::endl << lsAccounts << std::endl;

    std::cout << "adding test account..." << std::endl;
    int nId = pAccounts->Add("test", "test@somesite.org", "test account");
    std::cout << "test account id: " << nId << std::endl;

    std::cout << "getting added test account..." << std::endl;
    ::staff::das::samples::Account tAccount = pAccounts->Get(nId);
    std::cout << "test account: " << std::endl << tAccount << std::endl;

    lsAccounts = pAccounts->GetAll();
    std::cout << std::endl << "now accounts list is: " << std::endl << lsAccounts << std::endl;


    std::cout << "removing test account..." << std::endl;
    pAccounts->Remove(nId);
    std::cout << "Remove called" << std::endl;

    lsAccounts = pAccounts->GetAll();
    std::cout << std::endl << "now accounts list is: " << std::endl << lsAccounts << std::endl;
```

## 5. Generating client Proxy ##

To generate client stub enter the source directory and start codegen:
```
staff_codegen -tclient Accounts.h
```

These files will be generated:
```
AccountsProxy.h
AccountsProxy.cpp
```

## 6. Compilation ##

### 6.1. Compilation under Windows ###
Add all source files into Solution:
Right click to "das\_accounts\_client" project, select "Add" => "Existing Item...". In opened dialog go inside "src" directory and select all files. Click "Add".

Go to Project Settings: Right click to "das\_accounts\_client" project and select "Properties". Click to Configuration Drop down box and select "All configurations" In the "Configuration properties" tree select "C/C++" item and enter these "Additional Include Directories":
```
$(STAFF_HOME)\include
```
Click "Apply".

In the "Configuration properties" tree open "Linker" subtree and click "Input". Fill in the "Additional Dependencies" field as follows:
```
staff_client.lib staff_common.lib staff_utils.lib
```
Click "Apply".

In the "Linker" subtree click "General" item. Fill in the "Additional Library Directories" field as follows:
```
$(STAFF_HOME)\lib
```
Click "OK".

In the "Build Events" subtree click "Pre-Build Event" and fill in the "Command Line" field as follows:
```
staff_codegen -tclient -csrc -u Accounts.h
```
Set active solution configuration to release:

Click "Build" => "Configuration manager". Click "Active solution configuration" dropdown box and select "Release". Click "Close".

Compile the solution: "Build" => "Build Solution".

Client will be placed into Release/das\_accounts\_client.exe .

### 6.2. Compilation under Linux ###

Put Makefile as ~/devel/das\_accounts\_client/Makefile:

```
TARGET = das_accounts_client
TARGETNAME = $(TARGET)

PLATFORM ?= $(shell uname -m)
SRCDIR = src/
OBJDIR = obj/$(PLATFORM)/
OUTDIR = out/$(PLATFORM)/

CXXFLAGS += -Wno-deprecated -I$(STAFF_HOME)/include -fPIC
LDFLAGS += -fPIC -L$(STAFF_HOME)/lib -lstaffclient

COMPONENT_SERVICES_HEADERS := $(SRCDIR)Accounts.h
CODEGEN := $(patsubst %.h,%Proxy.h,$(COMPONENT_SERVICES_HEADERS)) $(patsubst %.h,%Proxy.cpp,$(COMPONENT_SERVICES_HEADERS))
CODEGEN_SOURCES := $(filter %.cpp,$(CODEGEN))

SOURCES = $(filter-out $(CODEGEN_SOURCES) ,$(wildcard $(SRCDIR)*.cpp)) $(CODEGEN_SOURCES)
OBJECTS = $(patsubst %.cpp,$(OBJDIR)%.o,$(notdir $(SOURCES)))

.NOTPARALLEL: $(CODEGEN_SOURCES)

# == make ===========================================
make: check $(CODEGEN) "$(OBJDIR)" "$(OUTDIR)" $(OUTDIR)$(TARGETNAME)

# link
$(OUTDIR)$(TARGETNAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(OUTDIR)$(TARGETNAME)

# compile
$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(SRCDIR)%Proxy.h $(SRCDIR)%Proxy.cpp:: $(SRCDIR)%.h
	staff_codegen -tclient -csrc -u $(patsubst $(SRCDIR)%,%,$(COMPONENT_SERVICES_HEADERS))

# == install ========================================
install: check "$(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)"
	cp -f $(OUTDIR)$(TARGETNAME) $(INSTALLDIR)$(SAMPLESDIR)$(BINDIR)$(TARGETNAME)

# == clean ==========================================
clean:
	rm -Rf $(OBJDIR) $(OUTDIR)
	rm -f $(CODEGEN)

# == mkdir ==========================================
"%/":
	@[ -z "$@" -o -d "$@" ] || mkdir -p $@ && chmod g+w $@

# == check ==========================================
check:
ifeq ($(STAFF_HOME),)
	@echo "\033[31;1mERROR: STAFF_HOME is not set\033[0m"; false
endif

```

Compile client with command
```
cd ~/devel/samples/das_accounts_client
make
```

Client will be placed into out/`<`Your\_Platform`>`/das\_accounts\_client.

## 7. Start client ##

Start client, You should see client execution results:

```
accounts list: 
	0  	  admin  	  admin@localhost  	  Administrator
	1  	  user  	  user@mysite.com  	  User

adding test account...
test account id: 5
getting added test account...
test account: 
	5  	  test  	  test@somesite.org  	  test account


now accounts list is: 
	0  	  admin  	  admin@localhost  	  Administrator
	1  	  user  	  user@mysite.com  	  User
	5  	  test  	  test@somesite.org  	  test account

removing added test account...
Remove called

now accounts list is: 
	0  	  admin  	  admin@localhost  	  Administrator
	1  	  user  	  user@mysite.com  	  User

```

## 8. Download ##
Download client source code, datasource, database [here](http://code.google.com/p/staff/downloads/detail?name=samples.das.Accounts.7z).
