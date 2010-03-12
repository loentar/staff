#ifndef _DBCONN_H_
#define _DBCONN_H_

#include <string>

typedef struct sqlite3 sqlite3; //!< sqlite3 db handle

namespace staff
{
  namespace security
  {
    class CDbConn
    {
    public:
      static void Open();
      static void Close();
      static sqlite3* GetDb();

    private:
      static sqlite3* m_pDb;
    };
  }
}

#endif // _DBCONN_H_
