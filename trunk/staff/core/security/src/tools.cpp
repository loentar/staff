#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include "DbConn.h"
#include "Sessions.h"
#include "Acl.h"
#include "sqlite3.h"
#include "tools.h"

int staff_security_init()
{
  int nResult = 0;
  try
  {
    staff::security::CDbConn::Open();
    nResult = 1;
  }
  RISE_CATCH_ALL;

  return nResult;
}

void staff_security_free()
{
  staff::security::CDbConn::Close();
}

int staff_security_calculate_access_by_session_id(const char* szObjectPath, const char* szSessionId, int* pnAccess)
{
  if ((szObjectPath == NULL) || (szSessionId == NULL) || (pnAccess == NULL))
  {
    return -1;
  }

  int nResult = 0;
  *pnAccess = 0;
  try
  {
    int nUserId = -1;
    if (staff::security::CSessions::Inst().GetUserId(szSessionId, nUserId))
    {
      if( staff::security::CAcl::Inst().CalculateUserAccess(szObjectPath, nUserId))
      {
        *pnAccess = 1;
      }
      rise::LogInfo() << "calculate access to [" << szObjectPath << "] = " << *pnAccess;
      nResult = 1;
    }
    else
    {
      rise::LogWarning() << "Can't get userid for sessionid [" << szSessionId
          << "] for access to object [" << szObjectPath << "]";
    }
  }
  RISE_CATCH_ALL;

  return nResult;
}
