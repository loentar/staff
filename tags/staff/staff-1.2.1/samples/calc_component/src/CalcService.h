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

#ifndef _CalcService_H_
#define _CalcService_H_

namespace samples
{
  namespace calc
  {
    // description: sample component service CalcService
    // uri: http://localhost:9090/axis2/services/samples.calc.CalcService
    class CCalcService
    {
    public:
      virtual ~CCalcService() {}

      /*! add nA and nB.
          example of using non-default result node
      */
      // responseElement: AddResponse
      // resultElement: addResult
      virtual int Add(int nA, int nB) const = 0;

      /*! substract nA and nB.
          example of invoking another component service
      */
      virtual int Sub(int nA, int nB) const = 0;

      /*! store integer value.
          example of using service configuration
      */
      virtual void SetMem(int nMem) = 0;

      /*! retrieve integer value.
          example of using service configuration
      */
      virtual int GetMem() const = 0;
    };
  }
}

#endif
