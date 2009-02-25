#ifndef _XMLEXCEPTION_H_
#define _XMLEXCEPTION_H_

#include <rise/common/ExceptionTemplate.h>

// генерация исключения с кодом и строкой описания
#define THROWXML(RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
{ RISE_LOCAL_EXCEPTION cException; \
  cException.Create(__FILE__, __LINE__, __PRETTY_FUNCTION__, RISE_LOCAL_DESCRIPTION, \
                    #RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE); \
  throw cException;}

#define ASSERTXMLS(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
  if (!(RISE_LOCAL_EXPRESSION)) \
    THROWXML(RISE_LOCAL_EXCEPTION, RISE_LOCAL_DESCRIPTION, RISE_LOCAL_EXPRESSION, RISE_LOCAL_FILE, RISE_LOCAL_LINE)

#define ASSERTXML(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE) \
  ASSERTXMLS(RISE_LOCAL_EXPRESSION, RISE_LOCAL_EXCEPTION, RISE_LOCAL_FILE, RISE_LOCAL_LINE)

namespace rise
{
  namespace xml
  {
    //!        базовый класс исключений XML
    class CXMLException: public CExceptionTemplate<EXCXML>
    {
    };

    //!        шаблонный класс исключений XML
    template<EXCODE CODE>
    class CXMLExceptionTemplate: public CXMLException
    {
      public:
        EXCODE GetCode() const throw() 
        { 
          return CODE; 
        };

        //!        создание исключения
        /*
           \param   szFile - имя файла исходника
           \param  nLine - номер строки в файле исходника 
           \param  szFunction - имя функции в которой произошло исключение
           \param  szDescr - описание исключения
           \return none
        */
        void Create( 
          const CString& szFile, 
          int nLine, 
          const CString& szFunction, 
          const CString& szDescr = "", 
          const CString& szExpr = "", 
          const CString& szFileName = "", 
          unsigned uLineNo = -1) throw()
        {
          CXMLException::Create(szFile, nLine, szFunction, szDescr, szExpr, szFileName);
          m_uLineNo = uLineNo;
        }

        //!        получение номера строки XML файла в которой произошло исключение
        virtual const int GetLineNo() const throw()
        {
          return m_uLineNo;
        }

        virtual void SetFormat( CString& sFormat ) const throw()
        {
          sFormat = "{File}[{Line}] {Func}: {Descr}({Expr}); {Object}: {LineNo}\n";
        }
        
        //!        получение полного описания исключения
        /*
           \param  sFormat - строка для форматирования
           \return описания исключения
        */
        CString GetString(const CString& sFormat = "") const throw()
        {
          CString sResult = CXMLException::GetString(sFormat);
          
          StrReplace(sResult, "{LineNo}", ToStr(GetLineNo()));
          
          return sResult;
        }
     
      private:
        unsigned m_uLineNo;  //! номер строки в XML файле

    };  // class CXMLException ///////////////////////////////////////

    typedef CXMLExceptionTemplate<EXCREATE> CXMLCreateException;
    typedef CXMLExceptionTemplate<EXOPEN>   CXMLOpenException;
    typedef CXMLExceptionTemplate<EXCLOSE>  CXMLCloseException;
    typedef CXMLExceptionTemplate<EXREAD>   CXMLReadException;
    typedef CXMLExceptionTemplate<EXWRITE>  CXMLWriteException;
    typedef CXMLExceptionTemplate<EXFORMAT> CXMLFormatException;
    typedef CXMLExceptionTemplate<EXNOITEM> CXMLNoItemException;

  } // namespace xml
} // namespace rise

#endif // #ifndef _XMLEXCEPTION_H_
