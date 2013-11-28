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

#ifndef _STAFF_XML_NAMESPACE_H_
#define _STAFF_XML_NAMESPACE_H_

#include <string>
#include <iosfwd>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  //! xml namespace
  class STAFF_XML_EXPORT Namespace
  {
  public:
    //! copy constructor
    Namespace(const Namespace& rNamespace);

    //! constructor
    /*! \param sPrefix - prefix
        \param sUri - uri
      */
    Namespace(const std::string& sPrefix, const std::string& sUri);


    //! copy operator
    /*! \param  rNamespace - source namespace
        \return *this
        */
    Namespace& operator=(const Namespace& rNamespace);


    //! get prefix
    /*! \return prefix
      */
    const std::string& GetPrefix() const;

    //! set prefix
    /*! \param sPrefix - prefix
      */
    void SetPrefix(const std::string& sPrefix);


    //! get uri
    /*! \return uri
      */
    const std::string& GetUri() const;

    //! set uri
    /*! \param sUri - uri
      */
    void SetUri(const std::string& sUri);


    //! get next sibling namespace
    /*! \return next sibling namespace
      */
    const Namespace* GetNextSibling() const;

    //! get next sibling namespace
    /*! \return next sibling namespace
      */
    Namespace* GetNextSibling();


    //! check whether the namespaces are equal
    /*! \param  rNamespace - other namespace
        \return true, if namespaces are equal
    */
    bool operator==(const Namespace& rNamespace) const;

    //! check whether the namespaces are not equal
    /*! \param  rNamespace - other namespace
        \return true, if namespaces are not equal
    */
    bool operator!=(const Namespace& rNamespace) const;

  private:
    std::string m_sPrefix;        //!< prefix
    std::string m_sUri;           //!< uri
    Namespace* m_pNextSibling;  //!< next namespace
    friend class Element;
  };

} // namespace xml
} // namespace staff

#endif // _STAFF_XML_NAMESPACE_H_
