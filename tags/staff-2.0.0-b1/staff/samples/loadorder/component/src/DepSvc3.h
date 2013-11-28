#ifndef _DepSvc3_H_
#define _DepSvc3_H_

#include <staff/common/IService.h>

namespace samples
{
namespace loadorder
{
  //! DepSvc3 service
  // *loadAtStartup: true
  // *dependencies: samples.loadorder.DepSvc4
  class DepSvc3: public staff::IService
  {
  };
}
}

#endif // #ifndef _DepSvc3_H_
