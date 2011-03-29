#ifndef _DepSvc1_H_
#define _DepSvc1_H_

#include <staff/common/IService.h>

namespace samples
{
namespace loadorder
{
  //! DepSvc1 service
  // *loadAtStartup: true
  // *dependencies: samples.loadorder.DepSvc3
  class DepSvc1: public staff::IService
  {
  };
}
}

#endif // #ifndef _DepSvc1_H_
