#ifndef _CalcService_H_
#define _CalcService_H_

namespace samples
{
  namespace calc
  {
    class CCalcService
    {
    public:
      virtual ~CCalcService() {}

      virtual int /*addResult*/ Add(int nA, int nB) const = 0;
      virtual int Sub(int nA, int nB) const = 0;
      virtual void SetMem(int nMem) = 0;
      virtual int GetMem() const = 0;
    };
  }
}

#endif
