#ifndef _STACKTRACER_H_
#define _STACKTRACER_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <rise/tools/AddrInfo.h>
#include <list>

namespace rise
{
  namespace tools
  {
    typedef std::list<SAddrInfo> TAddrInfoList; //!<  результат трассировки стека

    //!  трассировщик стека
    class RISE_EXPORT CStackTracer
    {
    public:
      //!         произвести трассировку стека
      /*! \param  sTraceResult - строковый результат трассировки стека
          \param  nSkip - пропустить nSkip верхних уровней стека
          */
      static void StackTraceStr(CString& sTraceResult, int nSkip = 0);

      //!         произвести трассировку стека
      /*! \param  rTraceResult - результат трассировки стека
          \param  nSkip - пропустить nSkip верхних уровней стека
          */
      static void StackTrace(TAddrInfoList& rTraceResult, int nSkip = 0);
    };
  }
}

#endif // _STACKTRACER_H_
