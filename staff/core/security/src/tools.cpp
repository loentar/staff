/*
 *  Copyright 2010 Utkin Dmitry
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

#include <rise/common/ExceptionTemplate.h>
#include <rise/common/exmacros.h>
#include <rise/common/Log.h>
#include <staff/sqlite3/sqlite3.h>
#include "DbConn.h"
#include "Sessions.h"
#include "Acl.h"
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
      rise::LogDebug() << "calculate access to [" << szObjectPath << "] = " << *pnAccess;
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
