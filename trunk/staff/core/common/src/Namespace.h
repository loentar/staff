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

#ifndef _NAMESPACE_H_
#define _NAMESPACE_H_

#include "staffcommonexport.h"
#include <string>

typedef struct axiom_namespace axiom_namespace_t;

namespace staff
{
  class CDataObject;

  //! Namespace
  class STAFF_COMMON_EXPORT CNamespace
  {
  public:
    //!         default constructor
    CNamespace();

    //!         owner pass constructor
    /*!         owner flag moved from rNamespace,
                rNamespace's owner flag will be reset
        \param  rNamespace - existing namespace
        */
    CNamespace(CNamespace& rNamespace);

    //!         initializing constructor (bound to dataobject or not)
    /*          
        \param  pAxiomNamespace - existing axiom namespace
        \param  pDataObject - bound dataobject or NULL
        \param  bOwner - owner flag (if set, namespace will be deleted while object destructs)
        */
    CNamespace(axiom_namespace_t* pAxiomNamespace, CDataObject* pDataObject = NULL, bool bOwner = false);

    //!         initializing constructor
    /*! \param  sUri - namespace URI
        \param  sPrefix - namespace prefix
        */
    CNamespace(const std::string& sUri, const std::string& sPrefix = "");

    //!         attach existing axiom namespace to object
    /*! \param  pAxiomNamespace - existing axiom namespace
        \param  pDataObject - bound dataobject or NULL
        \param  bOwner - owner flag
        */
    void Attach(axiom_namespace_t* pAxiomNamespace, CDataObject* pDataObject = NULL, bool bOwner = false);

    //!         detach (and free namespace if owner)
    void Detach();

    //!         free namespace
    void Free();

    //!         clone namespace
    /*! \return cloned namespace
    */
    CNamespace Clone() const;

    //!         clone namespace
    /*! \param  rNamespace - cloned namespace
    */
    const CNamespace& Clone(CNamespace& rNamespace) const;

    //!         get prefix
    /*! \return namespace prefix
    */
    std::string GetPrefix() const;

    //!         get URI
    /*! \return namespace URI
    */
    std::string GetUri() const;

    //!         get pointer to bound dataobject
    /*! \return get pointer to bound
    */
    CDataObject* GetDataObject() const;

    //!         set pointer to bound dataobject
    /*! resets owner flag
        \return set pointer to bound
    */
    void SetDataObject(CDataObject* pDataObject);

    //!         is owner
    /*! \return true if object is owner
    */
    bool IsOwner() const;

    //!         set object as owner
    /*! \param  bOwner - true: set a owner, false: reset owner flag
        */
    void SetOwner(bool bOwner);

    //!         test initialization
    /*! \return true, if object is not initialized
    */
    bool IsNull();

    //!         owner pass operator
    /*! \param  rNamespace - source namespace
        \return ref to current namespace
        */
    CNamespace& operator=(CNamespace& rNamespace);

    //!         equality check
    /*! \param  rNamespace - namespace to check with
        \return true - namespaces is equal
        */
    bool operator==(const CNamespace& rNamespace) const;

    //!         non equality check
    /*! \param  rNamespace - namespace to check with
        \return true - namespaces is not equal
        */
    bool operator!=(const CNamespace& rNamespace) const;

    //!         operator for conversion to axiom_namespace_t
    operator axiom_namespace_t*();

    CNamespace* operator->();
    const CNamespace* operator->() const;

  private:
    bool m_bOwner;
    CDataObject* m_pDataObject;
    axiom_namespace_t* m_pAxiomNamespace;
    static axutil_env_t* m_pEnv;
  };

} // namespace staff

#endif // _NAMESPACE_H_
