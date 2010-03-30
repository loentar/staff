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

#ifndef _ProfileAdmin_h_
#define _ProfileAdmin_h_

#include <string>
#include <list>
#include <staff/common/IService.h>

namespace webapp
{
  namespace admin
  {
    typedef std::list<std::string> TStringList; //!< string list

    //! widget description
    struct SWidget
    {
      std::string sClass; //!< widget class
      std::string sName;  //!< widget name
    };

    typedef std::list<SWidget> TWidgetList; //!< widget list

    //! profile
    struct SProfile
    {
      std::string sId;        //!< profile id
      std::string sName;      //!< profile name
      bool bIsAdmin;          //!< is admin profile
      TStringList lsWidgets;  //!< widgets list
    };

    typedef std::list<SProfile> TProfileList; //!< profile list

    //! base profile administrator
    class CProfileAdmin: public staff::IService
    {
    public:
      virtual ~CProfileAdmin() {}

      //!         get profile list
      /*! \return profile list
          */
      virtual TProfileList GetProfiles() = 0;

      //!         get widget list
      /*! \return widgets list
          */
      virtual TWidgetList GetWidgets() = 0;

      //!         remove profile by name
      /*! \param  sName - profile id
          */
      virtual void RemoveProfile(const std::string& sId) = 0;

      //!         set(create) profile
      /*! \param  rProfile - profile
          */
      virtual void SetProfile(const SProfile& rProfile) = 0;
    };
  }
}

#endif // _ProfileAdmin_h_
