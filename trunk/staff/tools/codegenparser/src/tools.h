/*
 *  Copyright 2010 Utkin Dmitry
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


#ifndef STAFF_CODEGEN_TOOLS_H
#define STAFF_CODEGEN_TOOLS_H

#include <string>
#include "Interface.h"

namespace staff
{
namespace codegen
{
  //! get base type(currently struct and enum) by full or partial name in current context
  /*! \param  sNsName - type name in form [[[[some::]namespace::]Struct::]SubStruct::]SubSubstruct
      \param  rstInterface - reference to current interface
      \param  pstParent - optional pointer to structure search from
      \return found structure, NULL if not found
    */
  const SBaseType* GetBaseType(const std::string& sNsName, const SInterface& rstInterface,
                               const int eBaseType = SBaseType::EAny,
                               const SStruct* pstParent = NULL);

  //! get structure by full or partial name in current context
  /*! \param  sNsName - structure's name in form [[[[some::]namespace::]Struct::]SubStruct::]SubSubstruct
      \param  rstInterface - reference to current interface
      \param  pstParent - optional pointer to structure search from
      \return found structure, NULL if not found
    */
  const SStruct* GetStruct(const std::string& sNsName, const SInterface& rstInterface,
                           const SStruct* pstParent = NULL);


  //! optimize C++ namespace
  /*! example:
        sOptimizeNs: ::samples::ticket::Ticket
        sCurrentNs: ::samples::sharedtypes::
        result: ticket::

      \param  sOptimizeNs - namespace to optimize
      \param  sCurrentNs - current namespace
  */
  void OptimizeCppNs(std::string& sOptimizeNs, const std::string& sCurrentNs);

  //! find existing or add new type in list
  /*! \param rList - list
      \param sName - type name
      \param sNamespace - type namespace
      \param sOwnerName - owner name
      */
  template<typename Type>
  Type& TypeInList(std::list<Type>& rList, const std::string& sName, const std::string& sNamespace,
                   const std::string& sOwnerName = "")
  {
    typename std::list<Type>::iterator itType = rList.begin();
    for (; itType != rList.end(); ++itType)
    {
      if (itType->sName == sName && itType->sNamespace == sNamespace)
      {
        return *itType;
      }
    }

    itType = rList.insert(rList.end(), Type());
    itType->sName = sName;
    itType->sNamespace = sNamespace;
    itType->sOwnerName = sOwnerName;
    return *itType;
  }


}
}

#endif // STAFF_CODEGEN_TOOLS_H
