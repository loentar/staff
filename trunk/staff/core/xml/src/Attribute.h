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

#ifndef _STAFF_XML_ATTRIBUTE_H_
#define _STAFF_XML_ATTRIBUTE_H_

#include <string>
#include <iosfwd>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  //! xml-attribute
  class STAFF_XML_EXPORT Attribute
  {
  public:
    //! copy constructor
    /*! \param  rAttr - source attribute
        */
    Attribute(const Attribute& rAttr);

    //! initializing constructor
    /*! \param  sName - attribute name
        */
    Attribute(const std::string& sName);

    //! initializing constructor
    /*! \param  sName - attribute name
        \param  sValue - attribute value
        */
    Attribute(const std::string& sName, const std::string& sValue);

    //! initializing constructor
    /*! \param  sName - attribute name
        \param  sValue - attribute value
        \param  sPrefix - attribute prefix
        */
    Attribute(const std::string& sName, const std::string& sValue, const std::string& sPrefix);


    //! get prefix
    /*! \return prefix
      */
    const std::string& GetPrefix() const;

    //! set prefix
    /*! \param sPrefix - sPrefix
      */
    void SetPrefix(const std::string& sPrefix);

    //! get name
    /*! \return name
      */
    const std::string& GetName() const;

    //! set name
    /*! \param sName - name
      */
    void SetName(const std::string& sName);

    //! get attribute name with prefix
    /*! \return attribute name with prefix
      */
    std::string GetPrefixName() const;

    //! get value
    /*! \return value
      */
    const std::string& GetValue() const;

    //! set value
    /*! \param sValue - value
      */
    void SetValue(const std::string& sValue);


    //! get next sibling attribute
    /*! \return next sibling attribute
      */
    const Attribute* GetNextSibling() const;

    //! get next sibling attribute
    /*! \return next sibling attribute
      */
    Attribute* GetNextSibling();


    //! copy operator
    /*! \param  rAttr - source attribute
        \return *this
    */
    Attribute& operator=(const Attribute& rAttr);

    //! check whether the attributes are equal
    /*! \param  rAttr - other attribute
        \return true, if attributes are equal
    */
    bool operator==(const Attribute& rAttr) const;

    //! check whether the attributes are not equal
    /*! \param  rAttr - other attribute
        \return true, if attributes are not equal
    */
    bool operator!=(const Attribute& rAttr) const;

  private:
    std::string m_sPrefix;       //!< attribute namespace prefix
    std::string m_sName;         //!< attribute name
    std::string m_sValue;        //!< attribute value
    Attribute* m_pNextSibling; //!< next attribute
    friend class Element;
  };

} // namespace xml
} // namespace staff

#endif // _STAFF_XML_ATTRIBUTE_H_

