#ifndef _FILEUNPACKER_H_
#define _FILEUNPACKER_H_

#include <string>
#include <rise/threading/Thread.h>
#include <rise/process/Process.h>

namespace webapp
{
  //!  File unpacker
  class CFileUnpacker: rise::threading::CThread
  {
  public:
    enum EStatus
    {
      EUnknown = -3,
      EGenericError = -2,
      ERunning = -1,
      ESuccess = 0
    };
  
  public:
    CFileUnpacker();
    void Start(const std::string& sUnpackCmd);
    int GetStatus();
    
  private:
    void Run(void*);

  private:
    int m_nStatus;
    rise::process::CProcess m_tUnpackProc;
    std::string m_sUnpackCmd;
  };
}

#endif // _FILEUNPACKER_H_
