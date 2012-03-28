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

#ifndef _STAFF_XML_DOCUMENT_H_
#define _STAFF_XML_DOCUMENT_H_

#include "Declaration.h"
#include "Element.h"
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  //! XML Document
  class STAFF_XML_EXPORT Document
  {
  public:
    //! get document declaration
    /*! \return document declaration
        */
    const Declaration& GetDeclaration() const;

    //! get document declaration
    /*! \return document declaration
        */
    Declaration& GetDeclaration();

    //! get root Element
    /*! \return root Element
        */
    const Element& GetRootElement() const;

    //! get root node
    /*! \return root node
        */
    Element& GetRootElement();

  private:
    Declaration m_tDeclaration;  //!< xml-declaration
    Element m_tRootElement;      //!< root element
  };

} // namespace xml
} // namespace staff


#endif // _STAFF_XML_DOCUMENT_H_
