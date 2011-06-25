/*
 *  Copyright 2010 Utkin Dmitry
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


#ifndef _AXIS2CALLBACK_H_
#define _AXIS2CALLBACK_H_

#include "staffclientexport.h"

typedef struct axis2_callback axis2_callback_t;
typedef struct axutil_env axutil_env_t;

namespace staff
{
  //! Axis2/C callback
  class STAFF_CLIENT_EXPORT Axis2Callback
  {
  public:
    //! constructor
    Axis2Callback();

    //! destructor
    virtual ~Axis2Callback();

    //! is operation completed
    /*! \return true - operation completed
      */
    bool IsCompleted() const;

    //! get Axis2/C callback handle
    /*! \return Axis2/C callback handle
      */
    axis2_callback_t* Get();

    //! set Axis2/C callback handle
    /*! \param  pCallback - Axis2/C callback handle
        \param  pEnv - Axis2/C env
      */
    virtual void Set(axis2_callback_t* pCallback, axutil_env_t* pEnv);

  private:
    axis2_callback_t* m_pCallback; //!< Axis2/C callback handle
    axutil_env_t* m_pEnv; //!< Axis2/C env
  };

#ifndef STAFF_NO_DEPRECATED
  typedef Axis2Callback CAxis2Callback STAFF_DEPRECATED(Axis2Callback);
#endif
}

#endif // _AXIS2CALLBACK_H_
