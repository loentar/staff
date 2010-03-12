#ifndef _SESSIONS_H_
#define _SESSIONS_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SSession
    {
      int nId;
//      int nExtraId;
      std::string sSessionId;
      int nUserId;
      int nExpires; //!< unix datetime
    };

    typedef std::list<SSession> TSessionsList;

    class CSessions
    {
    public:
      static const std::string sNobodySessionId;

    public:
      static CSessions& Inst();

      static void FreeInst();

/*      void GetById(int nId, SSession& rstSession);

      void GetBySessionId(const std::string& sSessionId, SSession& rstSession);*/

//      void GetList(TSessionsList& rlsSessions);

      void Open(const std::string& sUserName, const std::string& sPassword, bool bCloseExisting, std::string& sSessionId);

      void OpenExtra(const std::string& sSessionId, int nExtraId, std::string& sExtraSessionId);

      //! close session
      void Close(const std::string& sSessionId);

  //    void Close(int nUserId);

      void CloseExtra(const std::string& sSessionId, int nExtraId);

      bool Validate(const std::string& sSessionId);

      void Keepalive(const std::string& sSessionId);

      int GetExpiration() const;

      bool GetUserId(const std::string& sSessionId, int& nUserId);

      bool GetUserName(const std::string& sSessionId, std::string& sUserName);

      void Cleanup();

    private:
      CSessions();
      ~CSessions();

    private:
      static CSessions* m_pInst;
      int m_nSessionExpiration;
    };
  }
}

#endif // _SESSIONS_H_
