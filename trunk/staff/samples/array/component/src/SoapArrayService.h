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

#ifndef _SOAPARRAYSERVICE_H_
#define _SOAPARRAYSERVICE_H_

#include <string>
#include <staff/common/Array.h>
#include <staff/common/IService.h>

namespace samples
{
namespace soaparray
{

  typedef staff::Array< staff::Array< staff::Array<int> > > ArrayOfArrayOfIntArray;


  struct Data
  {
    staff::Array<int> aIntArray;
    staff::Array<std::string> aStringArray;
  };

  typedef staff::Array<Data> DataArray;

  typedef staff::Array< staff::Array<int> > ArrayOfIntArray;
  typedef staff::Array< staff::Array<Data> > ArrayOfDataArray;


  //! service to demonstrate SOAP Array support
  class SoapArrayService: public staff::IService
  {
  public:
    virtual void SetIntArray(const staff::Array<int>& rArray) = 0;
    virtual staff::Array<int> GetIntArray() const = 0;
    virtual staff::Array<int> GetSparseIntArray() const = 0;

    virtual void SetData(const Data& rData) = 0;
    virtual Data GetData() const = 0;
    virtual Data GetSparseData() const = 0;

    virtual void SetDataArray(const DataArray& rDataArray) = 0;
    virtual DataArray GetDataArray() const = 0;

    virtual void SetArrayOfIntArray(const ArrayOfIntArray& rArrayOfIntArray) = 0;
    virtual ArrayOfIntArray GetArrayOfIntArray() const = 0;
    virtual ArrayOfIntArray GetSparseArrayOfIntArray() const = 0;

    virtual void SetArrayOfDataArray(const ArrayOfDataArray& rArrayOfDataArray) = 0;
    virtual ArrayOfDataArray GetArrayOfDataArray() const = 0;

    virtual void SetArrayOfArrayOfIntArray(const ArrayOfArrayOfIntArray& rArrayOfArrayOfIntArray) = 0;
    virtual ArrayOfArrayOfIntArray GetArrayOfArrayOfIntArray() const = 0;
    virtual ArrayOfArrayOfIntArray GetSparseArrayOfArrayOfIntArray() const = 0;

    virtual staff::Array<int> Get2dIntArray() const = 0;
    virtual staff::Array<int> Get2dSparseIntArray() const = 0;

    virtual staff::Array< staff::Array<int> > GetComplex2dIntArray() const = 0;
    virtual staff::Array< staff::Array<int> > GetComplex2dSparseIntArray() const = 0;
  };

}
}

#endif // _SOAPARRAYSERVICE_H_

