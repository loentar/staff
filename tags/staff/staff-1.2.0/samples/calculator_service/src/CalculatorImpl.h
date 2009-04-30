#ifndef _CALCULATORIMPL_H_
#define _CALCULATORIMPL_H_

#include "Calculator.h"

namespace samples
{
  //! service implementation
  class CCalculatorImpl: public CCalculator
  {
  public:
    virtual int Add(int nA, int nB);
    virtual int Sub(int nA, int nB);
    virtual staff::CDataObject MulDO(const staff::CDataObject& rDataObject);
  };
}

#endif // _CALCULATORIMPL_H_
