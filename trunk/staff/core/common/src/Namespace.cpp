#include <axiom_namespace.h>
#include "Exception.h"
#include "Runtime.h"
#include "DataObject.h"
#include "QName.h"
#include "Value.h"
#include "Namespace.h"

namespace staff
{
  CNamespace::CNamespace():
    m_bOwner(false),
    m_pDataObject(NULL), 
    m_pAxiomNamespace(NULL)
  {
  }

  CNamespace::CNamespace( CNamespace& rNamespace ):
    m_bOwner(rNamespace.m_bOwner),
    m_pDataObject(rNamespace.m_pDataObject), 
    m_pAxiomNamespace(rNamespace.m_pAxiomNamespace)
  {
    rNamespace.m_bOwner = false;
  }

  CNamespace::CNamespace( axiom_namespace_t* pAxiomNamespace, CDataObject* pDataObject /*= NULL*/, bool bOwner /*= false*/ ):
    m_bOwner(false),
    m_pDataObject(NULL),
    m_pAxiomNamespace(NULL)
  {
    Attach(pAxiomNamespace, pDataObject, bOwner);
  }

  CNamespace::CNamespace( const std::string& sUri, const std::string& sPrefix ):
    m_bOwner(true),
    m_pDataObject(NULL)
  {
    m_pAxiomNamespace = axiom_namespace_create(m_pEnv, sUri.c_str(), sPrefix.c_str());
  }

  void CNamespace::Attach(axiom_namespace_t* pAxiomNamespace, CDataObject* pDataObject /*= NULL*/, bool bOwner /*= false*/)
  {
    Detach();
    m_pAxiomNamespace = pAxiomNamespace;
    m_pDataObject = pDataObject;
    m_bOwner = bOwner;
  }

  void CNamespace::Detach()
  {
    if (m_bOwner)
    {
      Free();
      m_bOwner = false;
    }
  }

  void CNamespace::Free()
  {
    if(m_pAxiomNamespace != NULL)
    {
      axiom_namespace_free(m_pAxiomNamespace, m_pEnv);
    }

    m_pAxiomNamespace = NULL;
    m_pDataObject = NULL;
  }

  CNamespace CNamespace::Clone() const
  {
    CNamespace tNamespace(axiom_namespace_clone(m_pAxiomNamespace, m_pEnv));
    return tNamespace;
  }

  const CNamespace& CNamespace::Clone(CNamespace& rNamespace) const
  {
    rNamespace.Attach(axiom_namespace_clone(m_pAxiomNamespace, m_pEnv));
    return *this;
  }

  std::string CNamespace::GetPrefix() const
  {
    RISE_ASSERTES(m_pAxiomNamespace != NULL, CDomNoItemException, "Uninitialized namespace");

    axis2_char_t* szPrefix = axiom_namespace_get_prefix(m_pAxiomNamespace, m_pEnv);
    return szPrefix == NULL ? "" : szPrefix;
  }

  std::string CNamespace::GetUri() const
  {
    RISE_ASSERTES(m_pAxiomNamespace != NULL, CDomNoItemException, "Uninitialized namespace");

    axis2_char_t* szUri = axiom_namespace_get_uri(m_pAxiomNamespace, m_pEnv);
    RISE_ASSERTES(szUri != NULL, CDomNoItemException, "Error while getting namespace URI");

    return szUri;
  }

  CDataObject* CNamespace::GetDataObject() const
  {
    return m_pDataObject;
  }

  void CNamespace::SetDataObject(CDataObject* pDataObject)
  {
    m_pDataObject = pDataObject;
    m_bOwner = false;
  }

  bool CNamespace::IsOwner() const
  {
    return m_bOwner;
  }

  void CNamespace::SetOwner(bool bOwner)
  {
    m_bOwner = bOwner;
  }

  bool CNamespace::IsNull()
  {
    return m_pAxiomNamespace == NULL;
  }

  CNamespace& CNamespace::operator=( CNamespace& rNamespace )
  {
    m_bOwner = rNamespace.m_bOwner;
    m_pDataObject = rNamespace.m_pDataObject;
    m_pAxiomNamespace = rNamespace.m_pAxiomNamespace;
    rNamespace.m_bOwner = false;
    return *this;
  }

  bool CNamespace::operator==( const CNamespace& rNamespace ) const
  {
    return axiom_namespace_equals(m_pAxiomNamespace, m_pEnv, rNamespace.m_pAxiomNamespace) == AXIS2_TRUE;
  }

  bool CNamespace::operator!=( const CNamespace& rNamespace ) const
  {
    return !(operator==(rNamespace));
  }

  CNamespace::operator axiom_namespace_t*()
  {
    return m_pAxiomNamespace;
  }

  CNamespace* CNamespace::operator->()
  {
    return this;
  }

  const CNamespace* CNamespace::operator->() const
  {
    return this;
  }

  axutil_env_t* CNamespace::m_pEnv = CRuntime::Inst().GetAxis2Env();

} // namespace staff

