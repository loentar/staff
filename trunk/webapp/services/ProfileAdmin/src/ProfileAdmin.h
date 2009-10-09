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
      std::string sName;   //!< widget class name
      std::string sDescr;  //!< widget description
    };

    typedef std::list<SWidget> TWidgetList; //!< widget list

    //! profile
    struct SProfile
    {
      std::string sName;      //!< profile name
      TWidgetList lsWidgets;  //!< widgets list
    };

    //! base profile administrator
    class CProfileAdmin
    {
    public:
      virtual ~CProfileAdmin() {}

      //!         get profile list
      /*! \return profile list
          */
      virtual TStringList GetProfileList() = 0;

      //!         remove profile by name
      /*! \param  sName - profile name
          */
      virtual void RemoveProfile(const std::string& sName) = 0;

      //!         get profile by name
      /*! \param  sName - profile name
          \return profile
          */
      virtual SProfile GetProfile(const std::string& sName) = 0;

      //!         set(create) profile
      /*! \param  rProfile - profile
          */
      virtual void SetProfile(const SProfile& rProfile) = 0;
    };
  }
}

#endif // _ProfileAdmin_h_
