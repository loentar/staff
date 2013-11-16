/*
 *  Copyright 2013 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */


#ifndef _TEST_DEFAULT_QUALIFIED_H_
#define _TEST_DEFAULT_QUALIFIED_H_

#include <string>
#include <list>
#include <staff/common/IService.h>
#include <staff/common/DataObject.h>

// *interface.elementFormDefault: qualified
// *interface.attributeFormDefault: qualified
// *interface.targetNamespace: http://tempui.org/test/TEST_DEFAULT_QUALIFIED

namespace staff
{
namespace tests
{

  struct TestDefaultQualified
  {
    // *useParentElement: true
    // *form: qualified
    std::list<std::string> qual;

    // *useParentElement: true
    // *form: unqualified
    std::list<std::string> unqual;

    // *useParentElement: true
    std::list<std::string> def;

    // *isAttribute: true
    // *form: unqualified
    std::string u;

    // *isAttribute: true
    // *form: qualified
    std::string q;

    // *isAttribute: true
    std::string d;
  };


  class DummyQualified: public IService
  {
  public:
    virtual staff::DataObject Test(const TestDefaultQualified& data) = 0;
  };

}
}

#endif

