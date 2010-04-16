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

#ifndef _EXCEPTIONTEMPLATE_HPP_
#define _EXCEPTIONTEMPLATE_HPP_

#include <stdlib.h>
#include <string.h>
#include <rise/common/Log.h>
#include <rise/tools/StackTracer.h>


namespace rise
{
  template<int CLASS>
    CExceptionTemplate<CLASS>::~CExceptionTemplate() throw()
  {
  }

  template<int CLASS>
    void CExceptionTemplate<CLASS>::Create(const CString& sFile, int nLine, const CString& sFunction, 
      const CString& sDescr /*= NULL*/, const CString& sExpr /*= NULL*/,
      const CString& sObject /*= NULL*/) throw()
  {
    m_sFile = sFile;
    m_nLine = nLine;
    m_sFunction = sFunction;
    m_sDescr = sDescr;
    m_sExpr = sExpr;
    m_sObject = sObject;
    
    char* szStackTracingEnv = NULL;
#ifdef WIN32
    _dupenv_s(&szStackTracingEnv, NULL, "RISE_EXCEPTION_STACKTRACING");
#else
    szStackTracingEnv = getenv("RISE_EXCEPTION_STACKTRACING");
#endif
    m_bStackTracing = szStackTracingEnv != NULL && ((!strcmp(szStackTracingEnv, "1") || !strcmp(szStackTracingEnv, "TRUE")));

#ifdef WIN32
    free(szStackTracingEnv);
#endif

    if (m_bStackTracing)
    {
      rise::LogWarning() << "Exception at " << sFile << "[" << nLine << "]: " << sFunction << ": " << sDescr << " (" << sExpr << "/" << sObject << ")";
      tools::CStackTracer::StackTraceStr(m_sStack);
    }
  }
  
  template<int CLASS>
    CExceptionTemplate<CLASS>& CExceptionTemplate<CLASS>::operator=(const CExceptionTemplate& rException)
  {
    Create(rException.m_sFile, rException.m_nLine, rException.m_sFunction, rException.m_sDescr, 
           rException.m_sExpr, rException.m_sObject);
    return *this;
  }
  
  template<int CLASS>
    const CString& CExceptionTemplate<CLASS>::GetFileName() const throw() 
  { 
    return m_sFile; 
  }
  
  template<int CLASS>
    int CExceptionTemplate<CLASS>::GetLine() const throw() 
  { 
    return m_nLine; 
  }
  
  template<int CLASS>
    const CString& CExceptionTemplate<CLASS>::GetFunction() const throw() 
  { 
    return m_sFunction; 
  }
  
  template<int CLASS>
    const CString& CExceptionTemplate<CLASS>::GetDescr() const throw()
  { 
    return m_sDescr;
  }
  
  template<int CLASS>
    const CString& CExceptionTemplate<CLASS>::GetExpr() const throw()
  {
    return m_sExpr;
  }

      
  template<int CLASS>
    const CString& CExceptionTemplate<CLASS>::GetObjectName() const throw()
  {
    return m_sObject;
  }

  template<int CLASS>
    void CExceptionTemplate<CLASS>::SetObjectName(const CString& sObjectName) throw()
  {
    m_sObject = sObjectName;
  }
  
  template<int CLASS>
    EXCLASS CExceptionTemplate<CLASS>::GetClass() const throw()
  {
    return static_cast<EXCLASS>(CLASS);
  }

  template<int CLASS>
  void CExceptionTemplate<CLASS>::SetFormat( CString& sFormat ) const throw()
  {
    sFormat = "{File}[{Line}] {Func}: {Descr}({Expr}); {Object}";
    if(m_bStackTracing)
      sFormat += "\nStack:\n{Stack}";
  }

  template<int CLASS>
    CString CExceptionTemplate<CLASS>::GetString(const CString& sFormat /*= ""*/) const throw()
  {
    CString sResult;
    
    if (sFormat == "")
      SetFormat(sResult);
    else
      sResult = sFormat;

    StrReplace(sResult, "{File}",   GetFileName());
    StrReplace(sResult, "{Line}",   ToStr(GetLine()));
    StrReplace(sResult, "{Func}",   GetFunction());
    StrReplace(sResult, "{Descr}",  GetDescr());
    StrReplace(sResult, "{Expr}",   GetExpr());
    StrReplace(sResult, "{Class}",  ToStr(GetClass()));
    StrReplace(sResult, "{Code}",   ToStr(GetCode()));
    StrReplace(sResult, "{Object}", ToStr(GetObjectName()));
    if(m_bStackTracing)
      StrReplace(sResult, "{Stack}",  m_sStack);
    
    return sResult;
  }

}  // namespace rise


#endif // #ifndef _EXCEPTIONTEMPLATE_HPP_

