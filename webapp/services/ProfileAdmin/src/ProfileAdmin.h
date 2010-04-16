#ifndef _ProfileAdmin_h_
#define _ProfileAdmin_h_

#include <string>
#include <list>

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
    class CProfileAdmin
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
