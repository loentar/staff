#ifndef _STRING_HPP_
#define _STRING_HPP_

#include <rise/common/streamtypes.h>

namespace rise
{
  
  template<class TData>
  TData& FromStr(const CString& sData, TData& tResult)
  {
    CIStringStream(sData) >> tResult;
    return tResult;
  }

  template<class TData>
  CString ToStr(const TData& rtData)
  {
    COStringStream tOStringStream;
    tOStringStream << rtData;
    return tOStringStream.str().c_str();
  }

  template<class TData>
  void ToStr(const TData& rtData, CString& sResult)
  {
    COStringStream tOStringStream;
    tOStringStream << rtData;
    sResult = tOStringStream.str().c_str(); // иначе не совпадет длинна
  }

} // namespace rise

#endif // _STRING_HPP_

