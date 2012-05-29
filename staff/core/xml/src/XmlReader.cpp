/*
 *  Copyright 2012 Utkin Dmitry
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

#include <streambuf>
#include <staff/utils/fromcstring.h>
#include <staff/utils/CharsetConverter.h>
#include <staff/utils/Error.h>
#include "Exception.h"
#include "Document.h"
#include "Attribute.h"
#include "Namespace.h"
#include "Element.h"
#include "Node.h"
#include "XmlReader.h"

namespace staff
{
namespace xml
{
  class XmlReader::XmlReaderImpl
  {
  public:
    XmlReaderImpl(std::istream& rStream, bool bStripWhitespace):
      m_rStream(rStream),
      m_bStripWhitespace(bStripWhitespace),
      m_nLine(0),
      m_pCharsetConvertor(NULL)
    {
    }

    ~XmlReaderImpl()
    {
      if (m_pCharsetConvertor)
      {
        delete m_pCharsetConvertor;
      }
    }

    void ReadDeclaration(Declaration& rDeclaration)
    {
      SkipWhitespace();

      rDeclaration.SetVersion("1.0");
      rDeclaration.SetEncoding("UTF-8");
      rDeclaration.SetStandalone(Declaration::StandaloneUndefined);

      if (!Test("<?xml"))
      {
        return;
      }

      std::string sName;
      std::string sValue;
      char chTmp = '\0';
      for (;;)
      {
        SkipWhitespace();
        if (Test("?>"))
        {
          break;
        }

        ReadStringBeforeChr(sName, " \n\r\t=");
        SkipWhitespace();
        STAFF_XML_ASSERT(Test("="), "'=' expected while reading attribute/namespace [" + sName + "]",
                         m_sFileName, m_nLine);
        SkipWhitespace();

        ReadChar(chTmp);
        STAFF_XML_ASSERT(chTmp == '"' || chTmp == '\'', "quote expected while reading attribute/namespace ["
                         + sName + "]", m_sFileName, m_nLine);
        ReadStringWithChr(sValue, chTmp);

        if (sName == "version")
        {
          rDeclaration.SetVersion(sValue);
        }
        else
        if (sName == "encoding")
        {
          rDeclaration.SetEncoding(sValue);
        }
        else
        if (sName == "standalone")
        {
          if (sValue == "yes")
          {
            rDeclaration.SetStandalone(Declaration::StandaloneYes);
          }
          else
          if (sValue == "no")
          {
            rDeclaration.SetStandalone(Declaration::StandaloneNo);
          }
          else
          {
            STAFF_XML_THROW("Invalid Attribute value: " + sValue, m_sFileName, m_nLine);
          }
        }
        else
        {
          STAFF_XML_THROW("Invalid Attribute name: " + sName, m_sFileName, m_nLine);
        }
      }
    }

    void ReadElement(Element& rElement)
    {
      STAFF_XML_ASSERT(Test("<"), "Element start expected", "", m_nLine);

      char chTmp = '\0';
      std::string sName;
      std::string sValue;
      ReadStringBeforeChr(sName);
      STAFF_XML_ASSERT(ValidateId(sName), "Element name validation failed: [" + sName + "]",
                       m_sFileName, m_nLine);
      rElement.SetName(sName);

      SkipWhitespace();

      // read attributes and namespaces
      while (!Test(">"))
      {
        // element without children
        if (Test("/>"))
        {
          return;
        }

        ReadStringBeforeChr(sName, " \n\r\t=");
        SkipWhitespace();
        STAFF_XML_ASSERT(Test("="), "'=' expected while reading attribute/namespace [" + sName + "]",
                         m_sFileName, m_nLine);
        SkipWhitespace();

        ReadChar(chTmp);
        STAFF_XML_ASSERT(chTmp == '"' || chTmp == '\'', "quote expected while reading attribute/namespace ["
                         + sName + "]", m_sFileName, m_nLine);
        ReadStringWithChr(sValue, chTmp);
        UnescapeString(sValue);

        if (sName == "xmlns")
        {
          // default namespace
          rElement.DeclareDefaultNamespace(sValue);
        }
        else
        if (!sName.compare(0, 6, "xmlns:"))
        {
          // namespace
          rElement.DeclareNamespace(sValue, sName.substr(6));
        }
        else
        {
          // attribute
          rElement.CreateAttribute(sName, sValue);
        }

        SkipWhitespace();
      }

      // read children

      for (;;)
      {
        // text node child
        ReadStringWithChr(sValue, '<');
        UnescapeString(sValue);

        if (!sValue.empty())
        {
          if (HasText(sValue) || !m_bStripWhitespace)
          {
            rElement.CreateChildText(sValue);
          }
        }

        // some node

        // comment
        if (Test("!--"))
        {
          ReadStringWithStr(sValue, "-->");
          rElement.CreateChildComment(sValue);
        }
        else
        // cdata
        if (Test("![CDATA["))
        {
          ReadStringWithStr(sValue, "]]>");
          rElement.CreateChildCdata(sValue);
        }
        else
        // end of parent element
        if (Test("/"))
        {
          ReadStringBeforeChr(sName);
          STAFF_XML_ASSERT(sName == rElement.GetPrefixName(), "Invalid name of element end."
                       " Found: [" + sName + "] expected [" + rElement.GetPrefixName() + "]",
                       m_sFileName, m_nLine);
          SkipWhitespace();
          STAFF_XML_ASSERT(Test(">"), "'>' Expected", m_sFileName, m_nLine);
          break;
        }
        else
        // child element
        {
          // moving back to the '<' char
          m_rStream.seekg(static_cast<std::istream::off_type>(-1), std::ios::cur);
          ReadElement(rElement.CreateChildElement());
        }
      }
    }


    void UnescapeString(std::string& sString)
    {
      std::string::size_type nPosStart = 0;
      std::string::size_type nPosEnd = 0;
      while ((nPosStart = sString.find_first_of('&', nPosEnd)) != std::string::npos)
      {
        nPosEnd = sString.find_first_of(';', nPosStart);
        STAFF_XML_ASSERT(nPosEnd != std::string::npos, "';' not found while unescaping string ["
                         + sString + "]", m_sFileName, m_nLine);

        const std::string& sEsc = sString.substr(nPosStart + 1, nPosEnd - nPosStart - 1);
        STAFF_XML_ASSERT(!sEsc.empty(), "Invalid sequence found while unescaping string ["
                         + sString + "]", m_sFileName, m_nLine);

        if (sEsc == "lt")
        {
          sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, '<');
          nPosEnd = nPosStart + 1;
        }
        else
        if (sEsc == "gt")
        {
          sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, '>');
          nPosEnd = nPosStart + 1;
        }
        else
        if (sEsc == "amp")
        {
          sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, '&');
          nPosEnd = nPosStart + 1;
        }
        else
        if (sEsc == "apos")
        {
          sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, '\'');
          nPosEnd = nPosStart + 1;
        }
        else
        if (sEsc == "quot")
        {
          sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, '"');
          nPosEnd = nPosStart + 1;
        }
        else
        {
          unsigned uChar = 0;
          const char* szEsc = sEsc.c_str();
          STAFF_XML_ASSERT(*szEsc == '#', "Invalid sequence found [" + sEsc +
                           "] while unescaping string [" + sString + "]", m_sFileName, m_nLine);

          ++szEsc;
          if (*szEsc == 'x')
          {
            // hex
            ++szEsc;
            FromHexCString(szEsc, uChar);
          }
          else
          {
            // dec
            FromCString(szEsc, uChar);
          }

          STAFF_XML_ASSERT((uChar >= 0x20 && uChar < 0xFFFFFF) ||
                           uChar == 0x09 || uChar == 0x0a || uChar == 0x0d,
                           "Invalid sequence found [" + sEsc + "] while unescaping string [" +
                           sString + "]", m_sFileName, m_nLine);

          if (uChar <= 0xFF)
          {
            sString.replace(nPosStart, nPosEnd - nPosStart + 1, 1, static_cast<char>(uChar));
            nPosEnd = nPosStart + 1;
          }
          else
          {
            char achUtf16[4];
            char achUtf8[4];
            unsigned char nPos = 0;
            unsigned long nUtf8Size = sizeof(achUtf8);

            for (; uChar; ++nPos, uChar = uChar / 0x0100)
            {
              achUtf16[3 - nPos] = static_cast<char>(uChar);
            }

            if (!m_pCharsetConvertor)
            {
              m_pCharsetConvertor = new CharsetConverter("UTF-16", m_sEncoding.c_str());
            }

            bool bConverted =
                m_pCharsetConvertor->Convert(&achUtf16[3 - nPos], nPos, achUtf8, nUtf8Size, &nUtf8Size);
            if (!bConverted || !nUtf8Size)
            {
              static const char szReplacer[] = "�";
              sString.replace(nPosStart, nPosEnd - nPosStart + 1, szReplacer, sizeof(szReplacer) - 1);
              nPosEnd = nPosStart + sizeof(szReplacer) - 1;
            }
            else
            {
              sString.replace(nPosStart, nPosEnd - nPosStart + 1, achUtf8, nUtf8Size);
              nPosEnd = nPosStart + nUtf8Size;
            }

          }
        }
      }
    }


    bool ReadChar(char& rChar)
    {
      m_rStream.get(rChar);
      if (rChar == '\n')
      {
        ++m_nLine;
      }

      return true;
    }

    char ReadChar()
    {
      char chData = 0;
      m_rStream.get(chData);
      if (chData == '\n')
      {
        ++m_nLine;
      }
      return chData;
    }

    void SkipWhitespace()
    {
      char chData = 0;
      static const std::string sWhitespace = " \t\n\r";

      for (;;)
      {
        chData = m_rStream.peek();
        if (sWhitespace.find(chData) == std::string::npos)
        {
          break;
        }
        if (chData == '\n')
        {
          ++m_nLine;
        }
        m_rStream.ignore();
      }
    }

    void ReadStringBeforeChr(std::string& sString, const std::string& sCharset =
        " \t\n\r!\"#$%&\'()*+,/;<=>?@[\\]^`{|}~")
    {
      char chData = '\0';
      sString.erase();
      for (;;)
      {
        chData = m_rStream.peek();
        if (sCharset.find(chData) != std::string::npos)
        {
          break;
        }
        if (chData == '\n')
        {
          ++m_nLine;
        }
        sString.append(1, chData);
        m_rStream.ignore();
      }
    }

    void ReadStringWithChr(std::string& sString, const char chEnd)
    {
      char chData = '\0';
      sString.erase();
      for (;;)
      {
        chData = m_rStream.peek();
        if (chEnd == chData)
        {
          m_rStream.ignore();
          break;
        }
        if (chData == '\n')
        {
          ++m_nLine;
        }
        sString.append(1, chData);
        m_rStream.ignore();
      }
    }

    void ReadStringWithStr(std::string& sString, const char* szEnd)
    {
      sString.erase();
      char chTmp = '\0';
      const char* szPos = NULL;

      for (;;)
      {
        szPos = szEnd;
        for (;;)
        {
          ReadChar(chTmp);
          if (chTmp == *szPos)
          {
            break;
          }
          sString.append(1, chTmp);
        }

        // first matched char found
        for (;;)
        {
          ++szPos;
          if (!*szPos) // found
          {
            return;
          }

          // read and compare next char
          ReadChar(chTmp);
          if (chTmp != *szPos)
          {
            // adding matched chars
            sString.append(szEnd, szPos - szEnd);
            // continuing search
            break;
          }
        }
      }
    }

    bool Test(const char* szString)
    {
      char chTmp = '\0';
      const char* szPos = szString;
      unsigned nReadedLines = 0;
      for (; *szPos; ++szPos)
      {
        chTmp = m_rStream.peek();

        if (chTmp != *szPos)
        {
          if (szPos != szString)
          {
            m_rStream.seekg(static_cast<std::istream::off_type>(szString - szPos), std::ios::cur);
          }
          return false;
        }

        m_rStream.ignore();
        if (chTmp == '\n')
        {
          ++nReadedLines;
        }
      }

      m_nLine += nReadedLines;

      return true;
    }

    bool HasText(const std::string& sString)
    {
      const char* szWhitespace = " \t\n\r";
      return sString.find_first_not_of(szWhitespace) != std::string::npos;
    }

    bool ValidateId(const std::string& sId)
    {
      static const std::string sNonId = " \t\n\r!\"#$%&\'()*+,/;<=>?@[\\]^`{|}~";

      if (sId.empty())
      {
        return false;
      }

      if (sId.find_first_of(sNonId) != std::string::npos)
      {
        return false;
      }

      const char chFirst = sId[0];
      if (chFirst == '.' || chFirst == '-' || (chFirst >= '0' && chFirst <= '9'))
      {
        return false;
      }

      return true;
    }

  public:
    std::istream& m_rStream;
    std::string m_sFileName;
    std::string m_sEncoding;
    bool m_bStripWhitespace;
    unsigned m_nLine;
    CharsetConverter* m_pCharsetConvertor;
  };


  XmlReader::XmlReader(std::istream& rStream, bool bStripWhitespace):
    m_pImpl(new XmlReaderImpl(rStream, bStripWhitespace))
  {
  }

  XmlReader::~XmlReader()
  {
    delete m_pImpl;
  }

  void XmlReader::SetEncoding(const std::string& sEncoding)
  {
    m_pImpl->m_sEncoding = sEncoding;
  }

  void XmlReader::SetFileName(const std::string& sFileName)
  {
    m_pImpl->m_sFileName = sFileName;
  }

  void XmlReader::ReadDocument(Document& rDocument)
  {
    try
    {
      m_pImpl->m_nLine = 0;
      m_pImpl->m_rStream.exceptions(std::ios::failbit | std::ios::badbit);
      m_pImpl->m_nLine = 1;
      m_pImpl->ReadDeclaration(rDocument.GetDeclaration());
      if (!m_pImpl->m_sEncoding.empty())
      {
        rDocument.GetDeclaration().SetEncoding(m_pImpl->m_sEncoding);
      }
      else
      {
        m_pImpl->m_sEncoding = rDocument.GetDeclaration().GetEncoding();
      }
      m_pImpl->SkipWhitespace();
      std::string sComment;
      bool bSkip = false;
      // skip comments before root element node
      do
      {
        bSkip = false;
        if (m_pImpl->Test("<!--"))
        {
          m_pImpl->ReadStringWithStr(sComment, "-->");
          bSkip = true;
        }
        m_pImpl->SkipWhitespace();
        if (m_pImpl->Test("<?"))
        {
          m_pImpl->ReadStringWithStr(sComment, "?>");
          bSkip = true;
        }
        m_pImpl->SkipWhitespace();
      }
      while (bSkip);
      m_pImpl->ReadElement(rDocument.GetRootElement());
    }
    catch (const Exception&)
    {
      throw;
    }
    catch (const std::exception& rEx)
    {
      STAFF_XML_THROW("Error while parsing file: " + Error::GetLastErrorStr() + "(" +
                      std::string(rEx.what()) + ")", m_pImpl->m_sFileName, m_pImpl->m_nLine);
    }
  }

  void XmlReader::ReadDeclaration(Declaration& rDeclaration)
  {
    m_pImpl->ReadDeclaration(rDeclaration);
  }

  void XmlReader::ReadElement(Element& rElement)
  {
    m_pImpl->ReadElement(rElement);
  }


  XmlFileReader::XmlFileReader(const std::string& sFileName, bool bStripWhitespace):
    XmlReader(m_ifStream, bStripWhitespace)
  {
    SetFileName(sFileName);
    m_ifStream.open(sFileName.c_str());
  }

} // namespace xml
} // namespace staff
