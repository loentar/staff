#ifndef _EXCEPTIONTEMPLATE_H_
#define _EXCEPTIONTEMPLATE_H_

#include <rise/os/osdllexport.h>
#include <rise/common/Exception.h>
#include <rise/common/excodes.h>
#include <rise/common/exmacros.h>
#include <rise/string/String.h>

namespace rise
{
  //!        шаблонный класс исключений
  template<int CLASS>
    class CExceptionTemplate: public CException
  {
    public:
      //!        деструктор
      virtual ~CExceptionTemplate() throw();

      //!        создание исключения
      /*
         \param  sFile - имя файла исходника
         \param  nLine - номер строки в файле исходника 
         \param  sFunction - имя функции в которой произошло исключение
         \param  sDescr - описание исключения
         \return none
      */
      void Create(const CString& sFile, int nLine, const CString& sFunction, 
        const CString& sDescr = "", const CString& sExpr = "", 
        const CString& sObject = "") throw();
      
      //!        оператор копирования
      /*
         \return ссылка на текущий обьект
      */
      CExceptionTemplate& operator=(const CExceptionTemplate& rException);
      
      //!        получение файла в котором произошло исключение
      /*
         \return имя файла-исходника в котором произошло исключение
      */
      const CString& GetFileName() const throw();
      
      //!        получение строки в файле в котором произошло исключение
      /*
         \return строка в файле в котором произошло исключение
      */
      int GetLine() const throw();
      
      //!        получение имени функции в которой произошло исключение
      /*
         \return имя функции в которой произошло исключение
      */
      const CString& GetFunction() const throw();
      
      //!        получение описания исключения
      /*
         \return описание исключения
      */
      const CString& GetDescr() const throw();
      
      //!        получение выражения 
      /*
         \return выражение в результате которого возникло исключение
      */
      const CString& GetExpr() const throw();
      
      //!        получение обьекта исключения
      /*
         \return обьекта исключения
      */
      const CString& GetObjectName() const throw();

      //!        установка обьекта исключения
      /*
         \param  sObjectName обьект исключения
      */
      void SetObjectName(const CString& sObjectName) throw();
      
      //!        получение класса исключения
      /*
         \return код исключения
      */
      EXCLASS GetClass() const throw();

      //!        получение полного описания исключения
      /*
         \sa SetFormat
         \param  sFormat - строка для форматирования
         \return описания исключения
      */
      CString GetString(const CString& sFormat = "") const throw();

      //!        установка формата строки для функции GetString
      /*
         поддерживаются следующие метасимволы:
         {File}   - имя исходного файла, в котором возникло исключение
         {Line}   - строка в файле,  в котором возникло исключение
         {Func}   - функция,  в котором возникло исключение
         {Descr}  - описание исключения
         {Expr}   - условие, из за невыполнения которого возникло исключение
         {Class}  - класс исключения(файловое, логическое, внутреннее)
         {Code}   - код исключения
         {Object} - объект исключения(строка с описанием)
         {Stack}  - стек вызовов
         
         стек вызовов будет доступен только при установленной переменной окружения RISE_EXCEPTION_STACKTRACING=1
         
         \sa GetString
         \param  sFormat - строка для форматирования
         \return описания исключения
      */
      virtual void SetFormat(CString& sFormat) const throw();

    private:
      CString  m_sFile;           //!< файл в котором произошло исключение
      int      m_nLine;           //!< строка в которой произошло исключение
      CString  m_sFunction;       //!< функция в которой произошло исключение
      CString  m_sExpr;           //!< выражение из за которого произошло исключение
      CString  m_sDescr;          //!< описание исключения
      CString  m_sObject;         //!< объект исключения
      CString  m_sStack;          //!< стек вызовов
      CString  m_sFormat;
      bool     m_bStackTracing;
  }; // class CExceptionTemplate

  //!        базовый класс для файловых исключений
  class RISE_EXPORT CFileException: public CExceptionTemplate<EXCFILE>
  {
  };

  template<int CODE>
  class RISE_EXPORT CFileExceptionTemplate: public CFileException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        базовый класс для логических исключений
  class RISE_EXPORT CLogicException: public CExceptionTemplate<EXCLOGIC>
  {
  };

  template<int CODE>
  class RISE_EXPORT CLogicExceptionTemplate: public CLogicException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        базовый класс для внутренних исключений
  class RISE_EXPORT CInternalException: public CExceptionTemplate<EXCINTERNAL>
  {
  };

  template<int CODE>
  class RISE_EXPORT CInternalExceptionTemplate: public CInternalException
  {
  public:
    EXCODE GetCode() const throw() { return static_cast<EXCODE>(CODE); }
  };

  //!        ошибка при создании файла
  typedef CFileExceptionTemplate<EXCREATE>             CFileCreateException;

  //!        ошибка при удалении файла
  typedef CFileExceptionTemplate<EXDELETE>             CFileDeleteException;

  //!        ошибка при открытии файла
  typedef CFileExceptionTemplate<EXOPEN>               CFileOpenException;

  //!        ошибка при закрытии файла
  typedef CFileExceptionTemplate<EXCLOSE>              CFileCloseException;

  //!        ошибка при чтении файла
  typedef CFileExceptionTemplate<EXREAD>               CFileReadException;

  //!        ошибка при записи в файл
  typedef CFileExceptionTemplate<EXWRITE>              CFileWriteException;

  //!        ошибка ввода/вывода
  typedef CFileExceptionTemplate<EXIO>                 CFileIOException;


  //!        ошибка не произведена инициализация
  typedef CLogicExceptionTemplate<EXNOINIT>            CLogicNoInitException;

  //!        ошибка объект уже существует/инициализация уже произведена
  typedef CLogicExceptionTemplate<EXALREADYEXISTS>     CLogicAlreadyExistsException;

  //!        ошибка неверный размер
  typedef CLogicExceptionTemplate<EXSIZE>              CLogicSizeException;

  //!        ошибка не найден указанный элемент
  typedef CLogicExceptionTemplate<EXNOITEM>            CLogicNoItemException;

  //!        ошибка превышено время ожидания
  typedef CLogicExceptionTemplate<EXTIMEOUT>           CLogicTimeoutException;

  //!        ошибка нет прав
  typedef CLogicExceptionTemplate<EXNOPERMISSION>      CLogicPermissionException;

  //!        ошибка неверный формат
  typedef CLogicExceptionTemplate<EXFORMAT>            CLogicFormatException;

  //!        ошибка переданы неверные параметра вызова
  typedef CLogicExceptionTemplate<EXPARAM>             CLogicParamException;


  //!        ошибка внутренняя
  typedef CInternalExceptionTemplate<EXINTERNAL>       CInternalInternalException;

  //!        ошибка недостаточно памяти для выполнения операции
  typedef CInternalExceptionTemplate<EXNOMEM>          CInternalNoMemException;

  //!        ошибка при проверке утверждения
  typedef CInternalExceptionTemplate<EXASSERT>         CInternalAssertException;

  //!        ошибка отсутствует реализация функции
  typedef CInternalExceptionTemplate<EXNOTIMPLEMENTED> CInternalNotImplementedException;

  //!        ошибка временное решение
  typedef CInternalExceptionTemplate<EXTEMPDECISION>   CInternalTempDecisionException;

}  // namespace rise

#include "ExceptionTemplate.hpp"

#endif // #ifndef _EXCEPTIONTEMPLATE_H_
