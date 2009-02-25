#ifndef _XMLVALUE_H_
#define _XMLVALUE_H_

#include <rise/os/oscommon.h>
#include <rise/string/String.h>
#include <rise/xml/XMLFileStream.h>

namespace rise
{
  namespace xml
  {
    //!        xml-значение
    class RISE_EXPORT CXMLValue
    {
    public:
      //!        конструктор
      CXMLValue();
      
      //!        конструктор
      CXMLValue(const CXMLValue& rValue);

      //!        конструктор
      template<typename TTYPE>
      CXMLValue(const TTYPE rValue);

      //!        конструктор
      CXMLValue(const CString& rValue);


      //!        оператор копирования
      /*
         \param  исходный объект
         \return ссылка на текущий объект
      */
      CXMLValue& operator=(const CXMLValue& rValue);

      //!        оператор копирования
      /*
         \param  исходный объект
         \return ссылка на текущий объект
      */
      template<typename TTYPE>
      CXMLValue& operator=(TTYPE rValue);

      //!        оператор копирования
      /*
         \param  исходный объект
         \return ссылка на текущий объект
      */
      CXMLValue& operator=(const CString& rValue);

      //!        оператор копирования
      /*
         \param  исходный объект
         \return ссылка на текущий объект
      */
      CXMLValue& operator=(const TChar* rValue);

      //!        оператор проверки на равенство
      /*
         \param  RETURN:         true - значения равны
      */
      template<typename TTYPE>
      bool operator==(const TTYPE rValue) const;

      //!        оператор проверки на равенство
      /*
         \param  RETURN:         true - значения равны
      */
      bool operator==(const CXMLValue& rXMLValue) const;

      //!        оператор проверки на равенство
      /*
         \param  RETURN:         true - значения равны
      */
      bool operator==(const TChar* szValue) const;

      //!        оператор проверки на равенство
      /*
         \param  RETURN:         true - значения равны
      */
      bool operator==(const CString& rValue) const;

      //!        оператор проверки на неравенство
      /*
         \param  значение
         \return true - значения не равны
      */
      template<typename TTYPE>
      bool operator!=(const TTYPE rValue) const;

      //!        оператор проверки на неравенство
      /*
         \param  значение
         \return true - значения не равны
      */
      bool operator!=(const CXMLValue& rXMLValue) const;

      //!        оператор проверки на неравенство
      /*
         \param  значение
         \return true - значения не равны
      */
      bool operator!=(const TChar* rValue) const;

      //!        оператор проверки на неравенство
      /*
         \param  значение
         \return true - значения не равны
      */
      bool operator!=(const CString& rValue) const;

      //!        оператор приведения к заданнуму типу
      /*
         \return результат в заданном типе
      */
      template<typename TTYPE>
      operator const TTYPE() const;

      //!        оператор приведения к заданнуму типу
      /*
         \return результат в заданном типе
      */
      template<typename TTYPE>
      operator TTYPE();

      //!        оператор приведения к строке
      /*
         \return строка
      */
      operator const CString&() const;

      //!        оператор приведения к строке
      /*
         \return строка
      */
      operator CString&();

      //!        приведение к строке
      /*
         \return строка
      */
      const CString& AsString() const;

      //!        приведение к строке
      /*
         \return строка
      */
      CString& AsString();

      //!        приведение к целому
      /*
         \return целое
      */
      int AsInt() const;

      //!        приведение к числу с плавающей точкой
      /*
         \return целое
      */
      float AsFloat() const;

      //!        приведение к числу с плавающей точкой
      /*
         \return целое
      */
      double AsDouble() const;

    private:
      CString m_sValue; // значение

    };

    //!        оператор сериализации
    /*
        CLogicFormatException
       \param   rStream - поток сериализации
       \param   rXMLValue - ссылка на зачение
       \return результирующий поток сериализации
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLValue& rXMLValue);

    //!        оператор десериализации
    /*
        CLogicFormatException
       \param   rStream - поток десериализации
       \param   rXMLValue - ссылка на зачение
       \return результирующий поток десериализации
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLValue& rXMLValue);

    //!        оператор сериализации
    /*
        CLogicFormatException
       \param   rStream - поток сериализации
       \param   rXMLValue - ссылка на зачение
       \return результирующий поток сериализации
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLValue& rXMLValue);

    //!        оператор десериализации
    /*
        CLogicFormatException
       \param   rStream - поток десериализации
       \param   rXMLValue - ссылка на зачение
       \return результирующий поток десериализации
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLValue& rXMLValue);


  } // namespace xml
} // namespace rise

#include "XMLValue.hpp"

#endif // _XMLVALUE_H_
