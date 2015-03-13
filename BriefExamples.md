<h1>Table of Contents</h1>


# Simple service and client: Calculator #

Writing WSF Staff-based service is so easy as writing ordinary C++ class.

To mark class is a service you should inherit it from staff::IService.

## Sample of Calculator interface ##

_Interface file describes service's operations:_

```
// Calculator.h
#include <staff/common/IService.h>

class Calculator: public staff::IService
{
public:
  virtual int Add(int a, int b) = 0;
};
```


## Sample of Calculator service implementation ##

```
// CalculatorImpl.h
#include "Calculator.h"

class CalculatorImpl: public Calculator
{
public:
  int Add(int a, int b);
};
```

```
// CalculatorImpl.cpp
#include "CalculatorImpl.h"

int CalculatorImpl::Add(int a, int b)
{
  return a + b;
}

```

## Sample of Calculator client ##

```
// calculator_client.cpp
#include <iostream>
#include <staff/client/ServiceFactory.h>
#include "Calculator.h"

int main(int, const char**)
{
  // get Calculator service by default url
  std::auto_ptr<Calculator> calc = staff::ServiceFactory::Inst().GetService<Calculator>();

  // execute Add operation
  int result = calc->Add(1, 2);

  // output
  std::cout << "1 + 2 = " << result << std::endl;

  return 0;
}
```

## Sample of non blocking Calculator client ##

```
// Calculator.h
#include <staff/common/IService.h>

class Calculator: public staff::IService
{
public:
  virtual void Add(int a, int b, staff::ICallback<int>& rCallback) = 0;
};
```

```
// calc_client_non_blocking.cpp
#include <memory>
#include <staff/utils/Log.h>
#include <staff/utils/Thread.h>
#include <staff/common/DataObject.h>
#include <staff/client/ICallback.h>
#include <staff/common/Exception.h>
#include <staff/client/ServiceFactory.h>
#include "Calculator.h"

// callback for Calculator::Add
class CalculatorAddCallback: public staff::ICallback< int >
{
public:
  virtual void OnComplete(int tResult)
  {
    // process result here
    staff::LogInfo() << "Add(asynch) result: " << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    staff::LogError() << rFault.ToString();
  }
};

int main(int, const char**)
{
  try
  {
    std::auto_ptr< Calculator > pCalculator(::staff::ServiceFactory::Inst().GetService<Calculator>());

    STAFF_ASSERT(pCalculator.get(), "Cannot get client for service Calculator!");

    // Invoking the service

    CalculatorAddCallback tCalculatorAddCallback;
    pCalculator->Add(1, 2, tCalculatorAddCallback);

    while (!tCalculatorAddCallback.IsCompleted())
    {
      staff::Thread::Sleep(1000);
    }
  }
  STAFF_CATCH_ALL

  return 0;
}
```

## Sample of Calculator AJAX client ##

```
// invoke the service
function OnButtonAddClick()
{
  var calc = new Calculator();
  var result = calc.Add(1, 2);
  alert("1 + 2 = " + result);
}
```

# Using complex types: Users service #

Using complex types also transparent.

## Sample of Users interface ##

```
// Users.h
#include <string>
#include <list>
#include <staff/common/IService.h>

// complex type: User
struct User
{
  int id;
  std::string name;
  std::string description;
};

// second complex type: users list
typedef std::list<User> UsersList;

// Users service
class Users: public staff::IService
{
public:
  virtual int Add(const std::string& name, const std::string& description) = 0;
  virtual void Del(int id) = 0;
  virtual User Get(int id) = 0;
  virtual UsersList GetList() const = 0;
};
```


## Sample of Users service ##

```
// UsersImpl.h
#include "Users.h"

class UsersImpl: public Users
{
public:
  UsersImpl();
  int Add(const std::string& name, const std::string& description);
  void Del(int id);
  User Get(int id);
  UsersList GetList() const;

private:
  int m_nLastId;
  UsersList m_lsUsers;
};


// UsersImpl.cpp
#include "UsersImpl.h"

UsersImpl::UsersImpl():
  m_nLastId(0)
{
}

int UsersImpl::Add(const std::string& name, const std::string& description)
{
  User user;
  user.id = ++m_nLastId;
  user.name = name;
  user.description = description;

  // just put it to list
  m_lsUsers.push_back(user);
}

void UsersImpl::Del(int id)
{
  // search and delete user from list by id
  for(UsersList::iterator it = m_lsUsers.begin(); it != m_lsUsers.end(); ++it)
  {
    if (it->id = id)
    {
      m_lsUsers.erase(it);
      break;
    }
  }
}

User UsersImpl::Get(int id)
{
  // search user by id
  for(UsersList::const_iterator it = m_lsUsers.begin(); it != m_lsUsers.end(); ++it)
  {
    if (it->id = id)
    {
      return *it;
    }
  }
}

UsersList UsersImpl::GetList()
{
  return m_lsUsers;
}

```

## Sample of Users client ##

```
// users_client.cpp
#include <iostream>
#include <staff/client/ServiceFactory.h>
#include "Users.h"

// manipulator for struct output 
std::ostream& operator<<(std::ostream& out, const User& user)
{
  return out << "id: " << user.id << " name: " << user.name << " descr: " << user.description;
}

int main(int, const char**)
{
  // get Users service by default url
  std::auto_ptr<Users> users = staff::ServiceFactory::Inst().GetService<Users>();

  // execute operations
  // add users
  int john_id = users->Add("John", "Project leader");
  int mary_id = users->Add("Mary", "Developer");

  User john = users->Get(john_id);

  // get john and output
  std::cout << "User from service: " << john << std::endl;

  // getting list and output
  UsersList users_list = users->GetList();

  std::cout << std::endl << "Users:" << std::endl;
  for(UsersList::iterator it = users_list.begin(); it != users_list.end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}
```

## Sample of Users AJAX client ##

```
include("clients/Users")

function DumpUser(user)
{
  return "id: " + user.id + " name: " + user.name + " description: " + user.description;
}

function StartSample()
{
  // get Users service by default url
  var users = new Users();

  // execute operations
  // add users
  var john_id = users.Add("John", "Project leader");
  var mary_id = users.Add("Mary", "Developer");

  var john = users.Get(john_id);

  // get john and output
  alert("User from service: " + DumpUser(john));

  // getting list and output
  var users_list = users.GetList();

  var result = "Users:\n";
  for (var it in users_list)
  {
    var user = users_list[it];
    if (typeof user == 'object') // filter out prototype.js functions
    {
      result += DumpUser(user) + "\n";
    }
  }

  alert(result);
}
```

# Sample of simple RESTful Calculator #

_Note: To mark service as RESTful just add special comments into interface file. Service implementation leave unchanged._

## RESTful service interface ##

```
// Calculator.h
#include <staff/common/IService.h>

class Calculator: public staff::IService
{
public:
  // *restMethod: GET
  // *restLocation: add?a={a}&b={b}
  virtual int Add(int a, int b) = 0;
};
```



## Executing RESTful service ##

You can execute RESTul service from web browser. Just enter link containing service name and params:

http://localhost:9090/axis2/services/Calculator/add?a=1&b=2

Browser output:
|`<`addResult`>`3`<`/addResult`>`|
|:-------------------------------|

<br />


# Using Optional and Nillable #

## Interface with Optional and Nillable ##

Optional and Nillable is very similar in use.

```
...
#include <staff/common/Optional.h>
#include <staff/common/Nillable.h>
...

// EchoNillOpt.h
class EchoNillOpt: public staff::IService
{
public:
  virtual staff::Optional<std::string> EchoOptional(const staff::Optional<std::string>& text) = 0;
  virtual staff::Nillable<std::string> EchoNillable(const staff::Nillable<std::string>& text) = 0;
};

```

## Using Optional and Nillable ##

```
...
#include <staff/common/Optional.h>
#include <staff/common/Nillable.h>
...

  staff::Optional<std::string> text;
  *text = "Hello, world!"; // set optional text
  staff::Optional<std::string> result = pEcho->EchoOptional(text);

  std::cout << "result: ";
  if (!result.IsNull())
  {
    std::cout << *result << std::endl;
  }
  else
  {
    std::cout << "(not set)" << std::endl;
  }
  

  staff::Nillable<std::string> text;
  *text = "Hello, world!"; // set nillable text
  staff::Nillable<std::string> result = pEcho->EchoNillable(text);

  std::cout << "result: ";
  if (!result.IsNull())
  {
    std::cout << *result << std::endl;
  }
  else
  {
    std::cout << "(null)" << std::endl;
  }

...
```

# Using Abstract datatypes #

## Interface with abstract datatypes ##

```
// AbstractTransport.h
...
#include <staff/common/Abstract.h>
...

  // *abstract: true
  struct Vehicle
  {
    // virtual destructor should present to get rtti working
    virtual ~Vehicle() {}
  };

  struct Car: public Vehicle
  {
    std::string sModel;
  };

  struct Plane: public Vehicle
  {
    std::string sSerialNumber;
  };

  enum TransportType
  {
    TransportTypeCar,
    TransportTypePlane
  };

  class Transport: public staff::IService
  {
  public:
    virtual staff::Abstract<Vehicle> GetTransport(TransportType eTransport) = 0;
  };

```

## Using abstract type on service side ##

```
// AbstractTransportImpl.cpp
...

staff::Abstract<Vehicle> TransportImpl::GetTransport(TransportType eTransport)
{
  staff::Abstract<Vehicle> tpResult;
  switch (eTransport)
  {
    case TransportTypeCar:
    {
      tpResult = new Car;
      Car& rCar = static_cast<Car&>(*tpResult);
      rCar.sModel = "cabriolet";
      break;
    }
    
    case TransportTypePlane:
    {
      tpResult = new Plane;
      Plane& rPlane = static_cast<Plane&>(*tpResult);
      rPlane.sSerialNumber = "012345";
      break;
    }
    
    default:
      STAFF_THROW(staff::RemoteException, "Invalid transport: " + staff::ToString(eTransport));
  }
  
  return tpResult;
}

```

## Using abstract type on client side ##

```
// AbstractTransportClient.cpp
...

  staff::Abstract<Vehicle> tResult = pTransport->GetTransport(TransportTypePlane);
  const Plane& rPlaneRes = dynamic_cast<const Plane&>(*tResult);
  std::cout << "plane serialno = " << rPlaneRes.sSerialNumber;

...
```

# WSDL generation #

## Generating generic WSDL ##
Service interface `Calculator.h` must located in current directory. To generate wsdl for Calculator service enter:

```
staff_codegen -twsdl Calculator.h
```

Calculator.wsdl will be generated.

## Generating WSDL with BPEL support ##

To generate WSDL with BPEL support, define special flag:

```
staff_codegen -twsdl -dbpel Calculator.h
```

Calculator.wsdl with BPEL support will be generated.


# Protobuf support #

## Generating service interface from .proto file ##

Source Calculator.proto file:
```
// Calculator.proto

package samples.protobuf.calc;

message AddRequest
{
  required int32 a = 1;
  required int32 b = 2;
};

message AddResponse
{
  required int32 result = 1;
};

service Calculator
{
  rpc Add(AddRequest) returns (AddResponse);
};

```

To generate service interface enter:
```
staff_codegen -pprotobuf Calculator.proto -tinterface
```

Resulting service interface:
```
// Calculator.h

#ifndef _Calculator_h_
#define _Calculator_h_

#include <staff/common/IService.h>


namespace samples
{
namespace protobuf
{
namespace calc
{
  struct AddRequest;
  struct AddResponse;

  struct AddRequest
  {
    // *protobufTag: 1
    int a;
    // *protobufTag: 2
    int b;
  };

  struct AddResponse
  {
    // *protobufTag: 1
    int result;
  };


  //!  service samples.protobuf.calc.Calculator
  class Calculator: public staff::IService
  {
  public:
    virtual AddResponse Add(const AddRequest& request) = 0;
  };

}
}
}

#endif // _Calculator_h_

```