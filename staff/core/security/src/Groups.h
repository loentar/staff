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

#ifndef _GROUPS_H_
#define _GROUPS_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SGroup
    {
      int nId;
      std::string sName;
      std::string sDescription;
    };

    typedef std::list<SGroup> TGroupsList;

    class CGroups
    {
    public:
      static CGroups& Inst();

      static void FreeInst();

      void GetById(int nId, SGroup& rstGroup);

      void GetByName(const std::string& sGroupName, SGroup& rstGroup);

      void GetList(TGroupsList& rlsGroups);

      void Add(const std::string& sName, const std::string& sDescription, int& nId);

      void Remove(int nId);

      void SetDescription(int nId, const std::string& sDescription);

    private:
      CGroups();
      ~CGroups();

    private:
      static CGroups* m_pInst;
    };
  }
}

#endif // _GROUPS_H_
