#include <staff/common/Value.h>
#include "CalculatorImpl.h"

int CCalculatorImpl::Add( int nA, int nB )
{
  return nA + nB;
}

int CCalculatorImpl::Sub( int nA, int nB )
{
  return nA - nB;
}

staff::CDataObject CCalculatorImpl::MulDO( const staff::CDataObject& rDataObject )
{
  staff::CDataObject tResult("MulDOResult");
  tResult.Value() = (int)rDataObject["nA"] * (int)rDataObject["nB"];
  return tResult;
}
