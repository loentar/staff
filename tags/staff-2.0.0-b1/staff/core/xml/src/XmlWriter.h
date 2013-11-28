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

#ifndef _STAFF_XML_XMLWRITER_H_
#define _STAFF_XML_XMLWRITER_H_

#include <string>
#include <fstream>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  class Document;
  class Declaration;
  class Node;
  class Element;
  class Comment;
  class Text;
  class Cdata;
  class Attribute;
  class Namespace;

  //! xml writer
  class STAFF_XML_EXPORT XmlWriter
  {
  public:
    //! constructor
    /*! \param  rStream - output stream
        \param  bEnableFormatting - produce human-readable xml
      */
    XmlWriter(std::ostream& rStream, bool bEnableFormatting = true);

    void WriteDocument(const Document& rDocument);
    void WriteDeclaration(const Declaration& rDeclaration);
    void WriteNode(const Node& rNode);
    void WriteElement(const Element& rElement);
    void WriteComment(const Comment& rComment);
    void WriteText(const Text& rText);
    void WriteCdata(const Cdata& rCdata);
    void WriteAttribute(const Attribute& rAttribute);
    void WriteNamespace(const Namespace& rNamespace);

  private:
    std::string EscapeString(std::string sString);
    void WriteIndent();
    void WriteNewLine();

  private:
    std::ostream& m_rStream;
    bool m_bEnableFormatting;
    unsigned m_nIndent;
  };


  //! xml file writer
  class STAFF_XML_EXPORT XmlFileWriter: public XmlWriter
  {
  public:
    XmlFileWriter(const std::string& sFileName, bool bEnableFormatting = true);

  private:
    std::ofstream m_ofStream;
  };

} // namespace xml
} // namespace staff

#endif // #ifndef _STAFF_XML_XMLWRITER_H_
