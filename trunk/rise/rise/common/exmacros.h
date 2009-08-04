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

#ifndef _EXMACROS_H_
#define _EXMACROS_H_

#include <exception>

//! throw exception 1 arg
#define RISE_THROW1(RISE_EXCEPTION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, #RISE_EXCEPTION); \
    throw cException;}

//! throw exception 2 arg
#define RISE_THROW2(RISE_EXCEPTION, RISE_DESCRIPTION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, RISE_DESCRIPTION); \
    throw cException;}

//! throw exception 3 arg
#define RISE_THROW3(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, RISE_DESCRIPTION, RISE_EXPRESSION); \
    throw cException;}

//! throw exception
#define RISE_THROW(RISE_EXCEPTION) \
  RISE_THROW1(RISE_EXCEPTION)

//! throw exception with description
#define RISE_THROWS(RISE_EXCEPTION, RISE_DESCRIPTION) \
  RISE_THROW2(RISE_EXCEPTION, RISE_DESCRIPTION)

//! throw exception with expression
#define RISE_THROWE(RISE_EXCEPTION, RISE_EXPRESSION) \
  RISE_THROW3(RISE_EXCEPTION, #RISE_EXCEPTION, #RISE_EXPRESSION)

//! throw exception with description and expression
#define RISE_THROWES(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION) \
  RISE_THROW3(RISE_EXCEPTION, RISE_DESCRIPTION, #RISE_EXPRESSION)

//////////////////////////////////////////////////////////////////////////
//!  ASSERT

//! check for expression, throw CAssertException, if expression false
#define RISE_ASSERT(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalAssertException)

//! check for expression, throw CAssertException with description, if expression false
#define RISE_ASSERTS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTES(RISE_EXPRESSION, rise::CInternalAssertException, RISE_DESCRIPTION)

//! check for expression, throw CLogicParamException, if expression false
#define RISE_ASSERTP(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CLogicParamException)

//! check for expression, throw CLogicParamException with description, if expression false
#define RISE_ASSERTPS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTES(RISE_EXPRESSION, rise::CLogicParamException, RISE_DESCRIPTION)

//! check for expression, throw CNotImplementedException, if expression false
#define RISE_ASSERTN(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalNotImplementedException)

//! check for expression, throw CNotImplementedException with description, if expression false
#define RISE_ASSERTNS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalNotImplementedException, RISE_DESCRIPTION)


//! check for expression, throw given exception, if expression false
#define RISE_ASSERTE(RISE_EXPRESSION, RISE_EXCEPTION) \
  if (!(RISE_EXPRESSION)) \
    RISE_THROWE(RISE_EXCEPTION, RISE_EXPRESSION);

//! check for expression, throw given exception with description, if expression false
#define RISE_ASSERTES(RISE_EXPRESSION, RISE_EXCEPTION, RISE_DESCRIPTION) \
 if (!(RISE_EXPRESSION)) RISE_THROWES(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION);

//    exception catching

//! catch all exception, make description and some action
#define RISE_CATCH_ALL_DESCR_ACTION(RISE_DESCRIPTION, RISE_ACTION) \
  catch (const rise::CException& rException) \
  {\
    RISE_ACTION; \
    rise::LogError() << RISE_DESCRIPTION << "CException: \n" << rException.GetString(); \
  }\
  catch (const std::exception& rException) \
  {\
    RISE_ACTION; \
    rise::LogError() << RISE_DESCRIPTION << "std::exception: \n" << rException.what(); \
  }\
  catch (int nException) \
  {\
    RISE_ACTION; \
    rise::LogError() << RISE_DESCRIPTION << "int exception: \n" << nException; \
  }\
  catch (...) \
  {\
    RISE_ACTION; \
    rise::LogError() << RISE_DESCRIPTION << "unknown exception"; \
  }
 

//! catch all exceptions
#define RISE_CATCH_ALL \
  RISE_CATCH_ALL_DESCR_ACTION("",)

//! catch all exceptions and give description
#define RISE_CATCH_ALL_DESCR(RISE_DESCRIPTION) \
  RISE_CATCH_ALL_DESCR_ACTION(RISE_DESCRIPTION,)

#endif // _EXMACROS_H_
