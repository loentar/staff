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

#ifndef _XMLEXCEPTION_H_
#define _XMLEXCEPTION_H_

#include <rise/common/ExceptionTemplate.h>

//! throw xml exception
#define THROWXML(RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
{ RISE_LOCAL_EXCEPTION cException; \
  cException.Create(__FILE__, __LINE__, RISE_PRETTY_FUNCTION, RISE_LOCAL_DESCRIPTION, \
                    #RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE); \
  throw cException;}

//! assert xml
#define ASSERTXMLS(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
  if (!(RISE_LOCAL_EXPRESSION)) \
    THROWXML(RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE)

//! assert
#define ASSERTXML(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
  ASSERTXMLS(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE)

namespace rise
{
  namespace xml
  {
    //!        base class for xml exceptions
    class CXMLException: public CExceptionTemplate<EXCXML>
    {
    };

    //!        template class for xml exceptions
    template<EXCODE CODE>
    class CXMLExceptionTemplate: public CXMLException
    {
      public:
        EXCODE GetCode() const throw() 
        { 
          return CODE; 
        };

        //!        creating an exception
        /*! \param  szFile - source filename
            \param  nLine - source line name
            \param  szFunction - function name
            \param  szDescr - exception description
            \param  szExpr - expression
            \param  szFileName - xml file name
            \param  uLineNo - xml file line number
        */
        void Create( 
          const CString& szFile, 
          int nLine, 
          const CString& szFunction, 
          const CString& szDescr = "", 
          const CString& szExpr = "", 
          const CString& szFileName = "", 
          unsigned uLineNo = -1) throw()
        {
          CXMLException::Create(szFile, nLine, szFunction, szDescr, szExpr, szFileName);
          m_uLineNo = uLineNo;
        }

        //!         get xml file line number
        /*! \return xml file line number
        */
        virtual const int GetLineNo() const throw()
        {
          return m_uLineNo;
        }

        //!         set string format
        /*! \param  sFormat - string format
            */
        virtual void SetFormat( CString& sFormat ) const throw()
        {
          sFormat = "{File}[{Line}] {Func}: {Descr}({Expr}); {Object}: {LineNo}\n";
        }
        
        //!        get string exception description
        /*! \param  sFormat - format string
            \return exception description
        */
        CString GetString(const CString& sFormat = "") const throw()
        {
          CString sResult = CXMLException::GetString(sFormat);
          
          StrReplace(sResult, "{LineNo}", ToStr(GetLineNo()));
          
          return sResult;
        }
     
      private:
        unsigned m_uLineNo;  //!< xml file line number

    };  // class CXMLException ///////////////////////////////////////

    //! create exception
    typedef CXMLExceptionTemplate<EXCREATE> CXMLCreateException;
    //! open exception
    typedef CXMLExceptionTemplate<EXOPEN>   CXMLOpenException;
    //! close exception
    typedef CXMLExceptionTemplate<EXCLOSE>  CXMLCloseException;
    //! read exception
    typedef CXMLExceptionTemplate<EXREAD>   CXMLReadException;
    //! write exception
    typedef CXMLExceptionTemplate<EXWRITE>  CXMLWriteException;
    //! format exception
    typedef CXMLExceptionTemplate<EXFORMAT> CXMLFormatException;
    //! item not found exception
    typedef CXMLExceptionTemplate<EXNOITEM> CXMLNoItemException;

  } // namespace xml
} // namespace rise

#endif // #ifndef _XMLEXCEPTION_H_
