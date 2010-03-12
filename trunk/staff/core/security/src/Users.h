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
