#ifndef _CALCULATOR_H_
#define _CALCULATOR_H_

#include <rise/string/String.h>
#include <staff/common/DataObject.h>

namespace samples
{
  //! абстрактный класс сервиса - от него будет наследоваться реализация и прокси, на основе него генерируется упаковщик
  class CCalculator
  {
  public:
    virtual ~CCalculator() {}

    virtual rise::CString Test() const = 0;
    virtual int Add(int nA, int nB) = 0;
    virtual int Sub(int nA, int nB) = 0;
    virtual staff::CDataObject MulDO(const staff::CDataObject& rDataObject) = 0;
  };
}

#endif // _CALCULATOR_H_
