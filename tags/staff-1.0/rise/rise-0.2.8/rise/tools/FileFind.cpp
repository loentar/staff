#ifdef WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#endif
#include <rise/common/Log.h>
#include "FileFind.h"

namespace rise
{
  void CFileFind::Find( const CString& sInDir, std::list<CString>& rList, const CString& sMask /*= "*"*/, int nAttrs /*= EFA_FILE | EFA_DIR*/ )
  {
    rList.clear();
#ifdef WIN32
    intptr_t nFile; 
    _finddata_t stSearchData;         // search record

    nFile = _findfirst((sInDir + "\\" + sMask).c_str(), &stSearchData);
    if( nFile != -1 )
      do
      {
        if( ((nAttrs & EFA_FILE) != 0 && (stSearchData.attrib & _A_SUBDIR) == 0) ||
            ((nAttrs & EFA_DIR) != 0 && (stSearchData.attrib & _A_SUBDIR) != 0) && !IsDots(stSearchData.name) )
          rList.push_back(stSearchData.name);
      }
      while (!_findnext(nFile, &stSearchData));

      _findclose(nFile);
#else
    DIR* pDir = opendir(sInDir.c_str());
    if(pDir != NULL)
    {
      struct stat stStat;
      dirent* pstDirent = NULL;

      while((pstDirent = readdir(pDir)) != NULL)
      {
        lstat(((sInDir + "/") + pstDirent->d_name).c_str(), &stStat);
        if( (fnmatch(sMask.c_str(), pstDirent->d_name, 0) == 0) &&
              ( ((nAttrs & EFA_FILE) != 0 && (stStat.st_mode & S_IFDIR ) == 0) ||
                ((nAttrs & EFA_DIR)  != 0 && (stStat.st_mode & S_IFDIR ) != 0) && !IsDots(pstDirent->d_name) ) )
          rList.push_back(pstDirent->d_name);
      }

      closedir(pDir);
    } else
    {
      LogWarning() << "can't open dir " << sInDir << ": " << strerror(errno);
    }
#endif
  }

  bool CFileFind::IsDots( const char* szName )
  {
    return (szName == NULL || (szName[0] == '.' && (szName[1] == '\0' || (szName[1] == '.' && szName[2] == '\0'))));
  }

}

