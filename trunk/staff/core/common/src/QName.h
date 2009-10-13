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

#ifndef _QNAME_H_
#define _QNAME_H_

#include "staffcommonexport.h"
#include <string>

typedef struct axutil_qname axutil_qname_t;

namespace staff
{

  //! Qualified name
  class STAFF_COMMON_EXPORT CQName
  {
  public:
    //!         default constructor
    CQName();

    //!         initializing constructor
    /*! \param  pQName - existing AxiOM QName
        */
    explicit CQName(axutil_qname_t* pQName);

    //!        constructor with ownership transfer
    /*! \param  rstQName - existing QName
        */
    CQName(CQName& rstQName);

    //!         initializing constructor
    /*! \param  sLocalPart - local part (may contain prefix)
        \param  sNamespaceUri - namespace URI
        */
    CQName(const std::string& sLocalPart, const std::string& sNamespaceUri);

    //!         initializing constructor
    /*! \param  sLocalPart - local part
        \param  sNamespaceUri - namespace URI
        \param  sPrefix - prefix
        */
    CQName(const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix);

    //!         destructor
    ~CQName();

    //!         ownership transfer operator
    /*! \param  rQName - source QName
        \return reference to current QName
        */
    CQName& operator=( CQName& rQName );

    //!         initialization operator
    /*! \param  pQName - AxiOM QName
        \return reference to current QName
        */
    CQName& operator=( axutil_qname_t* pQName );

    //!         test target QName for equality with specified QName
    /*! compare only local part and URI
        \param  rstQName - QName
        \return true - QNames are equals
        */
    bool operator==(const CQName& rstQName) const;

    //!         test target QName for inequality with specified QName
    /*! \param  rstQName - QName
        \return true - QNames are inequals
        */
    bool operator!=(const CQName& rstQName) const;

    //!         test target QName for equality with specified QName
    /*! \param  pQName - QName
        \return true - QNames are equals
        */
    bool operator==(axutil_qname_t* pQName) const;

    //!         test target QName for inequality with specified QName
    /*! \param  pQName - QName
        \return true - QNames are inequals
        */
    bool operator!=(axutil_qname_t* pQName) const;

    //!         get local part
    /*! \return local part
    */
    std::string GetLocalPart() const;
    
    //!         get namespace URI
    /*! \return namespace URI
    */
    std::string GetNamespaceUri() const;
    
    //!         get prefix
    /*! \return prefix
    */
    std::string GetPrefix() const;

    // create/delete
    
    //!         create QName with ownership flag
    /*! \param  sLocalPart - local part
        \param  sNamespaceUri - namespace URI
        \param  sPrefix - prefix
        \return reference to current QName
        */
    CQName& Create(const std::string& sLocalPart, const std::string& sNamespaceUri, const std::string& sPrefix = "");

    //!         free QName (even if ownership flag is not set)
    void Free();

    //!         attach QName object to AxiOM qname
    /*! \param  pQName - AxiOM qname
        \return reference to current QName
        */
    CQName& Attach(axutil_qname_t* pQName);

    //!         detach QName object from AxiOM qname
    /*! if ownership flag is set, QName will be freed */
    void Detach();

    //!         create QName clone from given
    /*! \param  rQName - source object
        \return reference to current QName
        */
    CQName& Clone(const CQName& rQName);
    
    //!         create QName clone
    /*! \return QName clone
    */
    CQName Clone();

    //!         string cast operator
    /*! \return prefix:localPart
    */
    operator std::string() const;

    //!         string cast
    /*! \return prefix:localPart
    */
    std::string ToString() const;

    //!         axiom qname cast operator
    /*! \raturn axiom qname
      */
    operator axutil_qname_t*() const;

    //!         axiom qname cast operator
    /*! \raturn axiom qname
      */
    operator axutil_qname_t*();

  private:
    bool m_bOwner;                   //!<  ownership flag
    axutil_qname_t* m_pAxutilQName;  //!<  AxiOM qname
  };

} // namespace staff

#endif // _QNAME_H_
