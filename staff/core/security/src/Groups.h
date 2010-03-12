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
