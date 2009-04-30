#ifndef _FILEFIND_H_
#define _FILEFIND_H_

#include <rise/os/osdllexport.h>
#include <list>
#include <rise/string/String.h>

namespace rise
{
  class RISE_EXPORT CFileFind
  {
  public:
    enum EFindAttrs
    {
      EFA_FILE = 1,
      EFA_DIR = 2
    };

  public:
    static void Find(const CString& sInDir, std::list<CString>& rList, const CString& sMask = "*", int nAttrs = EFA_FILE | EFA_DIR);
    static bool IsDots(const char* szName);
  };
}

#endif // _FILEFIND_H_

