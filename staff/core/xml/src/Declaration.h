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

#ifndef _STAFF_XML_DECLARATION_H_
#define _STAFF_XML_DECLARATION_H_

#include <string>
#include <iosfwd>
#include "staffxmlexport.h"

namespace staff
{
namespace xml
{
  //! xml-document declaration
  class STAFF_XML_EXPORT Declaration
  {
  public:
    enum Standalone //! standalone
    {
      StandaloneUndefined, //!< not defined
      StandaloneNo,        //!< standalone="no"
      StandaloneYes        //!< standalone="yes"
    };

  public:
    //! constructor
    Declaration();

    //! destructor
    ~Declaration();

    //! get version
    /*! \return version
      */
    const std::string& GetVersion() const;

    //! set version
    /*! \param sVersion - version
      */
    void SetVersion(const std::string& sVersion);


    //! get encoding
    /*! \return encoding
      */
    const std::string& GetEncoding() const;

    //! set encoding
    /*! \param sEncoding - encoding
      */
    void SetEncoding(const std::string& sEncoding);


    //! get stand alone
    /*! \return stand alone
      */
    Standalone GetStandalone() const;

    //! set stand alone
    /*! \param eStandalone - standalone
      */
    void SetStandalone(Standalone eStandalone);


  private:
    std::string  m_sVersion;      //!< xml document version(1.0)
    std::string  m_sEncoding;     //!< encoding
    Standalone   m_eStandalone;   //!< standalone declaration
  };

} // namespace xml
} // namespace staff

#endif // _STAFF_XML_DECLARATION_H_

