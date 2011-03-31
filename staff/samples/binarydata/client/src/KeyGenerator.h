/*
 *  Copyright 201 Utkin Dmitry
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

#ifndef _KEYGENERATOR_H_
#define _KEYGENERATOR_H_

#include <staff/common/WsdlTypes.h>
#include <staff/common/IService.h>

namespace samples
{
  namespace binarydata
  {
    //!  sample component service, to demonstare how to pass binary data over SOAP
    class KeyGenerator: public staff::IService
    {
    public:
      //! generate key based on source key
      /*! \param uSourceKey - source key
          \return generated key in hexBinary format
        */
      virtual staff::hexBinary GenerateHexKey(unsigned uSourceKey) = 0;

      //! generate key based on source key
      /*! \param uSourceKey - source key
          \return generated key in base64Binary format
        */
      virtual staff::base64Binary GenerateBase64Key(unsigned uSourceKey) = 0;

      //! validate key
      /*! \param rKey - key
          \return true if key valid
        */
      virtual bool ValidateHexKey(const staff::hexBinary& rKey) = 0;

      //! validate key
      /*! \param rKey - key
          \return true if key valid
        */
      virtual bool ValidateBase64Key(const staff::base64Binary& rKey) = 0;
    };
  }
}

#endif // _KEYGENERATOR_H_
