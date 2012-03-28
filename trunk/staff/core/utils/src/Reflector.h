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


#ifndef _STAFF_UTILS_REFLECTOR_H_
#define _STAFF_UTILS_REFLECTOR_H_

#include <string>
#include <typeinfo>
#include "staffutilsexport.h"

namespace staff
{

  //! rtti type name reflector
  /*! example:
      <pre>
      namespace example
      {
        struct MyStruct
        { ... };
      }
      // ...
      example::MyStruct st;
      staff::Reflector::GetTypeName(st); // returns "example.MyStruct"
      staff::Reflector::GetTypeName(&st); // returns "example.MyStruct"
      </pre>
    */
  class STAFF_UTILS_EXPORT Reflector
  {
  public:
    //! get type name
    /*! \param  tType - source type
        \return decoded type name. Namespace is delimited by dot
      */
    template <typename Type>
    inline static std::string GetTypeName(Type tType)
    {
      return DecodeTypeName(typeid(tType).name());
    }

    //! decode type name
    /*! \param  szName - name
        \return decoded type name. Namespace is delimited by dot
      */
    static std::string DecodeTypeName(const char* szName);
  };

}

#endif // _STAFF_UTILS_REFLECTOR_H_
