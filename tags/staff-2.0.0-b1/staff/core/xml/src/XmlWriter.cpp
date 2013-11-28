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

#include "Exception.h"
#include "Document.h"
#include "Declaration.h"
#include "Attribute.h"
#include "Namespace.h"
#include "Node.h"
#include "Element.h"
#include "XmlWriter.h"

namespace staff
{
namespace xml
{

  XmlWriter::XmlWriter(std::ostream& rStream, bool bEnableFormatting):
    m_rStream(rStream), m_bEnableFormatting(bEnableFormatting), m_nIndent(0)
  {
  }

  void XmlWriter::WriteDocument(const Document& rDocument)
  {
    m_rStream.exceptions(std::ios::failbit | std::ios::badbit);
    WriteDeclaration(rDocument.GetDeclaration());
    WriteElement(rDocument.GetRootElement());
  }

  void XmlWriter::WriteDeclaration(const Declaration& rDeclaration)
  {
    m_rStream << "<?xml";
    WriteAttribute(Attribute("version", rDeclaration.GetVersion()));

    if (!rDeclaration.GetEncoding().empty())
    {
      WriteAttribute(Attribute("encoding", rDeclaration.GetEncoding()));
    }

    if (rDeclaration.GetStandalone() == Declaration::StandaloneYes)
    {
      WriteAttribute(Attribute("standalone", "yes"));
    }
    else
    if (rDeclaration.GetStandalone() == Declaration::StandaloneNo)
    {
      WriteAttribute(Attribute("standalone", "no"));
    }

    m_rStream << " ?>";
  }

  void XmlWriter::WriteNode(const Node& rNode)
  {
    switch (rNode.GetType())
    {
    case Node::TypeElement:
      WriteElement(rNode.GetElement());
      break;

    case Node::TypeText:
      WriteText(static_cast<const Text&>(rNode));
      break;

    case Node::TypeComment:
      WriteComment(static_cast<const Comment&>(rNode));
      break;

    case Node::TypeCdata:
      WriteCdata(static_cast<const Cdata&>(rNode));
      break;

    default:
      STAFF_THROW_ASSERT("Invalid Node Type: " + ToString(rNode.GetType()));
    }
  }

  void XmlWriter::WriteElement(const Element& rElement)
  {
    WriteIndent();
    m_rStream << "<" << rElement.GetPrefixName();

    // write namespaces
    for (const Namespace* pNamespace = rElement.GetFirstNamespace();
         pNamespace; pNamespace = pNamespace->GetNextSibling())
    {
      WriteNamespace(*pNamespace);
    }

    // write attributes
    for (const Attribute* pAttribute = rElement.GetFirstAttribute();
         pAttribute; pAttribute = pAttribute->GetNextSibling())
    {
      WriteAttribute(*pAttribute);
    }

    if (rElement.IsEmpty())
    {
      // end element
      m_rStream << "/>";
    }
    else
    {
      m_rStream << ">";

      if (rElement.IsLeaf())
      {
        WriteText(static_cast<const Text&>(*rElement.GetFirstChild()));
      }
      else
      {
        // write childs

        ++m_nIndent;
        for (const Node* pNode = rElement.GetFirstChild(); pNode; pNode = pNode->GetNextSibling())
        {
          WriteNode(*pNode);
        }
        --m_nIndent;
        WriteIndent();
      }
      m_rStream << "</" << rElement.GetPrefixName() << ">";
    }
  }

  void XmlWriter::WriteComment(const Comment& rComment)
  {
    WriteIndent();
    m_rStream << "<!--" << EscapeString(rComment.GetValue()) << "-->";
  }

  void XmlWriter::WriteText(const Text& rText)
  {
    m_rStream << EscapeString(rText.GetValue());
  }

  void XmlWriter::WriteCdata(const Cdata& rCdata)
  {
    WriteIndent();
    std::string sString(rCdata.GetValue());
    std::string::size_type nPos = 0;
    while ((nPos = sString.find("]]>", nPos)) != std::string::npos)
    {
      sString.replace(nPos, 3, "]]]]><![CDATA[>", 15);
      nPos += 15;
    }

    m_rStream << "<![CDATA[" << sString << "]]>";
  }

  void XmlWriter::WriteAttribute(const Attribute& rAttribute)
  {
    m_rStream << " " << rAttribute.GetPrefixName() << "=\"" << EscapeString(rAttribute.GetValue()) << "\"";
  }

  void XmlWriter::WriteNamespace(const Namespace& rNamespace)
  {
    if (rNamespace.GetPrefix().empty())
    {
      m_rStream << " xmlns";
    }
    else
    {
      m_rStream << " xmlns:" << rNamespace.GetPrefix();
    }

    m_rStream << "=\"" << EscapeString(rNamespace.GetUri()) << "\"";
  }

  std::string XmlWriter::EscapeString(std::string sString)
  {
    static const char* szEscapeChars = "<>&'\"";

    std::string::size_type nPos = 0;
    while ((nPos = sString.find_first_of(szEscapeChars, nPos)) != std::string::npos)
    {
      switch (sString[nPos])
      {
      case '<':
        sString.replace(nPos, 1, "&lt;", 4);
        nPos += 4;
        break;

      case '>':
        sString.replace(nPos, 1, "&gt;", 4);
        nPos += 4;
        break;

      case '&':
        sString.replace(nPos, 1, "&amp;", 5);
        nPos += 5;
        break;

      case '\'':
        sString.replace(nPos, 1, "&apos;", 6);
        nPos += 6;
        break;

      case '"':
        sString.replace(nPos, 1, "&quot;", 6);
        nPos += 6;
        break;
      }
    }

    return sString;
  }

  void XmlWriter::WriteIndent()
  {
    if (m_bEnableFormatting)
    {
      m_rStream << "\n";
      for (unsigned nIndent = m_nIndent; nIndent; --nIndent)
      {
        m_rStream << "  ";
      }
    }
  }

  void XmlWriter::WriteNewLine()
  {
    if (m_bEnableFormatting)
    {
      m_rStream << "\n";
    }
  }


  XmlFileWriter::XmlFileWriter(const std::string& sFileName, bool bEnableFormatting):
    XmlWriter(m_ofStream, bEnableFormatting)
  {
    m_ofStream.open(sFileName.c_str());
  }

} // namespace xml
} // namespace staff
