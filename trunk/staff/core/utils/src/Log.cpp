/*
 *  Copyright 2011 Utkin Dmitry
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
 *  See the License for the specif ic language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <fstream>
#include "tocstring.h"
#include "Thread.h"
#include "Process.h"
#include "console.h"
#include "Log.h"

namespace staff
{

  inline const char* BaseName(const char* szPath)
  {
    const char* szLast = szPath;
    while (*szPath)
    {
      if (*szPath == '/' || *szPath == '\\')
      {
        szLast = szPath + 1;
      }
      ++szPath;
    }
    return szLast;
  }

  Log& Log::Inst()
  {
    static Log tInst;
    return tInst;
  }

  Log::Log():
    m_eLevel(LogLevelInfo), m_nVerbosity(LogVerbosityDefault)
  {
    const char* szLogFile = getenv("STAFF_LOG_FILE");
    if (!szLogFile)
    {
      m_pStream = &std::cout;
    }
    else
    {
      const char* szLogFileAppend = getenv("STAFF_LOG_FILE_APPEND");
      static std::ofstream tOutStream;
      std::ios::openmode nFlags = std::ios::out;

      if (!!szLogFileAppend && (!strcmp(szLogFileAppend, "1") || !strcmp(szLogFileAppend, "TRUE")))
      {
        nFlags |= std::ios::app;
      }

      tOutStream.open(szLogFile, nFlags);

      if (tOutStream.good())
      {
        m_pStream = &tOutStream;
      }
      else
      {
        std::cerr << ColorBright << ColorTextBrown
                  << "Warning: cannot open Log file: \"" << szLogFile
                  << "\". using stdout..\n\n" << ColorDefault << std::endl;
        m_pStream = &std::cout;
      }
    }

    const char* szStaffLogLevel = getenv("STAFF_LOG_LEVEL");
    if (szStaffLogLevel)
    {
#define STAFF_CHECK_LOG_LEVEL(STAFF_LOG_LEVEL, STAFF_LOG_LEVEL_ENUM)\
      if (!strcmp(szStaffLogLevel, #STAFF_LOG_LEVEL))\
      {\
         m_eLevel = LogLevel##STAFF_LOG_LEVEL_ENUM;\
      }\
      else

      STAFF_CHECK_LOG_LEVEL(ALERT,Alert)
      STAFF_CHECK_LOG_LEVEL(CRIT,Crit)
      STAFF_CHECK_LOG_LEVEL(ERROR,Error)
      STAFF_CHECK_LOG_LEVEL(WARNING,Warning)
      STAFF_CHECK_LOG_LEVEL(NOTICE,Notice)
      STAFF_CHECK_LOG_LEVEL(INFO,Info)
      STAFF_CHECK_LOG_LEVEL(DEBUG,Debug)
      STAFF_CHECK_LOG_LEVEL(DEBUG1,Debug1)
      STAFF_CHECK_LOG_LEVEL(DEBUG2,Debug2)
      STAFF_CHECK_LOG_LEVEL(DEBUG3,Debug3)
      std::cerr << "staff:Log: invalid STAFF_LOG_LEVEL: [" << szStaffLogLevel
                << "]\nvalid values are (ALERT, CRIT, ERROR, WARNING, NOTICE,"
                   "INFO, DEBUG, DEBUG1, DEBUG2, DEBUG3)" << std::endl;
#undef STAFF_CHECK_LOG_LEVEL
    }

    const char* szStaffLogVerbosity = getenv("STAFF_LOG_VERBOSITY");
    if (szStaffLogVerbosity)
    {
      m_nVerbosity = 0;
#define STAFF_CHECK_LOG_VERBOSITY(STAFF_LOG_VERBOSITY, STAFF_LOG_VERBOSITY_ENUM)\
      if (strstr(szStaffLogVerbosity, #STAFF_LOG_VERBOSITY))\
      {\
        m_nVerbosity |= LogVerbosity##STAFF_LOG_VERBOSITY_ENUM;\
      }

      STAFF_CHECK_LOG_VERBOSITY(TEXT,Text)
      STAFF_CHECK_LOG_VERBOSITY(LEVEL,Level)
      STAFF_CHECK_LOG_VERBOSITY(FILELINE,FileLine)
      STAFF_CHECK_LOG_VERBOSITY(FUNCTION,Function)
      STAFF_CHECK_LOG_VERBOSITY(DATETIME,DateTime)
      STAFF_CHECK_LOG_VERBOSITY(PID,Pid)
      STAFF_CHECK_LOG_VERBOSITY(THREAD,Thread)
      STAFF_CHECK_LOG_VERBOSITY(DEFAULT,Default)
      STAFF_CHECK_LOG_VERBOSITY(ALL,All)

      if (!m_nVerbosity)
      {
        std::cerr << "staff:Log: invalid STAFF_LOG_VERBOSITY: [" << szStaffLogVerbosity
                  << "]\nvalid values are (TEXT, LEVEL, FILELINE, FUNCTION, "
                     "DATETIME, PID, THREAD, DEFAULT, ALL). Setting default." << std::endl;
        m_nVerbosity = LogVerbosityDefault;
      }
#undef STAFF_CHECK_LOG_VERBOSITY
    }
  }

  Log::~Log()
  {
  }

  void Log::SetLogStream(std::ostream* pOutStream)
  {
    m_pStream = pOutStream;
  }

  LogStream Log::Write(LogLevel eLogLevel, const char* szFileLine, const char *szFunction)
  {
    if (!m_pStream || eLogLevel > m_eLevel)
    {
      return LogStream(NULL);
    }

    *m_pStream << ColorDefault;

    switch (eLogLevel)
    {
    case LogLevelAlert:
      *m_pStream << ColorFlashOn << ColorBright << ColorBackgroundBrown << ColorTextRed;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "ALERT ";
      break;

    case LogLevelCrit:
      *m_pStream << ColorFlashOn << ColorBright << ColorBackgroundBrown << ColorTextRed;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "CRITICAL ";
      break;

    case LogLevelError:
      *m_pStream << ColorBright << ColorTextRed;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "ERROR ";
      break;

    case LogLevelWarning:
      *m_pStream << ColorTextBrown;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "WARNING ";
      break;

    case LogLevelNotice:
      *m_pStream << ColorTextCyan;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "NOTICE ";
      break;

    case LogLevelInfo:
      *m_pStream << ColorTextGrey;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "INFO ";
      break;

    case LogLevelDebug:
      *m_pStream << ColorTextBlack << ColorBright;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "DEBUG ";
      break;

    case LogLevelDebug1:
      *m_pStream << ColorTextBlack << ColorBright;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "DEBUG1 ";
      break;

    case LogLevelDebug2:
      *m_pStream << ColorTextBlack << ColorBright;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "DEBUG2 ";
      break;

    case LogLevelDebug3:
      *m_pStream << ColorTextBlack << ColorBright;
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "DEBUG3 ";
      break;

    default:
      if ((m_nVerbosity & LogVerbosityLevel)) *m_pStream << "UNKNOWN ";
    }

    if ((m_nVerbosity & LogVerbosityDateTime))
    {
      static const int nBuffSize = 32;
      char szBuff[nBuffSize];

#if defined WIN32
      SYSTEMTIME tTime;
      GetLocalTime(&tTime);
      staff_snprintf(szBuff, nBuffSize, "%02d-%02d-%02d %02d:%02d:%02d.%03d ",
                     tTime.wDay, tTime.wMonth, tTime.wYear,
                     tTime.wHour, tTime.wMinute, tTime.wSecond, tTime.wMilliseconds);
#else
      struct tm tLocal;
      struct timeb tTimeb;
      ftime(&tTimeb);
      localtime_r(&tTimeb.time, &tLocal);

      staff_snprintf(szBuff, nBuffSize, "%02d-%02d-%02d %02d:%02d:%02d.%03d ",
                     tLocal.tm_mday, tLocal.tm_mon + 1, tLocal.tm_year + 1900,
                     tLocal.tm_hour, tLocal.tm_min, tLocal.tm_sec, tTimeb.millitm);
#endif

      *m_pStream << szBuff;
    }


    if ((m_nVerbosity & LogVerbosityPid))
    {
      *m_pStream << "P" << Process::GetCurrentId() << " ";
    }

    if ((m_nVerbosity & LogVerbosityThread))
    {
      *m_pStream << "T" << Thread::GetCurrentId() << " ";
    }

    if ((m_nVerbosity & LogVerbosityFileLine))
    {
      *m_pStream << szFileLine;
    }

    if ((m_nVerbosity & LogVerbosityFunction))
    {
      *m_pStream << ColorUnderlineOn << szFunction << ColorUnderlineOff << ": ";
    }

    return LogStream(m_pStream);
  }

  void Log::SetLogLevel(LogLevel eLogLevel)
  {
    m_eLevel = eLogLevel;
  }

  void Log::SetLogVerbosity(int nLogVerbosity)
  {
    m_nVerbosity = nLogVerbosity;
  }


  Log::LogLevel LogEntryScope::m_eLevelDefault = Log::LogLevelDebug;

  LogEntryScope::LogEntryScope(const char* szFileLine, const char* szFunction, Log::LogLevel eLevel):
    m_eLevel(eLevel),
    m_szFileLine(szFileLine),
    m_szFunction(szFunction)
  {
    Log::Inst().Write(m_eLevel, szFileLine, szFunction) << "Enter";
  }

  LogEntryScope::~LogEntryScope()
  {
    Log::Inst().Write(m_eLevel, m_szFileLine, m_szFunction) << "Exit";
  }

  void LogEntryScope::SetDefaultLogLevel(Log::LogLevel eLevel)
  {
    m_eLevelDefault = eLevel;
  }


  std::ostream& LogResultSuccess(std::ostream& rStream)
  {
    return rStream << ": " << ColorTextGreen << ColorBright << "Success" << ColorDefault;
  }

  std::ostream& LogResultDone(std::ostream& rStream)
  {
    return rStream << ": " << ColorTextGreen << ColorBright << "Done" << ColorDefault;
  }

  std::ostream& LogResultWarning(std::ostream& rStream)
  {
    return rStream << ": " << ColorTextBrown << ColorBright << "Warning" << ColorDefault;
  }

  std::ostream& LogResultFailed(std::ostream& rStream)
  {
    return rStream << ": " << ColorTextRed << ColorBright << "Failed" << ColorDefault;
  }

} // namespace staff
