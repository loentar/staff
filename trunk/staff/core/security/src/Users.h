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

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    //! user
    struct STAFF_SECURITY_EXPORT SUser
    {
      int nId;                  //!< user id
      std::string sName;        //!< user name
      std::string sDescription; //!< user description
    };

    typedef std::list<SUser> TUsersList; //!< users list

    //! users
    class STAFF_SECURITY_EXPORT CUsers
    {
    public:
      //! get users instance
      /*! \return users instance
        */
      static CUsers& Inst();

      //! free users instance
      static void FreeInst();

      //! get user by id
      /*! \param nId - user id
          \param rstUser - resulting user
          */
      void GetById(int nId, SUser& rstUser);

      //! get user by name
      /*! \param sUserName - user name
          \param rstUser - resulting user
          */
      void GetByName(const std::string& sUserName, SUser& rstUser);

      //! get user list
      /*! \param rlsUsers - resulting users list
        */
      void GetList(TUsersList& rlsUsers);

      //! create new user
      /*! \param sName - user name
          \param sPassword - user password
          \param sDescription - user description
          \param nId - created user id
          */
      void Add(const std::string& sName, const std::string& sPassword, const std::string& sDescription, int& nId);

      //! delete user
      /*! \param nId - user id
        */
      void Remove(int nId);

      //! set user password
      /*! \param nId - user id
          \param sPassword - user password
        */
      void SetPassword(int nId, const std::string& sPassword);

      //! get user password
      /*! \param nId - user id
          \param sPassword - resulting user password
        */
      void GetPassword(int nId, std::string& sPassword);

      //! set user description
      /*! \param nId - user id
          \param sDescription - user description
        */
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
