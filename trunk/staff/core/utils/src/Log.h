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
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */


#ifndef _STAFF_UTILS_LOG_H_
#define _STAFF_UTILS_LOG_H_

#include <string>
#include "staffutilsexport.h"
#include "LogStream.h"

#ifndef STAFF_FILE_LINE
#define STAFF_STRINGIZE(x) #x
#define STAFF_TO_STRING(x) STAFF_STRINGIZE(x)
#define STAFF_FILE_LINE __FILE__ "[" STAFF_TO_STRING(__LINE__) "] "
#endif

namespace staff
{
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;

  //! put data to log
  #define StaffLogWrite(STAFF_LOG_LEVEL)\
    Log::Inst().Write(::staff::Log::STAFF_LOG_LEVEL, STAFF_FILE_LINE, __FUNCTION__)

  //! log alert message
  #define LogAlert()   StaffLogWrite(LogLevelAlert)

  //! log critical message
  #define LogCrit()    StaffLogWrite(LogLevelCrit)

  //! log error message
  #define LogError()   StaffLogWrite(LogLevelError)

  //! log warning message
  #define LogWarning() StaffLogWrite(LogLevelWarning)

  //! log notice message
  #define LogNotice()  StaffLogWrite(LogLevelNotice)

  //! log info message
  #define LogInfo()    StaffLogWrite(LogLevelInfo)

  //! log high level debug message
  #define LogDebug()   StaffLogWrite(LogLevelDebug)

  //! log mid level debug message
  #define LogDebug1()  StaffLogWrite(LogLevelDebug1)

  //! log low level debug message
  #define LogDebug2()  StaffLogWrite(LogLevelDebug2)

  //! log very low level debug message
  #define LogDebug3()  StaffLogWrite(LogLevelDebug3)

  //! log standard debug message
  #define LogLabel()   StaffLogWrite(LogLevelDebug) << "*";


  //! log enter/exit function message
  #define LogEntry() \
    LogEntryScope tLogEntryScope(STAFF_FILE_LINE, __FUNCTION__);

  //! log enter/exit function message with given level
  #define LogEntryL(STAFF_LOG_LEVEL) \
    LogEntryScope tLogEntryScope(STAFF_FILE_LINE, __FUNCTION__, STAFF_LOG_LEVEL);


  //! Logger class
  /*! example:
        LogDebug() << "Some value = " << value;

      there is an log macro list:
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
      - LogEntry()    - log enter/exit scope message
  */
  class STAFF_UTILS_EXPORT Log
  {
  public:
    enum LogLevel  //! log level
    {
      LogLevelAlert,    //!< alert
      LogLevelCrit,     //!< critical
      LogLevelError,    //!< error
      LogLevelWarning,  //!< warning
      LogLevelNotice,   //!< notice
      LogLevelInfo,     //!< information
      LogLevelDebug,    //!< debug message
      LogLevelDebug1,   //!< debug message 1 level
      LogLevelDebug2,   //!< debug message 2 level
      LogLevelDebug3    //!< debug message 3 level
    };

    enum LogVerbosity      //! log verbosity
    {
      LogVerbosityText      = 0,        //!< text only
      LogVerbosityLevel     = (1 << 0), //!< message level (ERROR, WARNING, etc.)
      LogVerbosityFileLine  = (1 << 1), //!< source file name and file line
      LogVerbosityFunction  = (1 << 2), //!< function name
      LogVerbosityDateTime  = (1 << 3), //!< date time
      LogVerbosityPid       = (1 << 4), //!< process id
      LogVerbosityThread    = (1 << 5), //!< process thread id

      LogVerbosityDefault   = LogVerbosityLevel | //!< default
                              LogVerbosityFileLine | LogVerbosityFunction | LogVerbosityDateTime,

      LogVerbosityAll       = LogVerbosityLevel | //!< all
                              LogVerbosityFileLine | LogVerbosityFunction |
                              LogVerbosityPid | LogVerbosityThread | LogVerbosityDateTime
    };

  public:
    //! get log instance
    /*! \return log instance
      */
    static Log& Inst();

    //! set log stream to output messages
    /*! \param  pOutStream - log stream (NULL - do not output)
    */
    void SetLogStream(std::ostream* pOutStream);

    //! set log level
    /*! log messages with level equal or higher that set
        example: disable debug and info outputting.
                 output only warnings and more important messages
          SetLogLevel(::Log::LogLevelWarning)

        \param  eLogLevel - log level
    */
    void SetLogLevel(LogLevel eLogLevel);

    //! set log verbosity
    /*! example: log text only
        SetLogLevel(Log::LogVerbosityText)
        \param  nLogVerbosity - log verbosity
    */
    void SetLogVerbosity(int nLogVerbosity);

    //! log message
    /*! \param  eLogLevel - message log level
        \param  szFileLine - source file name and line
        \param  szFunction - function
        \return log stream
    */
    LogStream Write(LogLevel eLogLevel, const char* szFileLine, const char* szFunction);

  private:
    Log();
    ~Log();
    Log(const Log&);
    Log& operator=(const Log&);

  private:
    std::ostream*  m_pStream;    //!< output stream
    LogLevel       m_eLevel;     //!< log level
    int            m_nVerbosity; //!< verbosity
  };


  //! log entry
  class STAFF_UTILS_EXPORT LogEntryScope
  {
  public:
    //! log entry message
    /*! \param  szFileLine - source file name and line number
        \param  szFunction - function
        \param  eLevel - level
      */
    LogEntryScope(const char* szFileLine, const char* szFunction,
                  Log::LogLevel eLevel = m_eLevelDefault);

    //! destructor
    ~LogEntryScope();

    //! set default log level for log entry
    /*! \param eLevel - default log level
    */
    static void SetDefaultLogLevel(Log::LogLevel eLevel = Log::LogLevelDebug);

  private:
    static Log::LogLevel  m_eLevelDefault; //!< default log level
    Log::LogLevel         m_eLevel;        //!< current log level
    const char*           m_szFileLine;    //!< file name and line number
    const char*           m_szFunction;    //!< function
  };

} // namespace staff

#endif // _STAFF_UTILS_LOG_H_
