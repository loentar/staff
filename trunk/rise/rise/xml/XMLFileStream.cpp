#include <strstream>
#include <rise/string/String.h>
#include "XMLException.h"
#include "XMLFileStream.h"

namespace rise
{
  namespace xml
  {

    static CString sWhitespace = " \t\n\r";

    class CMetaChar
    {
    private:
      struct SMetaChar
      {
        TChar chChar;
        CString sMetaChar;

        bool operator==(const TChar chChar_) const
        {
          return chChar_ == chChar;
        }

        bool operator==(const CString& sMetaChar_) const
        {
          return sMetaChar_ == sMetaChar;
        }

        operator const TChar() const
        {
          return chChar;
        }

        operator const CString&() const
        {
          return sMetaChar;
        }
      };

      static const SMetaChar m_stMetaChars[5];

    public:
      static void ToMetaChar(const TChar chChar, CString& sMetaChar)
      {
        for (int i = 0; i < 5; ++i)
          if (chChar == m_stMetaChars[i].chChar)
          {
            sMetaChar = m_stMetaChars[i].sMetaChar;
            return;
          }

          COStringStream sStream;
          sStream << "&#" << (int)chChar << ";" << '\0';

          sStream.str(sMetaChar);
      }

      static bool FromMetaChar(const CString& sMetaChar, TChar& chChar)
      {
        int nSize = (int)sMetaChar.size();
        if (nSize < 3)
          return false;

        for (int i = 0; i < 5; ++i)
          if (sMetaChar == m_stMetaChars[i].sMetaChar)
          {
            chChar = m_stMetaChars[i].chChar;
            return true;
          }

          if (sMetaChar.substr(0, 2) != "&#")
            return false;

          for (int nPos = 2; nPos < (nSize - 1); ++nPos)
            if (sMetaChar[nPos] < '0' || sMetaChar[nPos] > '9')
              return false;

          if(sMetaChar[nSize - 1] != ';')
            return false;

          int nChar = 0;
          FromStr(sMetaChar.substr(2), nChar);

          chChar = static_cast<TChar>(nChar);
          return true;
      }

      static bool ToBaseMetaChar(const TChar chChar, CString& sMetaChar)
      {
        for (int i = 0; i < 5; ++i)
          if (chChar == m_stMetaChars[i].chChar)
          {
            sMetaChar = m_stMetaChars[i].sMetaChar;
            return true;
          }
          return false;
      }
    };

    const CMetaChar::SMetaChar CMetaChar::m_stMetaChars[] = 
    {
      { '\"',  "&quot;" },
      { '\'',  "&apos;" },
      {  '&',  "&amp;"  },
      {  '<',  "&lt;"   },
      {  '>',  "&gt;"   }
    };

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLIStream
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream::CXMLIStream(CIStream& rStream, const CString& sFileName, int& nLine/* = 0*/):
      m_nLineInternal(0), m_nLine(nLine), m_sFileName(sFileName), m_tIStream(rStream)
    {
    }

    CXMLIStream::CXMLIStream(CIStream& rStream, const CString& sFileName):
      m_nLineInternal(0), m_nLine(m_nLineInternal), m_sFileName(sFileName), m_tIStream(rStream)
    {
    }

    CXMLIStream::CXMLIStream(CIStream& rStream):
      m_nLineInternal(0), m_nLine(m_nLineInternal), m_sFileName(m_sFileNameInternal), m_tIStream(rStream)
    {
    }

    CXMLIStream::CXMLIStream(CXMLIStream& rStream):
      m_nLineInternal(0), m_nLine(rStream.m_nLine), m_sFileName(rStream.m_sFileName), m_tIStream(rStream.m_tIStream)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CXMLIStream
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream::~CXMLIStream()
    {
    }

    CXMLIStream::operator CIStream&()
    {
      return m_tIStream;
    }


    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadChar
    //    DESCRIPTION:    считать следующий символ из входного потока
    //    PARAMETRS:      none
    //    RETURN:         true - символ успешно прочитан
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CXMLIStream::ReadChar(TChar& rChar)
    {
      m_tIStream.get(rChar);
      if(!m_tIStream.good())
        return false;
      
      if (rChar == '\n')
        ++m_nLine;

      return true;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadChar
    //    DESCRIPTION:    считать следующий символ из входного потока
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CXMLReadException - ошибка чтения (EOF)
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    TChar CXMLIStream::ReadChar()
    {
      TChar chData;
      ASSERTXMLS(ReadChar(chData), CXMLReadException, "can\'t read file (maybe EOF)", m_sFileName, m_nLine);
      return chData;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       PeekChar
    //    DESCRIPTION:    получить следующий символ из входного потока не удаляя
    //    PARAMETRS:      none
    //    RETURN:         true - символ успешно прочитан
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CXMLIStream::PeekChar( TChar& rChar )
    {
      rChar = static_cast<TChar>(m_tIStream.peek());
      return m_tIStream.good();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       PeekChar
    //    DESCRIPTION:    получить следующий символ из входного потока не удаляя
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CXMLReadException - ошибка чтения (EOF)
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    TChar CXMLIStream::PeekChar()
    {
      TChar chData = '\0';
      ASSERTXMLS(PeekChar(chData), CXMLReadException, "Error PeekChar, maybe EOF", m_sFileName, m_nLine);
      return chData;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadMetaChar
    //    DESCRIPTION:    чтение метасимвола из потока
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CXMLFormatException - неверный метасимвол
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    TChar CXMLIStream::ReadMetaChar()
    {
      TChar chTmp = '\0';
      ASSERTXMLS(ReadMetaChar(chTmp), CXMLReadException, "can\'t read file (maybe EOF)", m_sFileName, m_nLine);
      return chTmp;
    }

    bool CXMLIStream::ReadMetaChar(TChar& chData)
    {
      if(!ReadChar(chData))
        return false;

      if (chData != '&')
        return true;

      CString sMetaChar = "&";

      do
      {
        chData = ReadChar();
        sMetaChar += chData;
      } while (chData != ';');

      ASSERTXMLS(CMetaChar::FromMetaChar(sMetaChar, chData), CXMLFormatException, 
        "invalid metachar: \"" + sMetaChar + "\"", GetFileName(), GetLine());;

      return true;
    }


    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SkipWhitespace
    //    DESCRIPTION:    пропустить пробельные символы
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLIStream::SkipWhitespace()
    {
      TChar chWhitespace = '\0';

      while(PeekChar(chWhitespace) && (sWhitespace.find(chWhitespace) != CString::npos))
        ReadChar();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadString
    //    DESCRIPTION:    чтение из потока строки до одного из символьных разделителей
    //    PARAMETRS:      sData - результат
    //                    sDelimiters - разделители
    //    RETURN:         номер разделителя по которому было прервано чтение
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    int CXMLIStream::ReadString( CString& sData, const CString& sDelimiters /*= " \t\n\r"*/ )
    {
      sData = "";
      TSize nPos = CString::npos;
      TChar chTmp = '\0';
      for(;;)
      {
        chTmp = PeekChar();
        if (chTmp == '&')
          chTmp = ReadMetaChar();
        else
        {
          chTmp = ReadChar();
          nPos = sDelimiters.find(chTmp);
          if (nPos != CString::npos)
            break;
        }
        sData += chTmp;
      }
      return static_cast<int>(nPos);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadString
    //    DESCRIPTION:    чтение из потока строки до маркера
    //    PARAMETRS:      sData - результат
    //                    sMarker - маркер 
    //    RETURN:         номер разделителя по которому было прервано чтение
    //    EXCEPTIONS:     none
    //    COMMENT:        если маркер ="" читать до конца файла
    //////////////////////////////////////////////////////////////////////////////
    void CXMLIStream::ReadStringUntil( CString& sString, const CString& sMarker /*= ""*/ )
    {
      sString = "";
      if (sMarker == "")
      {
        TChar chTmp;
        while ( ReadMetaChar(chTmp) )
          sString += chTmp;
      } else
      {
        while (!Test(sMarker))
          sString += ReadMetaChar();
      }
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadId(...)
    //    DESCRIPTION:    чтение идентификатора
    //    PARAMETRS:      (out) sId - идентификатор
    //    RETURN:         none
    //    EXCEPTIONS:     
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLIStream::ReadId( CString& sId )
    {
      bool bisNameSpace = false;
      SkipWhitespace();

      TChar ch = PeekChar();
      sId = "";

      if ( ch == '_' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' ) // проверка 1-го символа
      {
        sId += ch;
        for(;;)
        {
          ReadChar();
          ch = PeekChar();
          if ( !(ch == '_' || ch == '-' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9') )
          {
            if (ch != ':')
              break;

            if(!bisNameSpace)
              bisNameSpace = true;
            else
              THROWXML(CXMLFormatException, "Invalid ID! (duplicating ':' )", , m_sFileName, m_nLine);
          }

          sId += ch;
        }
      }

      ASSERTXMLS(sId != "", CXMLFormatException, "Invalid ID!", m_sFileName, m_nLine);
    }

    void CXMLIStream::UnGetChar( const TChar chData )
    {
      if(chData == '\n') 
        --m_nLine;
      m_tIStream.seekg(-1, std::ios::cur); // !!!!!!metasymbols?????
      RISE_ASSERT(m_tIStream.good());
    }

    void CXMLIStream::UnGetString( const CString& sData )
    {
      TSize stPos = 0; 
      while ((stPos = sData.find('\n', stPos)) != CString::npos)
        --m_nLine;
      m_tIStream.seekg(-std::streamoff(sData.size()), std::ios::cur); // !!!metasymbols
      RISE_ASSERT(m_tIStream.good());
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Test(...)
    //    DESCRIPTION:    сравнение строки со значением во входном потоке
    //    PARAMETRS:      (in) sValue - строка для теста
    //                    (out) bResult - результат теста
    //    RETURN:         true, если входной поток совпадает со строкой
    //    EXCEPTIONS:     
    //    COMMENT:        при положительном результате совпадающие данные будут 
    //                    удалены из потока
    //////////////////////////////////////////////////////////////////////////////
    bool CXMLIStream::Test( const CString& sValue )
    {
      TChar chTmp = '\0';
      for(CString::const_iterator itPos = sValue.begin(); itPos != sValue.end(); ++itPos)      
      {
        chTmp = PeekChar();

        if ( chTmp != *itPos )
        {// unget doesn't work 
          if (itPos != sValue.begin())
            UnGetString(sValue.substr(0, itPos - sValue.begin()));
          return false;
        }

        ReadChar();
      }

      return true;
    }

    const CString& CXMLIStream::GetFileName() const
    {
      return m_sFileName;
    }

    int CXMLIStream::GetLine() const
    {
      return m_nLine;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AddContent
    //    DESCRIPTION:    Добавление текста в контент
    //    PARAMETRS:      (in) rStream - поток
    //                    (out) sContent - контетн
    //    RETURN:         
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLIStream::AddContent( CString& sContent )
    {
      CString sTmp;
      ReadStringUntil(sTmp, "<");
      StrTrim(sTmp);
      UnGetChar('<');  // '<' != '\n'
      if ( sContent != "" && sTmp != "" )
        sContent += ' ';
      sContent += sTmp;
    }

    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLOStream
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream::CXMLOStream( COStream& rOStream, const CString& sFileName):
      m_tOStream(rOStream), m_sFileName(sFileName)
    {
    }

    CXMLOStream::CXMLOStream( COStream& rOStream):
      m_tOStream(rOStream), m_sFileName(m_sFileNameInternal)
    {
    }

    CXMLOStream::CXMLOStream( CXMLOStream& rStream ):
      m_tOStream(rStream.m_tOStream), m_sFileName(rStream.m_sFileName)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     CXMLOStream
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream::~CXMLOStream()
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       WriteString
    //    DESCRIPTION:    записывает в поток строку с поддержкой метасимволов
    //    PARAMETRS:      sString - строка для записи
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLOStream::WriteString( const CString& sString )
    {
      CString sTmp;
      for(CString::const_iterator itStr = sString.begin(); itStr != sString.end(); ++itStr)
      {
        if (CMetaChar::ToBaseMetaChar(*itStr, sTmp))
          m_tOStream << sTmp;
        else
          m_tOStream << *itStr;
      }
    }

    CXMLOStream::operator COStream&()
    {
      return m_tOStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLFileIStream
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLFileIStream::CXMLFileIStream():
      CXMLIStream(m_tIFStream, m_sFileName, m_nLine), m_nLine(0)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CXMLFileIStream
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLFileIStream::~CXMLFileIStream()
    {
      Close();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Open
    //    DESCRIPTION:    открыть файл потока для чтения
    //    PARAMETRS:      sFileName - имя файла
    //    RETURN:         none
    //    EXCEPTIONS:     CXMLOpenException - ошибка открытия файла
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLFileIStream::Open( const CString& sFileName )
    {
      Close();

      m_tIFStream.open(sFileName.c_str(), std::ios::in | std::ios::binary);
      ASSERTXMLS(m_tIFStream.good(), CXMLOpenException, "can\'t open file", sFileName, 0 );
      m_sFileName = sFileName;
      m_nLine = 0;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Close
    //    DESCRIPTION:    закрыть поток чтения
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLFileIStream::Close()
    {
      if (m_tIFStream.is_open())
        m_tIFStream.close();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLFileOStream
    //    DESCRIPTION:    default constructor
    //    PARAMETRS:      none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLFileOStream::CXMLFileOStream():
      CXMLOStream(m_tOFStream, m_sFileName)
    {
    }

    CXMLFileOStream::~CXMLFileOStream()
    {
      Close();
    }


    //////////////////////////////////////////////////////////////////////////////
    //    CLASS:          CXMLOStream
    //    DESCRIPTION:    none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLFileOStream::Open( const CString& sFileName )
    {
      Close();

      m_tOFStream.open(sFileName.c_str(), std::ios::out | std::ios::binary);
      ASSERTXMLS(m_tOFStream.good(), CXMLOpenException, "can\'t open file", sFileName, 0 );
      m_sFileName = sFileName;
    }

    void CXMLFileOStream::Close()
    {
      if (m_tOFStream.is_open())
        m_tOFStream.close();
    }


  } // namespace xml
} // namespace rise
