/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _DBCONN_H_
#define _DBCONN_H_

typedef struct sqlite3 sqlite3; //!< sqlite3 db handle

namespace staff
{
  namespace samples
  {
    //! database connection
    class DbConn
    {
    public:
      //! open db
      static void Open();

      //! close db
      static void Close();

      //! get db
      /*! \return pointer to db object
        */
      static sqlite3* GetDb();

    private:
      static sqlite3* m_pDb; //!< pointer to db object
      static int m_nCounter; //!< Open/Close counter
    };

  }
}

#endif // _DBCONN_H_
