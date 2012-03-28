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

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    //! group
    struct STAFF_SECURITY_EXPORT Group
    {
      int nId;                  //!< group id
      std::string sName;        //!< group name
      std::string sDescription; //!< group description
    };

    typedef std::list<Group> GroupsList; //!< list of groups

    //! groups
    class STAFF_SECURITY_EXPORT Groups
    {
    public:
      //! get groups instance
      /*! \return groups instance
        */
      static Groups& Inst();

      //! get group by id
      /*! \param nId - group id
          \param rstGroup - resulting group info
          */
      void GetById(int nId, Group& rstGroup);

      //! get group by name
      /*! \param sGroupName
          \param rstGroup - resulting group info
          */
      void GetByName(const std::string& sGroupName, Group& rstGroup);

      //! get groups list
      /*! \param rlsGroups - resulting group list
          */
      void GetList(GroupsList& rlsGroups);


      //! create new group
      /*! \param sName - group name
          \param sDescription - group description
          \param nId - resulting group id
          */
      void Add(const std::string& sName, const std::string& sDescription, int& nId);

      //! remove group by id
      /*! \param nId - group id to remove
        */
      void Remove(int nId);

      //! set group description
      /*! \param nId - group id
          \param sDescription - new group description
        */
      void SetDescription(int nId, const std::string& sDescription);

    private:
      Groups();
      ~Groups();
      Groups(const Groups&);
      Groups& operator=(const Groups&);
    };

  }
}

#endif // _GROUPS_H_
