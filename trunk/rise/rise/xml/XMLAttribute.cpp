#include <rise/xml/XMLException.h>
#include "XMLAttribute.h"

namespace rise
{
  namespace xml
  {
    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    SXMLAttribute
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SXMLAttribute::SXMLAttribute()
    {

    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    SXMLAttribute
    //    DESCRIPTION:    копирующий конструктор
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SXMLAttribute::SXMLAttribute(const SXMLAttribute& rAttribute):
      sAttrName(rAttribute.sAttrName), 
      sAttrValue(rAttribute.sAttrValue)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    SXMLAttribute
    //    DESCRIPTION:    инициализирующий конструктор
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SXMLAttribute::SXMLAttribute(const CString& rAttrName, const CXMLValue& rAttrValue):
      sAttrName(rAttrName), 
      sAttrValue(rAttrValue) 
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       =
    //    DESCRIPTION:    оператор копирования
    //    PARAMETRS:      rAttribute - другой атрибут
    //    RETURN:         ссылка на текущий объект
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SXMLAttribute& SXMLAttribute::operator=(const SXMLAttribute& rAttribute)
    { 
      sAttrName = rAttribute.sAttrName;
      sAttrValue = rAttribute.sAttrValue;  
      return *this;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       ==
    //    DESCRIPTION:    оператор проверки на равенство
    //    PARAMETRS:      rAttribute - другой атрибут
    //    RETURN:         true, если атрибуты эквивалентны
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool SXMLAttribute::operator==(const SXMLAttribute& rAttribute) const
    { 
      return sAttrName == rAttribute.sAttrName && sAttrValue == rAttribute.sAttrValue;  
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       ==
    //    DESCRIPTION:    оператор проверки на неравенство
    //    PARAMETRS:      rAttribute - другой атрибут
    //    RETURN:         true, если атрибуты не эквивалентны
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool SXMLAttribute::operator!=(const SXMLAttribute& rAttribute) const
    { 
      return !(operator==(rAttribute));
    }


    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //                    (in) rXMLAttr - ссылка на атрибут
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLAttribute& rXMLAttr)
    {
      return rStream << " " << rXMLAttr.sAttrName << "=\"" << rXMLAttr.sAttrValue << "\"";
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //                    (out) rXMLAttr - ссылка на атрибут
    //    RETURN:         результирующий поток десериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream& operator>>(CXMLIStream& rStream, SXMLAttribute& rXMLAttr)
    {
      TChar chTmp = '\0';
      rStream.ReadId(rXMLAttr.sAttrName);
      rStream.SkipWhitespace();
      chTmp = rStream.ReadChar();
      ASSERTXMLS(chTmp == '=', CXMLFormatException, "expected \'=\' while reading attribute \"" + 
        rXMLAttr.sAttrName + "\"", rStream.GetFileName(), rStream.GetLine());

      rStream.SkipWhitespace();
      chTmp = rStream.ReadChar();
      ASSERTXMLS(chTmp == '\"' || chTmp == '\'', CXMLFormatException, 
        "expected quote while reading attribute \"" + 
        rXMLAttr.sAttrName + "\"", rStream.GetFileName(), rStream.GetLine());
      
      rStream.ReadString(rXMLAttr.sAttrValue.AsString(), CString(1, chTmp));
      return rStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //                    (in) rXMLAttr - ссылка на атрибут
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLAttribute& rXMLAttr)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLAttr);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //                    (out) rXMLAttr - ссылка на атрибут
    //    RETURN:         результирующий поток десериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, SXMLAttribute& rXMLAttr)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLAttr);
    }



  } // namespace xml
} // namespace rise

