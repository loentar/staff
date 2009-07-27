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

#ifndef _XMLFILESTREAM_H_
#define _XMLFILESTREAM_H_

#include <rise/common/streamtypes.h>

namespace rise
{
  namespace xml
  {
    //!        класс для потокового чтения XML
    class RISE_EXPORT CXMLIStream
    {
    public:
      //!        default constructor
      CXMLIStream(CIStream& rSream, const CString& sFileName, int& nLine);
      CXMLIStream(CIStream& rSream, const CString& sFileName);
      CXMLIStream(CIStream& rSream);
      CXMLIStream(CXMLIStream& rStream);

      ~CXMLIStream();

      operator CIStream&();

      //!        считать следующий символ из входного потока
      /*
         \return         true - символ успешно прочитан
      */
      bool ReadChar(TChar& rChar);

      //!        считать следующий символ из входного потока
      /*
         CXMLReadException - ошибка чтения (EOF)
         \return считанный символ
      */
      TChar ReadChar();

      //!        чтение метасимвола из потока
      /*
          CXMLFormatException - неверный метасимвол
         \return считанный метасимвол
      */
      TChar ReadMetaChar();

      //!        чтение метасимвола из потока
      /*
          CXMLFormatException - неверный метасимвол
         \param  chData - считанный метасимвол
      */
      bool ReadMetaChar(TChar& chData);

      //!        получить следующий символ из входного потока не удаляя
      /*
         \return true - символ успешно прочитан
         \param  chData - считанный метасимвол
      */
      bool PeekChar(TChar& rChar);

      //!        получить следующий символ из входного потока не удаляя
      /*
         CXMLReadException - ошибка чтения (EOF)
         \return символ
      */
      TChar PeekChar();

      //!        пропустить пробельные символы
      void SkipWhitespace();

      //!        чтение из потока строки до одного из символьных разделителей
      /*
         \param  sData - результат
         \param  sDelimiters - разделители
         \return номер разделителя по которому было прервано чтение
      */
      int ReadString(CString& sData, const CString& sDelimiters = " \t\n\r");

      //!        чтение из потока строки до маркера
      /*
            если маркер ="" читать до конца файла
            
         \param  sData - результат
         \param  sMarker - маркер 
         \return номер разделителя по которому было прервано чтение
      */
      void ReadStringUntil(CString& sString, const CString& sMarker = "");

      //!        чтение идентификатора
      /*
         \param   sId - идентификатор
         \return none
      */
      void ReadId(CString& sId);

      //!        сравнение строки со значением во входном потоке
      /*
             при положительном результате совпадающие данные будут 
             удалены из потока
         \param   sValue - строка для теста
         \param   bResult - результат теста
         \return true, если входной поток совпадает со строкой
      */
      bool Test(const CString& sValue);
      
      //! получить имя файла
      const CString& GetFileName() const;

      //!  получить текущий номер строки
      int GetLine() const;

      //! отменить чтение символа
      void UnGetChar(const TChar chData);

      //!        Добавление текста в контент
      /*
         \param   rStream - поток
         \param   sContent - контетн
      */
      void AddContent(CString& sContent);

    private:
      //! отмена чтения строки
      void UnGetString(const CString& sData);

    private:
      int                 m_nLineInternal;     //! внутренний номер строки
      int&                m_nLine;             //! текущий номер строки
      const CString       m_sFileNameInternal; //! внутреннее имя файла
      const CString&      m_sFileName;         //! имя файла
      CIStream&           m_tIStream;          //! поток чтения

      CXMLIStream& operator =(const CXMLIStream&);

    };

    //!        класс для потоковой записи XML
    class RISE_EXPORT CXMLOStream
    {
    public:
      //!        конструктор
      CXMLOStream(COStream& rOStream, const CString& sFileName);

      //!        конструктор
      CXMLOStream(COStream& rOStream);

      //!        конструктор
      CXMLOStream(CXMLOStream& rStream);

      //!        деструктор
      ~CXMLOStream();

      //!        записывает в поток строку с поддержкой метасимволов
      /*
         \param  sString - строка для записи
      */
      void WriteString(const CString& sString);

      //!        записывает в поток данные
      /*
         \param  tData - данные
         \return ссылка на поток
      */
      template<typename TDATA>
      CXMLOStream& operator<<( const TDATA& tData )
      {
        m_tOStream << tData;
        return *this;
      }

      //!        оператор получения потока
      operator COStream&();

    private:
      COStream&      m_tOStream;          //! ссылка на поток
      const CString& m_sFileName;         //! имя файла
      const CString  m_sFileNameInternal; //! внутреннее имя файла

      CXMLOStream& operator=(const CXMLOStream&);
    };

    //!        класс для потокового чтения XML-файла
    class RISE_EXPORT CXMLFileIStream: public CXMLIStream
    {
    public:
      //!        конструктор
      CXMLFileIStream();

      //!        деструктор
      ~CXMLFileIStream();

      //!        открыть файл потока для чтения
      /*
          CXMLOpenException - ошибка открытия файла
         \param  sFileName - имя файла
      */
      void Open(const CString& sFileName);

      //!        закрыть поток чтения
      void Close();

    private:
      int             m_nLine;       //! текущий номер строки
      CString         m_sFileName;   //! имя файла
      CIFStream       m_tIFStream;   //! входной файловый поток
    };

    //!        класс для потоковой записи в  XML-файл
    class RISE_EXPORT CXMLFileOStream: public CXMLOStream
    {
    public:
      //!        конструктор
      CXMLFileOStream();
      
      //!        деструктор      
      ~CXMLFileOStream();

      //!        открыть файл потока для записи
      /*
         CXMLOpenException - ошибка открытия файла
         \param  sFileName - имя файла
      */
      void Open(const CString& sFileName);

      //!        закрыть поток записи
      void Close();

    private:
      COFStream         m_tOFStream;   //! файловый поток
      CString           m_sFileName;   //! имя файла
    };


  } // namespace xml
} // namespace rise

#endif // _XMLFILESTREAM_H_
