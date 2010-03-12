#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <rise/string/String.h>
#include <staff/common/DataObject.h>

namespace samples
{
  //! interface for service Calculator
  class CCalculator
  {
  public:
    virtual ~CCalculator() {}

    //! addition
    /*! \param nA - first operand
        \param nB - second operand
        \return nA + nB
        */
    virtual int Add(int nA, int nB) = 0;

    //! substraction
    /*! \param nA - first operand
        \param nB - second operand
        \return nA - nB
        */
    virtual int Sub(int nA, int nB) = 0;

    //! add
    /*! \param rDataObject - reference to dataobject, containing operands nA and nB
        \return nA * nB
        */
    virtual staff::CDataObject MulDO(const staff::CDataObject& rDataObject) = 0;
  };
}

#endif // _CALCULATOR_H_
