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

#ifndef _PERFORMANCECOUNTER_H_
#define _PERFORMANCECOUNTER_H_

#include <rise/os/osdllexport.h>

namespace rise
{
  //!        performance counter
  class RISE_EXPORT CPerformanceCounter
  {
  public:
    //!        constructor
    CPerformanceCounter();

    //!        destructor
    ~CPerformanceCounter();

    //!        start counting time
    void Start();

    //!        finish counting time
    /*! \return difference between finish and start in microseconds
    */
    long long Finish();

    //!        get last measured value
    /*! \return last measured value in microseconds
    */
    long long GetLastMeasuredValue() const;

  private:
    long long m_llStartValue;         //!< start value
    long long m_llLastMeasuredValue;  //!< last measured value
  };

}; // namespace rise

#endif // _PERFORMANCECOUNTER_H_
