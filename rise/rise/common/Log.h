#ifndef _LOG_H_
#define _LOG_H_

#include <iostream>
#include <string>
#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

//! формат лога
//! LogDebug() << "Timer no: " << nTimer << " was signalled" << EndLogLine;
#define PutToLog(RISE_LOG_LEVEL)\
  tLog.Log(RISE_LOG_LEVEL, __FILE__, __LINE__, __FUNCTION__)

//! запись в журнал сообщения о тревоге
#define LogAlert()   PutToLog(rise::CLog::ELL_ALERT)

//! запись в журнал сообщения о критичном событии
#define LogCrit()    PutToLog(rise::CLog::ELL_CRIT)

//! запись в журнал сообщения об ошибке
#define LogError()   PutToLog(rise::CLog::ELL_ERROR)

//! запись в журнал сообщения о предупреждении
#define LogWarning() PutToLog(rise::CLog::ELL_WARNING)

//! запись в журнал сообщения об уведомлении
#define LogNotice()  PutToLog(rise::CLog::ELL_NOTICE)

//! запись в журнал информационного сообщения
#define LogInfo()    PutToLog(rise::CLog::ELL_INFO)

//! запись в журнал отладочного сообщения высокого уровня
#define LogDebug()   PutToLog(rise::CLog::ELL_DEBUG)

//! запись в журнал отладочного сообщения среднего уровня
#define LogDebug1()  PutToLog(rise::CLog::ELL_DEBUG1)

//! запись в журнал отладочного сообщения низкого уровня
#define LogDebug2()  PutToLog(rise::CLog::ELL_DEBUG2)

//! запись в журнал отладочного сообщения очень низкого уровня
#define LogDebug3()  PutToLog(rise::CLog::ELL_DEBUG3)

//! запись в журнал стандартного отладочного сообщения
#define LogLabel()   PutToLog(rise::CLog::ELL_DEBUG) << "*";


//! запись в журнал сообщения о входе/выходе в функцию
#define LogEntry() \
  CLogEntry tLogEntry(__FILE__, __LINE__, __FUNCTION__);

//! запись в журнал сообщения о входе/выходе в функцию с указанным уровнем
#define LogEntryL(RISE_LOG_LEVEL) \
  CLogEntry tLogEntry(__FILE__, __LINE__, __FUNCTION__, RISE_LOG_LEVEL);

namespace rise
{
  class CLogStream;

  //!        поддержка журналирования
  class RISE_EXPORT CLog
  {
  public:
    enum ELogLevel  //! уровень журналирования
    {
      ELL_ALERT   = (1 << 0), //!< требуется немедленное вмешательство
      ELL_CRIT    = (1 << 1), //!< критические условия
      ELL_ERROR   = (1 << 2), //!< ошибки
      ELL_WARNING = (1 << 3), //!< предупреждения
      ELL_NOTICE  = (1 << 4), //!< важные рабочие условия
      ELL_INFO    = (1 << 5), //!< информационные сообщения
      ELL_DEBUG   = (1 << 6), //!< сообщения отладки
      ELL_DEBUG1  = (1 << 7), //!< сообщения отладки 1-го уровня
      ELL_DEBUG2  = (1 << 8), //!< сообщения отладки 2-го уровня
      ELL_DEBUG3  = (1 << 9), //!< сообщения отладки 3-го уровня
      ELL_ALL = ELL_ALERT | ELL_CRIT | ELL_ERROR | ELL_WARNING | ELL_NOTICE |
        ELL_INFO | ELL_DEBUG | ELL_DEBUG1 | ELL_DEBUG2 | ELL_DEBUG3 //!< все сообщения
    };

    enum ELogVerbosity      //! информативность сообщений
    {
      ELV_NONE      = 0,        //!< выводить только текст
      ELV_LEVEL     = (1 << 1), //!< выводить уроввень журналирования (ERROR, WARNING, etc.)
      ELV_FILE_LINE = (1 << 2), //!< выводить имя исходного файла и строку в нем
      ELV_FUNCTION  = (1 << 4), //!< выводить имя функции
      ELV_PID       = (1 << 5), //!< выводить идентификатор процесса и потока
      ELV_DEFAULT = ELV_LEVEL | //!< значение по умолчанию
         ELV_FILE_LINE | ELV_FUNCTION,
      ELV_ALL = ELV_LEVEL | //!< выводить всё
         ELV_FILE_LINE | ELV_FUNCTION | ELV_PID
    };

  public:
    //!        default constructor
    CLog();

    virtual ~CLog();

    //!        установка потока вывода лога
    /*
               установить поток (маска)
       \param  rOutStream - поток вывода (NULL - не выводить)
       \param  nLogLevel - уровени для которых необходимо 
    */
    void SetLogStream(COStream* pOutStream, int nLogLevel = ELL_ALL);

    //!        установить уровень журналирования.
    /*         в журнал будут выводиться только сообщения 
               с критичностью равной или выше указанной
               пример: запрет вывода сообщений отладки
                      SetLogLevel(rise::CLog::ELL_INFO)

       \param  eLogLevel - уровень журналирования
    */
    void SetLogLevel(ELogLevel eLogLevel);

    //!        установить уровень информативности журналирования
    /*
               пример: выводить только текст
                       SetLogLevel(rise::CLog::ELV_NONE)
       \param  nLogVerbosity - уровень информативности 
    */
    void SetLogVerbosity(int nLogVerbosity);

    //!        вывод сообщения в журнал
    /*         пример:
               LogDebug() << "Timer no: " << nTimer << " was signalled " << nTimerCount << " times";
       \param   eType - уровень сообщения
       \param  sFile - имя исходного файла
       \param  nLine - номер строки в исходном файле
       \param  sFunction - функция из которой был вызван лог
       \return поток для вывода
    */
    CLogStream Log(int eLogLevel, const TChar* sFile, unsigned nLine, const TChar* sFunction);

  private:
    COStream*    m_pLogStream[10];       //! потоки журналирования
    ELogLevel    m_eLogLevel;            //! уровень журналирования
    int          m_nLogVerbosity;        //! уровень информативности
    int          m_nSrcRecode;
    int          m_nDstRecode;
  };

  //!          класс-помощник вывода в журнал - для внутреннего использования
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
    mutable COStream*            m_pLogStream;  // поток вывода
    bool                         m_bOutEndLine; // выводить перевод строки
    int          m_nSrcRecode;
    int          m_nDstRecode;

  private:
    void OutRecoded( const std::string& tData ) const;

    CLogStream& operator=(const CLogStream&); // запрет копирования

    friend RISE_EXPORT CLogStream& LogEndLOff(CLogStream& rLogStream);
    friend RISE_EXPORT CLogStream& LogEndLOn(CLogStream& rLogStream);
  };

  //!  класс лога в области видимости
  class CLogEntry
  {
  public:
    CLogEntry(const TChar* sFile, unsigned nLine, const TChar* sFunction, CLog::ELogLevel eLevel = m_eLevelDefault);

    ~CLogEntry();

    static void SetLogLevelDefault(CLog::ELogLevel eLevel = rise::CLog::ELL_DEBUG);

  private:
    static CLog::ELogLevel   m_eLevelDefault;
    CLog::ELogLevel          m_eLevel;
    CString                  m_sFile;
    unsigned                 m_nLine;
    CString                  m_sFunction;
  };

  //!           вывести в журнал сообщение о успехе выполнения операции
  RISE_EXPORT COStream& LogResultSuccess(COStream& rLogStream);

  //!           вывести в журнал сообщение о завершении выполнения операции
  RISE_EXPORT COStream& LogResultDone(COStream& rLogStream);

  //!           вывести в журнал сообщение о предупреждении при выполнения операции
  RISE_EXPORT COStream& LogResultWarning(COStream& rLogStream);

  //!           вывести в журнал сообщение об ошибке при выполнения операции
  RISE_EXPORT COStream& LogResultFailed(COStream& rLogStream);

  //! отключить автоматический перевод строки в конце сообщения
  RISE_EXPORT CLogStream& LogEndLOff(CLogStream& rLogStream);

  //! включить автоматический перевод строки в конце сообщения (по умолчанию)
  RISE_EXPORT CLogStream& LogEndLOn(CLogStream& rLogStream);

  RISE_EXPORT extern CLog tLog;
}

#include "Log.hpp"

#endif // _LOG_H_
