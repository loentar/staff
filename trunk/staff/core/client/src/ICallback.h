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

#ifndef _ICLIENTCALLBACK_H_
#define _ICLIENTCALLBACK_H_

#include "Axis2Callback.h"

namespace staff
{
  class CDataObject;

  //! asynchronous client callback
  template <typename ReturnType>
  class ICallback: public CAxis2Callback
  {
  public:
    //! virtual destructor
    virtual ~ICallback() {}

    //! on complete handler
    virtual void OnComplete(ReturnType tResult) = 0;

    //! on fault handler
    virtual void OnFault(const staff::CDataObject& rFault) = 0;
  };

  //! asynchronous client callback with void type
  template <>
  class ICallback<void>: public CAxis2Callback
  {
  public:
    //! virtual destructor
    virtual ~ICallback() {}

    //! on complete handler
    virtual void OnComplete(void) = 0;

    //! on fault handler
    virtual void OnFault(const staff::CDataObject& rFault) = 0;
  };
}

#endif // _ICLIENTCALLBACK_H_

