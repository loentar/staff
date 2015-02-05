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

#ifndef _STAFF_COMMON_EXCEPTION_H_
#define _STAFF_COMMON_EXCEPTION_H_

#include <staff/utils/Exception.h>
#include "staffcommonexport.h"

//! assert expression and generate DOM exception
#define STAFF_ASSERT_DOM(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) STAFF_THROW(::staff::DomException, DESCRIPTION)

//! assert expression and generate remote exception
#define STAFF_ASSERT_REMOTE(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) STAFF_THROW(::staff::RemoteException, DESCRIPTION)

#define STAFF_THROW_SOAPFAULT(CODE, STRING, DETAIL) \
  throw ::staff::SoapFaultException(STAFF_FILE_LINE, __FUNCTION__, CODE, STRING, DETAIL);

#define STAFF_THROW_SOAPUSERFAULT(DATA) \
  throw ::staff::SoapUserFaultException(STAFF_FILE_LINE, __FUNCTION__, DATA);

//! assert expression and generate remote exception
#define STAFF_ASSERT_SOAPFAULT(EXPRESSION, CODE, STRING, DETAIL) \
  if (!(EXPRESSION)) STAFF_THROW_SOAPFAULT(CODE, STRING, DETAIL)


namespace staff
{

  //! timeout exception class
  class TimeoutException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline TimeoutException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

  //! remote exception class
  class RemoteException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline RemoteException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

  //! soap fault exception class
  class SoapFaultException: public RemoteException
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sCode - soap fault code
        \param  sString - soap fault string
        \param  sDetail - soap fault detail
      */
    inline SoapFaultException(const char* szFileLine, const char* szFunction, const std::string& sCode,
                              const std::string& sString, const std::string& sDetail):
      RemoteException(szFileLine, szFunction, sString),
      m_sCode(sCode), m_sString(sString), m_sDetail(sDetail)
    {
    }

    //! destructor
    inline virtual ~SoapFaultException() throw()
    {
    }

    //! get soap fault code
    /*! \return soap fault code
      */
    inline const std::string& GetCode() const throw()
    {
      return m_sCode;
    }

    //! get soap fault string
    /*! \return soap fault string
      */
    inline const std::string& GetString() const throw()
    {
      return m_sString;
    }

    //! get soap fault detail
    /*! \return soap fault detail
      */
    inline const std::string& GetDetail() const throw()
    {
      return m_sDetail;
    }

  private:
    std::string m_sCode;
    std::string m_sString;
    std::string m_sDetail;
  };

  //! soap fault exception class
  class SoapUserFaultException: public RemoteException
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sFault - xml-serialized fault
        \param  sString - exception string (used for logging)
      */
    inline SoapUserFaultException(const char* szFileLine, const char* szFunction, const std::string& sFault,
                                  const std::string& sString = std::string()):
      RemoteException(szFileLine, szFunction, sString),
      m_sFault(sFault)
    {
    }

    //! destructor
    inline virtual ~SoapUserFaultException() throw()
    {
    }

    //! get fault
    inline const std::string& GetFault() const throw()
    {
      return m_sFault;
    }

  private:
    std::string m_sFault;
  };

  class DomException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline DomException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

}

#endif // _STAFF_COMMON_EXCEPTION_H_
