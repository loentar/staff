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

#ifndef _USERS_H_
#define _USERS_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SUser
    {
      int nId;
      std::string sName;
      std::string sDescription;
    };

    typedef std::list<SUser> TUsersList;

    class CUsers
    {
    public:
      static CUsers& Inst();

      static void FreeInst();

      void GetById(int nId, SUser& rstUser);

      void GetByName(const std::string& sUserName, SUser& rstUser);

      void GetList(TUsersList& rlsUsers);

      void Add(const std::string& sName, const std::string& sPassword, const std::string& sDescription, int& nId);

      void Remove(int nId);

      void SetPassword(int nId, const std::string& sPassword);

      void SetDescription(int nId, const std::string& sDescription);

    private:
      CUsers();
      ~CUsers();

    private:
      static CUsers* m_pInst;
    };
  }
}

#endif // _USERS_H_
