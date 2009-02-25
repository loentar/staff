#ifndef _EXMACROS_H_
#define _EXMACROS_H_

#include <exception>

#define RISE_THROW1(RISE_EXCEPTION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, #RISE_EXCEPTION); \
    throw cException;}

#define RISE_THROW2(RISE_EXCEPTION, RISE_DESCRIPTION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, RISE_DESCRIPTION); \
    throw cException;}

#define RISE_THROW3(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION)\
  {RISE_EXCEPTION cException; \
    cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, RISE_DESCRIPTION, RISE_EXPRESSION); \
    throw cException;}

//! генерация исключения
#define RISE_THROW(RISE_EXCEPTION) \
  RISE_THROW1(RISE_EXCEPTION)

//! генерация исключения со строкой описания
#define RISE_THROWS(RISE_EXCEPTION, RISE_DESCRIPTION) \
  RISE_THROW2(RISE_EXCEPTION, RISE_DESCRIPTION)

//! генерация исключения с выражением
#define RISE_THROWE(RISE_EXCEPTION, RISE_EXPRESSION) \
  RISE_THROW3(RISE_EXCEPTION, #RISE_EXCEPTION, #RISE_EXPRESSION)

//! генерация исключения с выражением и строкой описания
#define RISE_THROWES(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION) \
  RISE_THROW3(RISE_EXCEPTION, RISE_DESCRIPTION, #RISE_EXPRESSION)

//////////////////////////////////////////////////////////////////////////
//!  ASSERT

//! проверка утверждения, генерация исключения CAssertException, если утверждение ложно
#define RISE_ASSERT(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalAssertException)

#define RISE_ASSERTS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTES(RISE_EXPRESSION, rise::CInternalAssertException, RISE_DESCRIPTION)

//! проверка утверждения, генерация исключения CLogicParamException, если утверждение ложно
#define RISE_ASSERTP(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CLogicParamException)

#define RISE_ASSERTPS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTES(RISE_EXPRESSION, rise::CLogicParamException, RISE_DESCRIPTION)

//! проверка утверждения, генерация исключения CNotImplementedException, если утверждение ложно
#define RISE_ASSERTN(RISE_EXPRESSION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalNotImplementedException)

#define RISE_ASSERTNS(RISE_EXPRESSION, RISE_DESCRIPTION) \
  RISE_ASSERTE(RISE_EXPRESSION, rise::CInternalNotImplementedException, RISE_DESCRIPTION)


//! проверка утверждения. генерация заданного исключения если утверждение ложно
#define RISE_ASSERTE(RISE_EXPRESSION, RISE_EXCEPTION) \
  if (!(RISE_EXPRESSION)) \
    RISE_THROWE(RISE_EXCEPTION, RISE_EXPRESSION);

//! проверка утверждения. генерация заданного исключения со строкой описания если утверждение ложно
#define RISE_ASSERTES(RISE_EXPRESSION, RISE_EXCEPTION, RISE_DESCRIPTION) \
 if (!(RISE_EXPRESSION)) RISE_THROWES(RISE_EXCEPTION, RISE_DESCRIPTION, RISE_EXPRESSION);

//    СБОР ИСКЛЮЧЕНИЙ

//! перехват исключений с описанием и действием в случае исключения
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
 

//! перехват исключений без описания
#define RISE_CATCH_ALL \
  RISE_CATCH_ALL_DESCR_ACTION("",)

//! перехват исключений с описанием
#define RISE_CATCH_ALL_DESCR(RISE_DESCRIPTION) \
  RISE_CATCH_ALL_DESCR_ACTION(RISE_DESCRIPTION,)

#endif // _EXMACROS_H_
