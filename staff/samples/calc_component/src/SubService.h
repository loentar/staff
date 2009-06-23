#ifndef _SUBSERVICE_H_
#define _SUBSERVICE_H_

namespace samples
{
  namespace calc
  {
    //!  sample service, implementing substract operation
    class CSubService
    {
    public:
      //!        destructor
      virtual ~CSubService() {}
      virtual int Sub(int nA, int nB) const = 0;
    };
  }
}

#endif // _SUBSERVICE_H_
