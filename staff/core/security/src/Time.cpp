#include <time.h>
#include "Time.h"

namespace staff
{
  namespace security
  {
    int CTime::Get()
    {
      return static_cast<int>(time(NULL));
    }
  }
}
