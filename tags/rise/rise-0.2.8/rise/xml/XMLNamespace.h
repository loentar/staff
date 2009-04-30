#ifndef _XMLNAMESPACE_H_
#define _XMLNAMESPACE_H_

#include <rise/os/osdllexport.h>
#include <map>
#include <rise/common/containertypes.h>
#include <rise/xml/XMLFileStream.h>
#include <rise/string/String.h>

namespace rise
{
  namespace xml
  {

    //! пространство имен XML
    struct RISE_EXPORT SXMLNamespace
    {
      CString  sNs;    //!< имя пространства имен
      CString  sUri;   //!< URI

      //!         конструктор по умолчанию
      SXMLNamespace();

      //!         копирующий конструктор
      SXMLNamespace(const SXMLNamespace& rNamespace);

      //!         инициализирующий конструктор
      /*! \param  sNsInit - имя пространства имен
          \param  sUriInit - URI
          \return 
          */
      SXMLNamespace(const CString& sNsInit, const CString& sUriInit);

      //!         оператор копирования
      /*! \param  rNamespace - исходный 
          \return 
          */
      SXMLNamespace& operator=(const SXMLNamespace& rNamespace);

      //!         оператор проверки на равенство
      bool operator==(const SXMLNamespace& rNamespace) const;

      //!         оператор проверки на неравенство
      bool operator!=(const SXMLNamespace& rNamespace) const;
    };

    //!         оператор сериализации
    /*! \param  rStream - поток сериализации
        \param  rXMLNs - ссылка на пространство имен
        \return поток сериализации
        */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const SXMLNamespace& rXMLNs);

    //!         оператор сериализации
    /*! \param  rStream - поток сериализации
        \param  rXMLNs - ссылка на пространство имен
        \return поток сериализации
        */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const SXMLNamespace& rXMLNs);

  } // namespace xml
} // namespace rise

#endif // _XMLNAMESPACE_H_
