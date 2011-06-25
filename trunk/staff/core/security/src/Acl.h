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

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    //! user/group access to object
    enum EAccess
    {
      EAccessDenied,   //!< access is denied
      EAccessGranted,  //!< access is granted
      EAccessInherited //!< access is inherited
    };

    //! access control list
    class STAFF_SECURITY_EXPORT Acl
    {
    public:
      //! get acl instance
      /*! \return acl instance
        */
      static Acl& Inst();

      //! set access to object for all users
      /*! \param nObjectId - object id
          \param eAccess - access
          */
      void SetUserAccess(int nObjectId, EAccess eAccess);

      //! set access to object for specified user
      /*! \param nObjectId - object id
          \param nUserId - user id
          \param eAccess - access
          */
      void SetUserAccess(int nObjectId, int nUserId, EAccess eAccess);

      //! get access to object for specified user
      /*! \param nObjectId - object id
          \param nUserId - user id
          \param reAccess - result: access
          */
      void GetUserAccess(int nObjectId, int nUserId, EAccess& reAccess);


      //! set access to object for all groups
      /*! \param nObjectId - object id
          \param eAccess - access
          */
      void SetGroupAccess(int nObjectId, EAccess eAccess);

      //! set access to object for specified group
      /*! \param nObjectId - object id
          \param nGroupId - group id
          \param eAccess - access
          */
      void SetGroupAccess(int nObjectId, int nGroupId, EAccess eAccess);

      //! get access to object for specified group
      /*! \param nObjectId - object id
          \param nGroupId - group id
          \param reAccess - result: access
          */
      void GetGroupAccess(int nObjectId, int nGroupId, EAccess& reAccess);

      //! calculate access to object for user
      /*! \param nObjectId - object id
          \param nUserId - group id
          \return true - user have access, false - user does not have access
          */
      bool CalculateUserAccess(int nObjectId, int nUserId);

      //! calculate access to object for user by object name
      /*! \param sObjectPath - full object name (sample: "components.samples.calc.CalsService.Add")
          \param nUserId - group id
          \return true - user have access, false - user does not have access
          */
      bool CalculateUserAccess(const std::string& sObjectPath, int nUserId);

    private:
      Acl();
      ~Acl();
      Acl(const Acl&);
      Acl& operator=(const Acl&);
    };

#ifndef STAFF_NO_DEPRECATED
    typedef Acl CAcl STAFF_DEPRECATED(Acl);
#endif

  }
}

#endif // _ACL_H_
