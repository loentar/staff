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

#include "Interface.h"

namespace staff
{
namespace codegen
{

  DataType::DataType():
    bIsConst(false), bIsRef(false), eType(TypeGeneric)
  {
  }

  Member::Member():
    bIsConst(false), bIsAsynch(false)
  {
  }

  BaseType::BaseType():
    eType(TypeUnknown), bExtern(false), bForward(true)
  {
  }

  Enum::Enum()
  {
    eType = TypeEnum;
  }

  Struct::Struct()
  {
    eType = TypeStruct;
  }

  Typedef::Typedef()
  {
    eType = TypeTypedef;
  }

}
}
