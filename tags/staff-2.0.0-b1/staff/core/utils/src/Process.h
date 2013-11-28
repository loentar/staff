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


#ifndef _STAFF_UTILS_PROCESS_H_
#define _STAFF_UTILS_PROCESS_H_

#include <string>
#include "staffutilsexport.h"

namespace staff
{

  //! Process class
  class STAFF_UTILS_EXPORT Process
  {
  public:
    //! constructor
    Process();

    //! destructor
    ~Process();

    //! start process
    /*! Example: process.Start("/usr/bin/ls /tmp", true);
        \param  sCommandLine - command line delimited by whitespaces
        \param  bReadOutput - install handler to read standatd output stream
      */
    void Start(const std::string& sCommandLine, bool bReadOutput = false);

    //! is process running
    /*! \return true if process is running
      */
    bool IsRunning();

    //! terminate the process
    void Terminate();

    //! get current process id
    /*! \return current process id
      */
    static unsigned long GetCurrentId();

    //! get process id
    /*! \return process id
      */
    unsigned long GetId();

    //! read all standard output
    /*! this function will block until process is finished
        \return readed standard output
      */
    std::string ReadAllStdandardOutput();

    //! wait for process ends
    /*! \return exit code
      */
    int Wait();

    //! get current exec path
    /*! \return filename of executable with path
      */
    static std::string GetCurrentExecPath();

  private:
    struct ProcessImpl;
    ProcessImpl* m_pImpl;
  };

}

#endif // _STAFF_UTILS_PROCESS_H_
