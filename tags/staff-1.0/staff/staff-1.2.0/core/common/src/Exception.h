#ifndef _STAFF_EXCEPTION_H_
#define _STAFF_EXCEPTION_H_

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/excodes.h>

namespace staff
{
  enum EXCLASS
  {
    EXCFIRST = rise::EXCLAST,
    EXCDOM,
    EXCREMOTE
  };

  enum EXCODE
  {
    EXFIRST = rise::EXTEMPDECISION,
    EXPARAM,
    EXTYPE,
    EXNOITEM,
    EXFORMAT,
    EXINTERNAL
  };

  class CDomException: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCDOM))>
  {
  };


  template<EXCODE CODE>
  class CDomExceptionTemplate: public CDomException
  {
  public:
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };



  class CRemoteExceptionBase: public rise::CExceptionTemplate<static_cast<rise::EXCLASS>(static_cast<int>(EXCREMOTE))>
  {
  };


  template<EXCODE CODE>
  class CRemoteExceptionTemplate: public CRemoteExceptionBase
  {
  public:
    rise::EXCODE GetCode() const throw() { return static_cast<rise::EXCODE>(static_cast<int>(CODE)); }  
  };

  typedef CDomExceptionTemplate<EXPARAM> CDomParamException;
  typedef CDomExceptionTemplate<EXTYPE> CDomTypeException;
  typedef CDomExceptionTemplate<EXNOITEM> CDomNoItemException;
  typedef CDomExceptionTemplate<EXFORMAT> CDomFormatException;
  typedef CDomExceptionTemplate<EXINTERNAL> CDomInternalException;

  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteException;
  typedef CRemoteExceptionTemplate<EXINTERNAL> CRemoteInternalException;
}

#endif // _STAFF_EXCEPTION_H_
