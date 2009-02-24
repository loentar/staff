#include <rise/common/Log.h>
#include <rise/common/Exception.h>
#include <rise/common/exmacros.h>
#include <rise/string/String.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDocument.h>
#include "Runtime.h"
#include "Config.h"

#ifdef WIN32
#pragma warning (disable:4996)
#endif

namespace staff
{
  class CConfig::CConfigImpl
  {
  public:
    rise::xml::CXMLDocument m_tConfig;
  };

  CConfig::CConfig()
  {
    m_pImpl = new CConfigImpl;

    try
    {
      m_pImpl->m_tConfig.LoadFromFile(CRuntime::Inst().GetStaffHome() + "/staff.xml");
    }
    RISE_CATCH_ALL
  }

  CConfig::~CConfig()
  {
    if (m_pImpl != NULL)
    {
      delete m_pImpl;
      m_pImpl = NULL;
    }
  }

  rise::xml::CXMLNode& CConfig::GetModuleConfig( const rise::CString& sModuleName )
  {
    return m_pImpl->m_tConfig.GetRoot().Subnode(sModuleName);
  }

  CConfig& CConfig::Inst()
  {
    if (m_pInst == NULL)
      m_pInst = new CConfig;
    return *m_pInst;
  }

  CConfig* CConfig::m_pInst = NULL;
}
