#ifndef _CONTAINERTYPES_H_
#define _CONTAINERTYPES_H_

#include <vector>
#include <list>
#include <sstream>
#include <iostream>
#include <rise/string/String.h>
#include "types.h"

namespace rise 
{
  //! массив байт
  typedef std::vector<byte> CByteArray;
  
  //! список строк
  typedef std::list<CString> CStringList;
}

#endif // _CONTAINERTYPES_H_
