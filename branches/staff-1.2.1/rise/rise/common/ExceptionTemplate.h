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

#ifndef _EXCEPTIONTEMPLATE_H_
#define _EXCEPTIONTEMPLATE_H_

#include <rise/os/osdllexport.h>
#include <rise/common/Exception.h>
#include <rise/common/excodes.h>
#include <rise/common/exmacros.h>
#include <rise/string/String.h>

namespace rise
{
  //!        template exceptions class 
  template<int CLASS>
    class CExceptionTemplate: public CException
  {
    public:
      //!        destructor
      virtual ~CExceptionTemplate() throw();

      //!        creating an exception
      /*! \param  sFile - source file name
          \param  nLine - source file line
          \param  sFunction - function name
          \param  sDescr - description
          \param  sExpr - expression
          \param  sObject - object
      */
      void Create(const CString& sFile, int nLine, const CString& sFunction, 
        const CString& sDescr = "", const CString& sExpr = "", 
        const CString& sObject = "") throw();
      
      //!        copy operator
      /*! \return reference to *this
      */
      CExceptionTemplate& operator=(const CExceptionTemplate& rException);
      
      //!        get file name
      /*! \return source file name
      */
      const CString& GetFileName() const throw();
      
      //!        get source file line
      /*! \return source file line
      */
      int GetLine() const throw();
      
      //!        get function
      /*! \return function
      */
      const CString& GetFunction() const throw();
      
      //!        get description
      /*! \return description
      */
      const CString& GetDescr() const throw();
      
      //!        get assert expression
      /*! \return assert expression
      */
      const CString& GetExpr() const throw();
      
      //!        get object
      /*! \return object
      */
      const CString& GetObjectName() const throw();

      //!        set object name
      /*! \param sObjectName - object name
      */
      void SetObjectName(const CString& sObjectName) throw();
      
      //!        get exception class
      /*! \return exception class
      */
      EXCLASS GetClass() const throw();

      //!        get full description of exception
      /*! \sa SetFormat
          \param  sFormat - format for exception
          \return exception description
      */
      CString GetString(const CString& sFormat = "") const throw();

      //!        set exception string format
      /*! 
         supported substitution symbols:
         {File}   - source file name
         {Line}   - source file line
         {Func}   - function
         {Descr}  - description
         {Expr}   - assert expression
         {Class}  - exception class (file, logic, internal)
         {Code}   - exception code
         {Object} - exception object (description string)
         {Stack}  - call stack
         
         call stack is available only if environment variable RISE_EXCEPTION_STACKTRACING=1
         
         \sa GetString
         \param  sFormat - format for exception
      */
      virtual void SetFormat(CString& sFormat) const throw();

    private:
      CString  m_sFile;           //!< source file name
      int      m_nLine;           //!< source file line
      CString  m_sFunction;       //!< function
      CString  m_sExpr;           //!< assert expression
      CString  m_sDescr;          //!< description
      CString  m_sObject;         //!< exception object
      CString  m_sStack;          //!< call stack
      CString  m_sFormat;
      bool     m_bStackTracing;
  }; // class CExceptionTemplate

  //!        base class for file exceptions
  class RISE_EXPORT CFileException: public CExceptionTemplate<EXCFILE>
  {
  };

  //!        base template class for file exceptions
  template<int CODE>
  class RISE_EXPORT CFileExceptionTemplate: public CFileException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        base class for logic exceptions
  class RISE_EXPORT CLogicException: public CExceptionTemplate<EXCLOGIC>
  {
  };

  //!        base template class for logic exceptions
  template<int CODE>
  class RISE_EXPORT CLogicExceptionTemplate: public CLogicException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        base class for internal exceptions
  class RISE_EXPORT CInternalException: public CExceptionTemplate<EXCINTERNAL>
  {
  };

  //!        base template class for internal exceptions
  template<int CODE>
  class RISE_EXPORT CInternalExceptionTemplate: public CInternalException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        error while creating file
  typedef CFileExceptionTemplate<EXCREATE>             CFileCreateException;

  //!        error while deleting file
  typedef CFileExceptionTemplate<EXDELETE>             CFileDeleteException;

  //!        error while opening file
  typedef CFileExceptionTemplate<EXOPEN>               CFileOpenException;

  //!        error while closing file
  typedef CFileExceptionTemplate<EXCLOSE>              CFileCloseException;

  //!        error while reading file
  typedef CFileExceptionTemplate<EXREAD>               CFileReadException;

  //!        error while write to file
  typedef CFileExceptionTemplate<EXWRITE>              CFileWriteException;

  //!        error while file I/O
  typedef CFileExceptionTemplate<EXIO>                 CFileIOException;


  //!        error: not initialized
  typedef CLogicExceptionTemplate<EXNOINIT>            CLogicNoInitException;

  //!        error: already initialized
  typedef CLogicExceptionTemplate<EXALREADYEXISTS>     CLogicAlreadyExistsException;

  //!        error: invalid size
  typedef CLogicExceptionTemplate<EXSIZE>              CLogicSizeException;

  //!        error: item not found
  typedef CLogicExceptionTemplate<EXNOITEM>            CLogicNoItemException;

  //!        error: timeout
  typedef CLogicExceptionTemplate<EXTIMEOUT>           CLogicTimeoutException;

  //!        error: permission 
  typedef CLogicExceptionTemplate<EXNOPERMISSION>      CLogicPermissionException;

  //!        error: invalid format
  typedef CLogicExceptionTemplate<EXFORMAT>            CLogicFormatException;

  //!        error: invalid function params
  typedef CLogicExceptionTemplate<EXPARAM>             CLogicParamException;


  //!        internal error
  typedef CInternalExceptionTemplate<EXINTERNAL>       CInternalInternalException;

  //!        error: insufficient memory
  typedef CInternalExceptionTemplate<EXNOMEM>          CInternalNoMemException;

  //!        assert error
  typedef CInternalExceptionTemplate<EXASSERT>         CInternalAssertException;

  //!        error: function is not implemented
  typedef CInternalExceptionTemplate<EXNOTIMPLEMENTED> CInternalNotImplementedException;

  //!        error: temporary decision was reached
  typedef CInternalExceptionTemplate<EXTEMPDECISION>   CInternalTempDecisionException;

}  // namespace rise

#include "ExceptionTemplate.hpp"

#endif // #ifndef _EXCEPTIONTEMPLATE_H_
