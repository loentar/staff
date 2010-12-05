/*
 *  Copyright 2009 Utkin Dmitry
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

#include <iostream>
#include <string.h>
#include <locale.h>
#include <rise/common/console.h>
#include <rise/common/streamtypes.h>
#include <rise/string/String.h>
#include <rise/string/Encoding.h>
#include <rise/process/Process.h>
#include <rise/threading/Thread.h>
#include "Log.h"

namespace rise
{
  CLog tLog;
  
  void DetectRecode(const char* szEncoding, int& nRecode)
  {
    if (szEncoding != NULL)
    {
      if (strcmp(szEncoding, "utf-8") == 0)
      {
        nRecode = CEncoding::ET_UTF_8;
      }
      else
      if (strcmp(szEncoding, "koi8-r") == 0)
      {
        nRecode = CEncoding::ET_KOI8R;
      }
      else
      if (strcmp(szEncoding, "cp1251") == 0)
      {
        nRecode = CEncoding::ET_WIN1251;
      }
      else
      if (strcmp(szEncoding, "cp866") == 0)
      {
        nRecode = CEncoding::ET_CP866;
      }
      else
      if (strcmp(szEncoding, "mac") == 0)
      {
        nRecode = CEncoding::ET_MACINTOSH;
      }
      else
      if (strcmp(szEncoding, "iso-8859-5") == 0)
      {
        nRecode = CEncoding::ET_ISO_8859_5;
      }
    }
  }

  CLog::CLog():
    m_eLogLevel(ELL_INFO), m_nLogVerbosity(ELV_DEFAULT)
  {
//    setlocale( LC_ALL, ".ACP" );

    const char* szLogFile = getenv("RISE_LOG_FILE");
    if(szLogFile == NULL)
      for (int i = 0; i != 10; ++i)
        m_pLogStream[i] = &tStdOut;
    else
    {
      CString sLogFile = szLogFile;
      const char* szLogFileAppend = getenv("RISE_LOG_FILE_APPEND");
      static COFStream tOutStream;
      std::ios::openmode nFlags = std::ios::out;
      
      if(szLogFileAppend != NULL && (strcmp(szLogFileAppend, "1") == 0 || strcmp(szLogFileAppend, "TRUE") == 0))
        nFlags |= std::ios::app;

      //  expand variables in log file name      
      CString::size_type nBegin = 0;
      CString::size_type nEnd = 0;
      CString sVariable;
      CString sValue;
      while((nBegin = sLogFile.find("$(", nEnd)) != CString::npos)
      {
        nEnd = sLogFile.find(')', nBegin + 2);
        if(nEnd == CString::npos)
          break;

        sVariable = sLogFile.substr(nBegin + 2, nEnd - nBegin - 2);
        if(sVariable == "PID")
        {
          sValue = ToStr(process::CProcess::GetCurrentProcessID());
        } else
        {
          const char* szValue = getenv(sVariable.c_str());
          if(szValue != NULL)
            sValue = szValue;
          else
            sValue = "";
        }
        
        if(sValue != "")
        {
          sLogFile.replace(nBegin, nEnd - nBegin + 1, sValue);
          nEnd = nBegin + sValue.size();
        }
      }
      
      tOutStream.open(sLogFile.c_str(), nFlags);

      if(tOutStream.good())
        for (int i = 0; i != 10; ++i)
          m_pLogStream[i] = &tOutStream;
      else
      {
        std::cout << ColorBright << ColorInkBrown << "Warning: cannot open Log file: \"" << szLogFile << "\". using stdout..\n\n" << ColorDefault << std::endl;
        for (int i = 0; i != 10; ++i)
          m_pLogStream[i] = &tStdOut;
      }
    }

#ifndef WIN32
    if(getenv("USER") == NULL)
      std::cout << ColorBright << ColorInkBrown << "Warning: environment variable USER = NULL!\n"
      "Please, check for proper linking librise.so\n"
      "Logging, signals etc. may does not work properly." << ColorDefault << std::endl;
#endif

    const char* szRiseLogLevel = getenv("RISE_LOG_LEVEL");
    if (szRiseLogLevel != NULL)
    {
#define RISE_CHECK_LOG_LEVEL(RISE_LOG_LEVEL)\
      if(strcmp(szRiseLogLevel, #RISE_LOG_LEVEL) == 0 )\
         m_eLogLevel = ELL_##RISE_LOG_LEVEL;\
      else
      RISE_CHECK_LOG_LEVEL(ALERT)
      RISE_CHECK_LOG_LEVEL(CRIT)
      RISE_CHECK_LOG_LEVEL(ERROR)
      RISE_CHECK_LOG_LEVEL(WARNING)
      RISE_CHECK_LOG_LEVEL(NOTICE)
      RISE_CHECK_LOG_LEVEL(INFO)
      RISE_CHECK_LOG_LEVEL(DEBUG)
      RISE_CHECK_LOG_LEVEL(DEBUG1)
      RISE_CHECK_LOG_LEVEL(DEBUG2)
      RISE_CHECK_LOG_LEVEL(DEBUG3)
      std::cerr << "rise:CLog: invalid RISE_LOG_LEVEL: " << szRiseLogLevel << std::endl;
#undef RISE_CHECK_LOG_LEVEL

      std::cout << "rise:CLog: RISE_LOG_LEVEL: " << szRiseLogLevel << std::endl;
    }

    const char* szRiseLogVerbosity = getenv("RISE_LOG_VERBOSITY");
    if (szRiseLogVerbosity != NULL)
    {
#define RISE_CHECK_LOG_VERBOSITY(RISE_LOG_VERBOSITY)\
      if(strcmp(szRiseLogVerbosity, #RISE_LOG_VERBOSITY) == 0 )\
        m_nLogVerbosity = ELV_##RISE_LOG_VERBOSITY;\
      else
      RISE_CHECK_LOG_VERBOSITY(NONE)
      RISE_CHECK_LOG_VERBOSITY(LEVEL)
      RISE_CHECK_LOG_VERBOSITY(FILE_LINE)
      RISE_CHECK_LOG_VERBOSITY(FUNCTION)
      RISE_CHECK_LOG_VERBOSITY(PID)
      RISE_CHECK_LOG_VERBOSITY(DEFAULT)
      RISE_CHECK_LOG_VERBOSITY(ALL)
      std::cout << "rise:CLog: invalid RISE_LOG_VERBOSITY: " << szRiseLogVerbosity << std::endl;
#undef RISE_CHECK_LOG_VERBOSITY

      std::cout << "rise:CLog: RISE_LOG_VERBOSITY: " << szRiseLogVerbosity << std::endl;
    }


    const char* szRiseLogSrcRecode = getenv("RISE_LOG_SRC_RECODE");
    if (szRiseLogSrcRecode == NULL)
    {
#if defined LINUX_RELEASE_MCBC || defined OS_FreeBSD
      m_nSrcRecode = CEncoding::ET_KOI8R;
#else
      m_nSrcRecode = CEncoding::ET_UTF_8;
#endif

#ifdef RISE_LOG_SRC_RECODE
      DetectRecode(RISE_LOG_SRC_RECODE, m_nSrcRecode);
#endif
    }
    else
    {
      DetectRecode(szRiseLogSrcRecode, m_nSrcRecode);
    }

    const char* szRiseLogDstRecode = getenv("RISE_LOG_DST_RECODE");
    if (szRiseLogDstRecode == NULL)
    {
#if defined LINUX_RELEASE_MCBC || defined OS_FreeBSD
      m_nDstRecode = CEncoding::ET_KOI8R;
#else
      m_nDstRecode = CEncoding::ET_UTF_8;
#endif

#ifdef RISE_LOG_DST_RECODE
      DetectRecode(RISE_LOG_DST_RECODE, m_nDstRecode);
#endif
    }
    else
    {
      DetectRecode(szRiseLogDstRecode, m_nDstRecode);
    }
  }
  
  CLog::~CLog()
  {
  }

  void CLog::SetLogStream( COStream* pOutStream, int nLogLevel /*= ELL_ALL*/ )
  {
    for (int i = ELL_ALERT, nPos = 0; i <= ELL_DEBUG3; i <<= 1, ++nPos)
      if ( (nLogLevel & i ) != 0 )
        m_pLogStream[nPos] = pOutStream;
  }

  CLogStream CLog::Log( int eLogLevel, const TChar* sFile, unsigned nLine, const TChar* sFunction )
  {
    if (eLogLevel > m_eLogLevel)
      return CLogStream(static_cast<COStream*>(NULL)/*, m_cs*/);

    int nLogStream = 0;
    for (int i = ELL_ALERT; (i < ELL_DEBUG3) && ((eLogLevel & i ) == 0); i <<= 1, ++nLogStream);
    if (m_pLogStream[nLogStream] == NULL)
      return CLogStream(static_cast<COStream*>(NULL)/*, m_cs*/);

    {
      COStream& tStream = *m_pLogStream[nLogStream];

      tStream << ColorDefault;

      switch(eLogLevel)
      {
      case ELL_ALERT:
        tStream << ColorFlashOn << ColorBright << ColorPaperBrown << ColorInkRed;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "ALERT ";
        break;
      case ELL_CRIT:
        tStream << ColorFlashOn << ColorBright << ColorPaperBrown << ColorInkRed;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "CRITICAL ";
        break;
      case ELL_ERROR:
        tStream << ColorBright << ColorInkRed;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "ERROR ";
        break;
      case ELL_WARNING:
        tStream << ColorInkBrown;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "WARNING ";
        break;
      case ELL_NOTICE:
        tStream << ColorInkCyan;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "NOTICE ";
        break;
      case ELL_INFO:
        tStream << ColorInkGrey;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "INFO ";
        break;
      case ELL_DEBUG:
        tStream << ColorInkBlack << ColorBright;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "DEBUG ";
        break;
      case ELL_DEBUG1:
        tStream << ColorInkBlack << ColorBright;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "DEBUG1 ";
        break;
      case ELL_DEBUG2:
        tStream << ColorInkBlack << ColorBright;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "DEBUG2 ";
        break;
      case ELL_DEBUG3:
        tStream << ColorInkBlack << ColorBright;
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "DEBUG3 ";
        break;
      default:
        if ((m_nLogVerbosity & ELV_LEVEL) != 0) tStream << "UNKNOWN ";
      }

      if ((m_nLogVerbosity & ELV_PID) != 0)
        tStream << "{" << process::CProcess::GetCurrentProcessID() << ":" << 
        threading::CThread::GetCurrentId() << "}";

      if ((m_nLogVerbosity & ELV_FILE_LINE) != 0)
      {
        CString tsFile = sFile;
        TSize stPos = tsFile.find_last_of(RISE_PATH_SEPARATOR);
        tStream << ( (stPos != CString::npos) ? tsFile.substr(stPos + 1) : sFile ) << "[" << nLine << "]: ";
      }

      if ((m_nLogVerbosity & ELV_FUNCTION) != 0)
        tStream << ColorUnderlineOn << sFunction << ColorUnderlineOff << ": ";

      return CLogStream(&tStream, true, m_nSrcRecode, m_nDstRecode);
    }
  }

  void CLog::SetLogLevel( ELogLevel eLogLevel )
  {
    m_eLogLevel = eLogLevel;
  }

  void CLog::SetLogVerbosity( int nLogVerbosity )
  {
    m_nLogVerbosity = nLogVerbosity;
  }

  CLogStream::CLogStream( COStream* pLogStream, bool bOutEndLine /*= true*/, int nSrcRecode, int nDstRecode) :
    m_pLogStream(pLogStream), m_bOutEndLine(bOutEndLine),
    m_nSrcRecode(nSrcRecode), m_nDstRecode(nDstRecode)
  {
  }

  CLogStream::~CLogStream()
  {
    try
    {
      if (m_pLogStream != NULL)
      {
        if (m_bOutEndLine)
          *m_pLogStream << ColorDefault << std::endl;
        else
          *m_pLogStream << std::flush;
      }
      
      m_pLogStream = NULL;
    }
    catch(...)
    {}
  }

  COStream* CLogStream::GetStream() const
  {
    return m_pLogStream;
  }

  void CLogStream::SetStream( COStream* pStream )
  {
    m_pLogStream = pStream;
  }

  COStream& LogResultSuccess(COStream& rLogStream)
  {
    return rLogStream << ": " << ColorInkGreen << ColorBright << "Success" << ColorDefault;
  }

  COStream& LogResultDone(COStream& rLogStream)
  {
    return rLogStream << ": " << ColorInkGreen << ColorBright << "Done" << ColorDefault;
  }

  COStream& LogResultWarning(COStream& rLogStream)
  {
    return rLogStream << ": " << ColorInkBrown << ColorBright << "Warning" << ColorDefault;
  }

  COStream& LogResultFailed(COStream& rLogStream)
  {
    return rLogStream << ": " << ColorInkRed << ColorBright << "Failed" << ColorDefault;
  }

  CLogStream& LogEndLOff( CLogStream& rLogStream )
  {
    rLogStream.m_bOutEndLine = false;
    return rLogStream;
  }

  CLogStream& LogEndLOn( CLogStream& rLogStream )
  {
    rLogStream.m_bOutEndLine = true;
    return rLogStream;
  }

  const CLogStream& CLogStream::operator<<( const char* tData ) const
  {
    if(tData != NULL)
    {
      OutRecoded(tData);
    }
    return *this;
  }

  const CLogStream& CLogStream::operator<<( const std::string& tData ) const
  {
    OutRecoded(tData);
    return *this;
  }

  void CLogStream::OutRecoded( const std::string& tData ) const
  {
    if (m_pLogStream != NULL)
    {
      if(m_nSrcRecode != m_nDstRecode)
      {
        try
        {
          CString sTmp;
          CEncoding::Convert(tData, sTmp, 
            static_cast<CEncoding::EType>(m_nSrcRecode), static_cast<CEncoding::EType>(m_nDstRecode));
          *m_pLogStream << sTmp;
        }
        catch(...)
        {
          *m_pLogStream << " *** " << tData << " *** ";
        }
      }
      else
      {
        *m_pLogStream << tData;
      }
    }
  }


  CLog::ELogLevel CLogEntry::m_eLevelDefault = rise::CLog::ELL_DEBUG;

  CLogEntry::CLogEntry( const TChar* sFile, unsigned nLine, const TChar* sFunction, CLog::ELogLevel eLevel /*= m_eLevelDefault*/ ) :
    m_eLevel(eLevel),
    m_sFile(sFile),
    m_nLine(nLine),
    m_sFunction(sFunction)
  {
    tLog.Log(m_eLevel, sFile, nLine, sFunction) << "Enter";
  }

  CLogEntry::~CLogEntry()
  {
    tLog.Log(m_eLevel, m_sFile.c_str(), m_nLine, m_sFunction.c_str()) << "Exit";
  }

  void CLogEntry::SetLogLevelDefault( CLog::ELogLevel eLevel /*= rise::CLog::ELL_DEBUG*/ )
  {
    m_eLevelDefault = eLevel;
  }

}
