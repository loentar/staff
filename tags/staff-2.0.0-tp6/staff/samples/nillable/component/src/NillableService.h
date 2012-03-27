/*
 *  Copyright 2011 Utkin Dmitry
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

#ifndef _NILLABLESERVICE_H_
#define _NILLABLESERVICE_H_

#include <string>
#include <staff/common/Nillable.h>
#include <staff/common/IService.h>

namespace samples
{
namespace nillable
{

  struct Info
  {
    staff::Nillable<int> tNillableInt;
    staff::Nillable<std::string> tNillableStr;
  };

  //! service to demonstrate nillable types support
  class NillableService: public staff::IService
  {
  public:
    virtual void Set(const Info& rInfo) = 0;
    virtual Info Get() = 0;

    virtual void SetNillableStruct(const staff::Nillable<Info>& rInfo) = 0;
    virtual staff::Nillable<Info> GetNillableStruct() = 0;

    virtual staff::Nillable<int> GetNillableInt() = 0;
//    virtual int Int() = 0;
    virtual void SetNillableInt(const staff::Nillable<int>& rNillableInt) = 0;
  };

}
}

#endif // _NILLABLESERVICE_H_

