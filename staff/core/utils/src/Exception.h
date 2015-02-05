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

#ifndef _STAFF_UTILS_EXCEPTION_H_
#define _STAFF_UTILS_EXCEPTION_H_

#include <string>
#include <exception>
#include "staffutilsexport.h"

#ifndef STAFF_FILE_LINE
#define STAFF_STRINGIZE(x) #x
#define STAFF_TO_STRING(x) STAFF_STRINGIZE(x)
#define STAFF_FILE_LINE __FILE__ "[" STAFF_TO_STRING(__LINE__) "] "
#endif

//! throw specific exception
#define STAFF_THROW(EXCEPTION_CLASS, DESCRIPTION) \
  throw EXCEPTION_CLASS(STAFF_FILE_LINE, __FUNCTION__, DESCRIPTION);

//! throw assert exception
#define STAFF_THROW_ASSERT(DESCRIPTION) \
  throw ::staff::AssertException(STAFF_FILE_LINE, __FUNCTION__, DESCRIPTION);

//! assert expression
#define STAFF_ASSERT(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) STAFF_THROW_ASSERT(DESCRIPTION)

//! assert parameter
#define STAFF_ASSERT_PARAM(EXPRESSION) \
  if (!(EXPRESSION)) STAFF_THROW_ASSERT("Invalid argument: (" #EXPRESSION ")")

//! debug assert expression
#ifdef _DEBUG
#define STAFF_DEBUG_ASSERT(EXPRESSION, DESCRIPTION) \
  if (!(EXPRESSION)) \
    STAFF_THROW_ASSERT(DESCRIPTION + std::string(". Debug assertion failed: (" #EXPRESSION ")"))
#else
#define STAFF_DEBUG_ASSERT(EXPRESSION, DESCRIPTION)
#endif

//! catch all exceptions
#define STAFF_CATCH_ALL\
  catch (const ::staff::Exception& rException)\
  {\
    ::staff::LogError() << "Exception: \n" << rException.what();\
  }\
  catch (const std::exception& rException) \
  {\
    ::staff::LogError() << "std::exception: \n" << rException.what();\
  }\
  catch (...) \
  {\
    ::staff::LogError() << "unknown exception"; \
  }

//! catch all exceptions and write description
#define STAFF_CATCH_ALL_DESCR(DESCRIPTION)\
  catch (const ::staff::Exception& rException)\
  {\
    ::staff::LogError() << (DESCRIPTION) << "\nException: \n" << rException.what();\
  }\
  catch (const std::exception& rException) \
  {\
    ::staff::LogError() << (DESCRIPTION) << "\nstd::exception: \n" << rException.what();\
  }\
  catch (...) \
  {\
    ::staff::LogError() << (DESCRIPTION) << "\nunknown exception"; \
  }

namespace staff
{

  //! base staff exception class
  class STAFF_UTILS_EXPORT Exception: public std::exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    Exception(const char* szFileLine, const char* szFunction, const std::string& sDescr);

    //! destructor
    inline virtual ~Exception() throw()
    {
    }

    //! get description of exception thrown
    /*! \return description
      */
    inline virtual const char* what() const throw()
    {
      return m_sDescr.c_str();
    }

    //! get description of exception thrown
    /*! \return description
      */
    inline virtual const std::string& strWhat() const throw()
    {
      return m_sDescr;
    }

  private:
    std::string m_sDescr; //!< description
  };

  //! assert exception class
  class AssertException: public Exception
  {
  public:
    //! exception constructor
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function signature
        \param  sDescr - description
      */
    inline AssertException(const char* szFileLine, const char* szFunction, const std::string& sDescr):
      Exception(szFileLine, szFunction, sDescr)
    {
    }
  };

}

#endif // _STAFF_UTILS_EXCEPTION_H_
