#ifndef _XMLDecl_h_
#define _XMLDecl_h_

#include <rise/os/osdllexport.h>
#include <iostream>
#include <rise/xml/XMLFileStream.h>

namespace rise
{
  namespace xml
  {
    //!        класс декларации XML-документа
    class RISE_EXPORT CXMLDecl
    {
    public:
      enum EXMLDeclStandAlone //! тип автономности
      {
        EXMLSAUndefined = -1, //!< автономность не указана
        EXMLSANo,             //!< не автономный
        EXMLSAYes             //!< вытономный
      };

      CString              m_sVersion;      //!< Версия XML-документа(поддерживается только 1.0)
      CString              m_sEncoding;     //!< Кодировка
      EXMLDeclStandAlone   m_eStandalone;   //!< Декларация автономного документа

    public:
      //!        конструктор
      CXMLDecl();

      //!        деструктор
      ~CXMLDecl();
    };

    //!        оператор сериализации
    /*
       \param   rStream - поток сериализации
       \return результирующий поток сериализации
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLDecl& rXMLDecl);

    //!        оператор десериализации
    /*
       исключения:
       CLogicFormatException - неверный формат файла
       CXMLNoItemException - файл не начинается с декларации XML

       \param   rStream - поток десериализации
       \return результирующий поток сериализации
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLDecl& rXMLDecl);

    //!        оператор сериализации
    /*
       \param   rStream - поток сериализации
       \return результирующий поток сериализации
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLDecl& rXMLDecl);

    //!        оператор десериализации
    /*
       исключения:
       CLogicFormatException - неверный формат файла
       CXMLNoItemException - файл не начинается с декларации XML

       \param   rStream - поток десериализации
       \return результирующий поток сериализации
    */
     RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLDecl& rXMLDecl);

  } // namespace xml
} // namespace rise

#endif // _XMLDecl_h_

