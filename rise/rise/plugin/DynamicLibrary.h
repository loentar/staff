#ifndef _DYNAMICLIBRARY_H_
#define _DYNAMICLIBRARY_H_

#include <string>
#include <rise/os/osdllexport.h>
#include <rise/os/osdl.h>

namespace rise
{
  namespace plugin
  {
    //!        класс работы с динамическими библиотеками
    class RISE_EXPORT CDynamicLibrary
    {
    public:
      CDynamicLibrary();

      virtual ~CDynamicLibrary();
      
      //!        подгрузка динамической библиотеки
      virtual void Load(const CString& sLibName, bool bRawName = false);

      //!        получение имени
      /*
         \return имя библиотеки
      */
      const CString& GetName() const;

      //!        получение указателя на символ в библиотеке
      /*
           CLogicNoItemException - указанный символ не найден
         \param  sSymName - имя символа
         \return указатель на функцию, NULL при ошибке
      */
      PLibSymbol GetSymbol(const CString& sSymName) const;

      //!        выгрузка библиотеки
      /*         CFileCloseException
      */
      void UnloadLibrary();

    private:
      HDynamicLib  m_hDynLib; //! дескриптор модуля
      CString      m_sName;   //! имя модуля
    };
  } //  namespace plugin
} // namespace rise

#endif // _DYNAMICLIBRARY_H_
