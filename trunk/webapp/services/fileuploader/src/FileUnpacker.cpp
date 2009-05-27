#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include "FileUnpacker.h"

namespace webapp
{
  CFileUnpacker::CFileUnpacker():
    m_nStatus(EUnknown)
  {
  }
  
  void CFileUnpacker::Start(const std::string& sUnpackCmd)
  {
    m_sUnpackCmd = sUnpackCmd;
    rise::threading::CThread::Start();
  }
  
  int CFileUnpacker::GetStatus()
  {
    return m_nStatus;
  }

  void CFileUnpacker::Run(void*)
  {
    int nStatus = -2;
    rise::LogDebug() << "Unpacking: [" << m_sUnpackCmd << "]";
    RISE_ASSERTES(m_tUnpackProc.Exec(m_sUnpackCmd), rise::CLogicNoItemException, "Cannot start unpack process: \"" + m_sUnpackCmd + "\"");
    
    m_nStatus = ERunning;

    while (m_nStatus == ERunning)
    {
      nStatus = m_tUnpackProc.GetExitStatus(true);

      if (nStatus == -2)
      { // some error
        m_nStatus = EGenericError;
      }
      else
      if (nStatus != -1)
      { // finished
        m_nStatus = WIFEXITED(nStatus) ? WEXITSTATUS(nStatus) : EGenericError;
      }
    }
  }
  
}

