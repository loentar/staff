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

#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

//! put data to log
#define PutToLog(RISE_LOG_LEVEL)\
  tLog.Log(RISE_LOG_LEVEL, __FILE__, __LINE__, __FUNCTION__)

//! log alert message
#define LogAlert()   PutToLog(rise::CLog::ELL_ALERT)

//! log critical message
#define LogCrit()    PutToLog(rise::CLog::ELL_CRIT)

//! log error message
#define LogError()   PutToLog(rise::CLog::ELL_ERROR)

//! log warning message
#define LogWarning() PutToLog(rise::CLog::ELL_WARNING)

//! log notice message
#define LogNotice()  PutToLog(rise::CLog::ELL_NOTICE)

//! log info message
#define LogInfo()    PutToLog(rise::CLog::ELL_INFO)

//! log high level debug message
#define LogDebug()   PutToLog(rise::CLog::ELL_DEBUG)

//! log mid level debug message
#define LogDebug1()  PutToLog(rise::CLog::ELL_DEBUG1)

//! log low level debug message
#define LogDebug2()  PutToLog(rise::CLog::ELL_DEBUG2)

//! log very low level debug message
#define LogDebug3()  PutToLog(rise::CLog::ELL_DEBUG3)

//! log standard debug message
#define LogLabel()   PutToLog(rise::CLog::ELL_DEBUG) << "*";


//! log enter/exit function message
#define LogEntry() \
  CLogEntry tLogEntry(__FILE__, __LINE__, __FUNCTION__);

//! log enter/exit function message with given level
#define LogEntryL(RISE_LOG_LEVEL) \
  CLogEntry tLogEntry(__FILE__, __LINE__, __FUNCTION__, RISE_LOG_LEVEL);

namespace rise
{
  class CLogStream;

  //!        Logger class
  /*!   putting to log example:
        LogDebug() << "Timer no: " << nTimer << " was signaled";

        there is an log macros list:
        - LogAlert()    - log alert message
        - LogCrit()     - log critical message
        - LogError()    - log error message
        - LogWarning()  - log warning message
        - LogNotice()   - log notice message
        - LogInfo()     - log info message
        - LogDebug()    - log high level debug message
        - LogDebug1()   - log mid level debug message
        - LogDebug2()   - log low level debug message
        - LogDebug3()   - log very low level debug message
        - LogLabel()    - log standard debug message
        - LogEntry()    - log enter/exit function(scope) message
  */
  class RISE_EXPORT CLog
  {
  public:
    enum ELogLevel  //! log level
    {
      ELL_ALERT   = (1 << 0), //!< alert
      ELL_CRIT    = (1 << 1), //!< critical
      ELL_ERROR   = (1 << 2), //!< error
      ELL_WARNING = (1 << 3), //!< warning
      ELL_NOTICE  = (1 << 4), //!< notice
      ELL_INFO    = (1 << 5), //!< information
      ELL_DEBUG   = (1 << 6), //!< debug message
      ELL_DEBUG1  = (1 << 7), //!< debug message 1 level
      ELL_DEBUG2  = (1 << 8), //!< debug message 2 level
      ELL_DEBUG3  = (1 << 9), //!< debug message 3 level
      ELL_ALL = ELL_ALERT | ELL_CRIT | ELL_ERROR | ELL_WARNING | ELL_NOTICE |
        ELL_INFO | ELL_DEBUG | ELL_DEBUG1 | ELL_DEBUG2 | ELL_DEBUG3 //!< all messages
    };

    enum ELogVerbosity      //! log verbosity
    {
      ELV_NONE      = 0,        //!< out text only
      ELV_LEVEL     = (1 << 1), //!< out message level (ERROR, WARNING, etc.)
      ELV_FILE_LINE = (1 << 2), //!< out source file name and file line
      ELV_FUNCTION  = (1 << 4), //!< out function name
      ELV_PID       = (1 << 5), //!< out process id and thread id
      ELV_DEFAULT = ELV_LEVEL | //!< default
         ELV_FILE_LINE | ELV_FUNCTION,
      ELV_ALL = ELV_LEVEL | //!< all
         ELV_FILE_LINE | ELV_FUNCTION | ELV_PID
    };

  public:
    //!        default constructor
    CLog();

    virtual ~CLog();

    //!        set log stream
    /*
       \param  rOutStream - log stream (NULL - do not output)
       \param  nLogLevel - combination of levels for this stream
    */
    void SetLogStream(COStream* pOutStream, int nLogLevel = ELL_ALL);

    //!        set log level
    /*!        log messages with level equal or higher that set
               example: disable log messages
                       SetLogLevel(rise::CLog::ELL_INFO)

       \param  eLogLevel - log level
    */
    void SetLogLevel(ELogLevel eLogLevel);

    //!        set log verbosity
    /*!        example: log text only
                       SetLogLevel(rise::CLog::ELV_NONE)
       
       \param  nLogVerbosity - log verbosity
    */
    void SetLogVerbosity(int nLogVerbosity);

    //!        put message to log
    /*! \param  eLogLevel - message log level
        \param  sFile - source file name
        \param  nLine - source file line
        \param  sFunction - function
        \return log stream
    */
    CLogStream Log(int eLogLevel, const TChar* sFile, unsigned nLine, const TChar* sFunction);

  private:
    COStream*    m_pLogStream[10];       //! log streams
    ELogLevel    m_eLogLevel;            //! log level
    int          m_nLogVerbosity;        //! verbosity level
    int          m_nSrcRecode;           //! source encoding
    int          m_nDstRecode;           //! destination(screen) encoding
  };

  //!          log stream - log output helper(internal use only)
  class RISE_EXPORT CLogStream
  {
  public:
    CLogStream(COStream* pLogStream, bool bOutEndLine = true, int nSrcRecode = 0, int nDstRecode = 0);
    ~CLogStream();

    void SetStream(COStream* pStream);
    COStream* GetStream() const;

    const CLogStream& operator<<( const char* tData ) const;
    const CLogStream& operator<<( const std::string& tData ) const;

    template<typename TDATA>
    const CLogStream& operator<<( TDATA tData ) const;

  private:
    mutable COStream*            m_pLogStream;
    bool                         m_bOutEndLine;
    int          m_nSrcRecode;
    int          m_nDstRecode;

  private:
    void OutRecoded( const std::string& tData ) const;

    CLogStream& operator=(const CLogStream&);

    friend RISE_EXPORT CLogStream& LogEndLOff(CLogStream& rLogStream);
    friend RISE_EXPORT CLogStream& LogEndLOn(CLogStream& rLogStream);
  };

  //!  log entry
  class RISE_EXPORT CLogEntry
  {
  public:
    //!        log entry message
    /*         
       \param  eType - message level
       \param  sFile - source file name
       \param  nLine - source file line
       \param  sFunction - function
       \return log stream
    */
    CLogEntry(const TChar* sFile, unsigned nLine, const TChar* sFunction, CLog::ELogLevel eLevel = m_eLevelDefault);

    //! destructor
    ~CLogEntry();

    //! set default log level for log entry
    /*! \param eLevel - default log level
    */
    static void SetLogLevelDefault(CLog::ELogLevel eLevel = rise::CLog::ELL_DEBUG);

  private:
    static CLog::ELogLevel   m_eLevelDefault; //! default log level
    CLog::ELogLevel          m_eLevel;        //! current log level
    CString                  m_sFile;         //! file name
    unsigned                 m_nLine;         //! line number
    CString                  m_sFunction;     //! function
  };

  //!           log success message
  RISE_EXPORT COStream& LogResultSuccess(COStream& rLogStream);

  //!           log done message
  RISE_EXPORT COStream& LogResultDone(COStream& rLogStream);

  //!           log warning message
  RISE_EXPORT COStream& LogResultWarning(COStream& rLogStream);

  //!           log fail message
  RISE_EXPORT COStream& LogResultFailed(COStream& rLogStream);

  //!           disable carriage return
  RISE_EXPORT CLogStream& LogEndLOff(CLogStream& rLogStream);

  //!           enable carriage return
  RISE_EXPORT CLogStream& LogEndLOn(CLogStream& rLogStream);

  RISE_EXPORT extern CLog tLog;
}

#include "Log.hpp"

#endif // _LOG_H_
