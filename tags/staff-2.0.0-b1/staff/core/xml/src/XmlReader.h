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

#ifndef _STAFF_XML_XMLREADER_H_
#define _STAFF_XML_XMLREADER_H_

#include <string>
#include <fstream>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  class Document;
  class Declaration;
  class Element;

  //! xml reader
  class STAFF_XML_EXPORT XmlReader
  {
  public:
    //! constructor
    /*! \param  rStream - input stream
        \param  bStripWhitespace - ignore human-readable xml formatting
      */
    XmlReader(std::istream& rStream, bool bStripWhitespace = true);

    //! destructor
    ~XmlReader();

    //! set xml document encoding
    /*! if document encoding is set, encoding value from declaration will be overriden
        else will be used value from declaration or "UTF-8" if encoding attribute is missing
        \param  sEncoding - xml document encoding
      */
    void SetEncoding(const std::string& sEncoding = "");

    //! set file name
    /*! \param  sFileName - file name
      */
    void SetFileName(const std::string& sFileName);

    //! read xml document
    /*! \param  rDocument - xml document
      */
    void ReadDocument(Document& rDocument);

    //! read xml declaration
    /*! \param  rDeclaration - xml declaration
      */
    void ReadDeclaration(Declaration& rDeclaration);

    //! read element
    /*! \param  rElement - element
      */
    void ReadElement(Element& rElement);

  private:
    class XmlReaderImpl;
    XmlReaderImpl* m_pImpl;
  };


  //! xml file writer
  class STAFF_XML_EXPORT XmlFileReader: public XmlReader
  {
  public:
    //! constructor
    /*! \param  sFileName - file name
        \param  bStripWhitespace - ignore human-readable xml formatting
      */
    XmlFileReader(const std::string& sFileName, bool bStripWhitespace = true);

  private:
    std::ifstream m_ifStream;
  };

} // namespace xml
} // namespace staff

#endif // #ifndef _STAFF_XML_XMLREADER_H_
