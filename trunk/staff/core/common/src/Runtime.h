/*
 *  Copyright 2009 Utkin Dmitry
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

#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "staffcommonexport.h"
#include <string>

// Axis2/C typedefs
typedef struct axutil_env axutil_env_t;

namespace staff
{
  //!  Runtime information
  class STAFF_COMMON_EXPORT Runtime
  {
  public:
    //! get instance
    /*! \return ref to instance
    */
    static Runtime& Inst();

    //! get Axis2/C env for staff component
    /*! \return Axis2/C env
    */
    axutil_env_t* GetAxis2Env();

    //! get Axis2/C env for given component
    /*! \param  szLogName - log name(C-string), for example: client.log
        \return Axis2/C env
    */
    axutil_env_t* CreateAxis2Env(const char* szLogName);

    //! get Axis2/C env for given component
    /*! \param  sLogName - log name, for example: client.log
        \return Axis2/C env
    */
    axutil_env_t* CreateAxis2Env(const std::string& sLogName);

    //! free Axis2/C env
    /*! \param  pEnv - Axis2/C environment created by CreateAxis2Env
    */
    void FreeAxis2Env(axutil_env_t* pEnv);

    //! get Axis2/C home dir
    /*! \return Axis2/C home dir
    */
    const std::string& GetAxis2Home() const;

    //! get staff home dir
    /*! \return staff home dir
    */
    const std::string& GetStaffHome() const;

    //! get home dir for staff components
    /*! \return home dir for staff components
    */
    std::string GetComponentsHome() const;

    //! get home dir for given component
    /*! \param  sComponent - component
        \return home dir for given component
        */
    std::string GetComponentHome(const std::string& sComponent) const;

    //! get environment variable value
    /*! throws exception, when variable is not found
        \param  sVariable - environment variable
        \return environment variable value
        */
    std::string GetEnv(const std::string& sVariable) const;

    //! get environment variable value
    /*! \param  sVariable - environment variable
        \return environment variable value, or empty, if no variable found
        */
    std::string GetEnvOpt(const std::string& sVariable) const;

  private:
    Runtime();
    ~Runtime();
    Runtime(const Runtime&);
    Runtime& operator=(const Runtime&);

  private:
    class RuntimeImpl;
    RuntimeImpl* m_pImpl;
  };

}

#endif // _RUNTIME_H_
