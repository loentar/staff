#ifndef _DepSvc2_H_
#define _DepSvc2_H_

#include <staff/common/IService.h>

namespace samples
{
namespace loadorder
{
  //! DepSvc2 service
  // *loadAtStartup: true
  // *dependencies: samples.loadorder.DepSvc1
  class DepSvc2: public staff::IService
  {
  };
}
}

#endif // #ifndef _DepSvc2_H_
