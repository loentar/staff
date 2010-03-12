#ifndef _ACL_H_
#define _ACL_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    enum EAccess
    {
      EAccessDenied,
      EAccessGranted,
      EAccessInherited
    };

    class CAcl
    {
    public:
      static CAcl& Inst();

      static void FreeInst();
      
      void SetUserAccess(int nObjectId, EAccess eAccess);
      void SetUserAccess(int nObjectId, int nUserId, EAccess eAccess);
      void GetUserAccess(int nObjectId, int nUserId, EAccess& reAccess);

      void SetGroupAccess(int nObjectId, EAccess eAccess);
      void SetGroupAccess(int nObjectId, int nGroupId, EAccess eAccess);
      void GetGroupAccess(int nObjectId, int nGroupId, EAccess& reAccess);

      bool CalculateUserAccess(int nObjectId, int nUserId);
      bool CalculateUserAccess(const std::string& sObjectPath, int nUserId);

    private:
      CAcl();
      ~CAcl();

    private:
      static CAcl* m_pInst;
    };
  }
}

#endif // _ACL_H_
