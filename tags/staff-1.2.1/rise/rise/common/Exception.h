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

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <rise/os/oscommon.h>
#include <rise/common/excodes.h>
#include <string>
#include <rise/string/String.h>

namespace rise 
{
  //!        abstract class for exceptions
  class RISE_EXPORT CException
  {
  public:
    //!        destructor
    virtual ~CException() throw() {}
  
    //!        creating an exception
    /*! \param  sFile - source file name
        \param  nLine - source file line
        \param  sFunction - function name
        \param  sDescr - description
        \param  sExpr - expression
        \param  sObject - object
    */
    virtual void Create(const CString& sFile, int nLine, const CString& sFunction, 
      const CString& sDescr = "", const CString& sExpr = "", 
      const CString& sObject = "") throw() = 0;
  
    //!        get file name
    /*! \return source file name
    */
    virtual const CString& GetFileName() const throw() = 0;

    //!        get source file line
    /*! \return source file line
    */
    virtual int GetLine() const throw() = 0;

    //!        get function
    /*! \return function
    */
    virtual const CString& GetFunction() const throw() = 0;

    //!        get description
    /*! \return description
    */
    virtual const CString& GetDescr() const throw() = 0;

    //!        get assert expression
    /*! \return assert expression
    */
    virtual const CString& GetExpr() const throw() = 0;

    //!        get object
    /*! \return object
    */
    virtual const CString& GetObjectName() const throw() = 0;
    
    //!        get exception class
    /*! \return exception class
    */
    virtual EXCLASS GetClass() const throw() = 0;
  
    //!        get exception code
    /*! \return exception code
    */
    virtual EXCODE GetCode() const throw() = 0;

    //!        get full description of exception
    /*! \param  sFormat - output exception format
        \return exception description
    */
    virtual CString GetString(const CString& sFormat = "") const throw() = 0;

  };  // class CException ///////////////////////////////////////////

}  // namespace rise

#endif // #ifndef _EXCEPTION_H_

