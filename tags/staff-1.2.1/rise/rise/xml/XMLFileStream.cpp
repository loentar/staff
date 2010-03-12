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

    CXMLIStream::~CXMLIStream()
    {
    }

    CXMLIStream::operator CIStream&()
    {
      return m_tIStream;
    }


    bool CXMLIStream::ReadChar(TChar& rChar)
    {
      m_tIStream.get(rChar);
      if(!m_tIStream.good())
        return false;
      
      if (rChar == '\n')
        ++m_nLine;

      return true;
    }

    TChar CXMLIStream::ReadChar()
    {
      TChar chData;
      ASSERTXMLS(ReadChar(chData), CXMLReadException, "can\'t read file (maybe EOF)", m_sFileName, m_nLine);
      return chData;
    }

    bool CXMLIStream::PeekChar( TChar& rChar )
    {
      rChar = static_cast<TChar>(m_tIStream.peek());
      return m_tIStream.good();
    }

    TChar CXMLIStream::PeekChar()
    {
      TChar chData = '\0';
      ASSERTXMLS(PeekChar(chData), CXMLReadException, "Error PeekChar, maybe EOF", m_sFileName, m_nLine);
      return chData;
    }

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


    void CXMLIStream::SkipWhitespace()
    {
      TChar chWhitespace = '\0';

      while(PeekChar(chWhitespace) && (sWhitespace.find(chWhitespace) != CString::npos))
        ReadChar();
    }

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

    void CXMLIStream::ReadId( CString& sId )
    {
      bool bisNameSpace = false;
      SkipWhitespace();

      TChar ch = PeekChar();
      sId = "";

      if ( ch == '_' || ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' )
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

    CXMLOStream::~CXMLOStream()
    {
    }

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

    CXMLFileIStream::CXMLFileIStream():
      CXMLIStream(m_tIFStream, m_sFileName, m_nLine), m_nLine(0)
    {
    }

    CXMLFileIStream::~CXMLFileIStream()
    {
      Close();
    }

    void CXMLFileIStream::Open( const CString& sFileName )
    {
      Close();

      m_tIFStream.open(sFileName.c_str(), std::ios::in | std::ios::binary);
      ASSERTXMLS(m_tIFStream.good(), CXMLOpenException, "can\'t open file", sFileName, 0 );
      m_sFileName = sFileName;
      m_nLine = 0;
    }

    void CXMLFileIStream::Close()
    {
      if (m_tIFStream.is_open())
        m_tIFStream.close();
    }

    CXMLFileOStream::CXMLFileOStream():
      CXMLOStream(m_tOFStream, m_sFileName)
    {
    }

    CXMLFileOStream::~CXMLFileOStream()
    {
      Close();
    }


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
