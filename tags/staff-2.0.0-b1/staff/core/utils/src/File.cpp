/*
 *  Copyright 2012 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <fnmatch.h>
#include <dirent.h>
#endif
#include "Exception.h"
#include "File.h"

#ifdef WIN32
#define stat _stat
#endif

namespace staff
{
  File::File(const std::string& sPath):
    m_sPath(sPath)
  {
  }


  void File::List(StringList& rList, const std::string& sMask /*= "*" */,
                  int nAttrs /*= AttributeAnyFile | AttributeDirectory */)
  {
    rList.clear();
#ifdef WIN32
    _finddata_t stSearchData;         // search record
    intptr_t nFile = _findfirst((m_sPath + "\\" + sMask).c_str(), &stSearchData);

    if (nFile != -1)
    {
      do
      {
        if (!IsDots(stSearchData.name) &&
            (((nAttrs & AttributeDirectory) != 0)
                && (stSearchData.attrib & _A_SUBDIR) != 0) ||
            ((nAttrs & AttributeAnyFile) != 0))
        {
          rList.push_back(stSearchData.name);
        }
      }
      while (!_findnext(nFile, &stSearchData));

      _findclose(nFile);
    }
#else
    DIR* pDir = opendir(m_sPath.c_str());
    if (pDir)
    {
      struct stat stStat;
      dirent* pstDirent = NULL;
      unsigned nMask =
          ((nAttrs & AttributeDirectory) ? S_IFDIR : 0) |
          ((nAttrs & AttributeRegularFile) ? S_IFREG : 0) |
          ((nAttrs & AttributeOtherFile) ? (S_IFMT & (~(S_IFREG | S_IFDIR))) : 0);

      while((pstDirent = readdir(pDir)) != NULL)
      {
        if (!IsDots(pstDirent->d_name) &&
            !fnmatch(sMask.c_str(), pstDirent->d_name, 0) &&
            !lstat(((m_sPath + "/") + pstDirent->d_name).c_str(), &stStat) &&
            (stStat.st_mode & nMask) == (stStat.st_mode & S_IFMT))
        {
          rList.push_back(pstDirent->d_name);
        }
      }

      closedir(pDir);
    }
#endif
  }

  int File::GetAttributes()
  {
    int nResult = AttributeNone;

    struct stat stStat;
    if (!stat(m_sPath.c_str(), &stStat))
    {
      if ((stStat.st_mode & S_IFDIR) != 0)
      {
        nResult |= AttributeDirectory;
      }
      else
      if ((stStat.st_mode & S_IFREG) != 0)
      {
        nResult |= AttributeRegularFile;
      }
      else
      {
        nResult |= AttributeOtherFile;
      }
    }
    return nResult;
  }

  bool File::IsExists()
  {
    return GetAttributes() != AttributeNone;
  }

  bool File::IsDirectory()
  {
    return (GetAttributes() & AttributeDirectory) != 0;
  }

  bool File::IsRegularFile()
  {
    return (GetAttributes() & AttributeRegularFile) != 0;
  }

  bool File::IsOtherFile()
  {
    return (GetAttributes() & AttributeOtherFile) != 0;
  }

  bool File::IsAnyFile()
  {
    return (GetAttributes() & AttributeAnyFile) != 0;
  }

  int File::GetTime()
  {
    struct stat stStat;
    if (stat(m_sPath.c_str(), &stStat))
    {
      return 0;
    }

    return static_cast<int>(stStat.st_mtime);
  }

  bool File::Mkdir()
  {
    int nRes =
#ifndef WIN32
        mkdir(m_sPath.c_str(), 0755);
#else
        _mkdir(m_sPath.c_str());
#endif
    STAFF_ASSERT(nRes != -1 || errno == EEXIST, "Failed to create dir ["
                   + m_sPath + "]: " + strerror(errno));

    return !nRes;
  }

  bool File::Mkdirs()
  {
    int nRes = -1;
    for (std::string::size_type nPos = 0; nPos != std::string::npos;)
    {
      nPos = m_sPath.find_first_of("/\\", nPos + 1);
      const std::string& sCurrDir = m_sPath.substr(0, nPos);
      if (!sCurrDir.empty())
      {
        nRes =
#ifndef WIN32
            mkdir(sCurrDir.c_str(), 0755);
#else
            _mkdir(sCurrDir.c_str());
#endif
        STAFF_ASSERT(nRes != -1 || errno == EEXIST, "Failed to create dir ["
                     + sCurrDir + "]: " + strerror(errno));
      }
    }

    return !nRes;
  }

  bool File::IsDots(const char* szName)
  {
    return (szName == NULL || (szName[0] == '.' && (szName[1] == '\0' ||
                                                    (szName[1] == '.' && szName[2] == '\0'))));
  }

  bool File::IsDots(const std::string& sName)
  {
    return IsDots(sName.c_str());
  }

}
