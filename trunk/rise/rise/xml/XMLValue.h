/*
 *  Copyright 2009 Utkin Dmitry
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

#ifndef _XMLVALUE_H_
#define _XMLVALUE_H_

#include <rise/os/oscommon.h>
#include <rise/string/String.h>
#include <rise/xml/XMLFileStream.h>

namespace rise
{
  namespace xml
  {
    //!        xml value
    class RISE_EXPORT CXMLValue
    {
    public:
      //!        constructor
      CXMLValue();
      
      //!         constructor
      /*! \param  rValue - value
          */
      CXMLValue(const CXMLValue& rValue);

      //!         constructor
      /*! \param  rValue - value
      */
      template<typename TTYPE>
      CXMLValue(const TTYPE rValue);

      //!         constructor
      /*! \param  rValue - value
      */
      CXMLValue(const CString& rValue);


      //!         copy operator
      /*! \param  rValue - source value
          \return *this
      */
      CXMLValue& operator=(const CXMLValue& rValue);

      //!         copy operator
      /*! \param  rValue - source value
          \return *this
      */
      template<typename TTYPE>
      CXMLValue& operator=(TTYPE rValue);

      //!         copy operator
      /*! \param  rValue - source value
          \return *this
      */
      CXMLValue& operator=(const CString& rValue);

      //!         copy operator
      /*! \param  rValue - source value
          \return *this
      */
      CXMLValue& operator=(const TChar* rValue);

      //!         check values for equality
      /*! \param  rValue - other value
          \return true, if values are equal
      */
      template<typename TTYPE>
      bool operator==(const TTYPE rValue) const;

      //!         check values for equality
      /*! \param  rXMLValue - other value
          \return true, if values are equal
      */
      bool operator==(const CXMLValue& rXMLValue) const;

      //!         check values for equality
      /*! \param  rValue - other value
          \return true, if values are equal
      */
      bool operator==(const TChar* rValue) const;

      //!         check values for equality
      /*! \param  rValue - other value
          \return true, if values are equal
      */
      bool operator==(const CString& rValue) const;

      //!         check values for inequality
      /*! \param  rValue - other value
          \return true, if values are inequal
      */
      template<typename TTYPE>
      bool operator!=(const TTYPE rValue) const;

      //!         check values for inequality
      /*! \param  rXMLValue - other value
          \return true, if values are inequal
      */
      bool operator!=(const CXMLValue& rXMLValue) const;

      //!         check values for inequality
      /*! \param  rValue - other value
          \return true, if values are inequal
      */
      bool operator!=(const TChar* rValue) const;

      //!         check values for inequality
      /*! \param  rValue - other value
          \return true, if values are inequal
      */
      bool operator!=(const CString& rValue) const;

      //!         type cast operator
      /*! \return casted result
      */
      template<typename TTYPE>
      operator const TTYPE() const;

      //!         type cast operator
      /*! \return casted result
      */
      template<typename TTYPE>
      operator TTYPE();

      //!         type cast operator
      /*! \return casted result
      */
      operator const CString&() const;

      //!         type cast operator
      /*! \return casted result
      */
      operator CString&();

      //!         cast to string
      /*! \return string value
      */
      const CString& AsString() const;

      //!         cast to string
      /*! \return string value
      */
      CString& AsString();

      //!         cast to int
      /*! \return int value
      */
      int AsInt() const;

      //!         cast to float
      /*! \return float value
      */
      float AsFloat() const;

      //!         cast to double
      /*! \return double value
      */
      double AsDouble() const;

    private:
      CString m_sValue; //!< value

    };

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLValue - reference to value
        \return  serializing stream
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLValue& rXMLValue);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLValue - reference to value
        \return  deserializing stream
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLValue& rXMLValue);

    //!          serializing operator
    /*! \param   rStream - serializing stream
        \param   rXMLValue - reference to value
        \return  serializing stream
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLValue& rXMLValue);

    //!          deserializing operator
    /*! \param   rStream - deserializing stream
        \param   rXMLValue - reference to value
        \return  deserializing stream
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLValue& rXMLValue);


  } // namespace xml
} // namespace rise

#include "XMLValue.hpp"

#endif // _XMLVALUE_H_
