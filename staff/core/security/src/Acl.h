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

#ifndef _ACL_H_
#define _ACL_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    enum EAccess
    {
      EAccessDenied,
      EAccessGranted,
      EAccessInherited
    };

    class CAcl
    {
    public:
      static CAcl& Inst();

      static void FreeInst();
      
      void SetUserAccess(int nObjectId, EAccess eAccess);
      void SetUserAccess(int nObjectId, int nUserId, EAccess eAccess);
      void GetUserAccess(int nObjectId, int nUserId, EAccess& reAccess);

      void SetGroupAccess(int nObjectId, EAccess eAccess);
      void SetGroupAccess(int nObjectId, int nGroupId, EAccess eAccess);
      void GetGroupAccess(int nObjectId, int nGroupId, EAccess& reAccess);

      bool CalculateUserAccess(int nObjectId, int nUserId);
      bool CalculateUserAccess(const std::string& sObjectPath, int nUserId);

    private:
      CAcl();
      ~CAcl();

    private:
      static CAcl* m_pInst;
    };
  }
}

#endif // _ACL_H_
