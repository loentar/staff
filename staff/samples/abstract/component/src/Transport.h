/*
 *  Copyright 2011 Utkin Dmitry
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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <string>
#include <staff/common/Abstract.h>
#include <staff/common/IService.h>
#include <staff/common/WsdlTypes.h>

namespace samples
{
namespace abstract
{
  //! abstract struct
  // *abstract: true
  struct Vehicle
  {
    // virtual destructor should present to get rtti working
    virtual ~Vehicle() {}
  };

  //! inherited struct
  struct Car: public Vehicle
  {
    std::string sModel;
  };

  //! inherited struct
  struct Plane: public Vehicle
  {
    std::string sSerialNumber;
  };


  struct TransportInfo
  {
    staff::Abstract<Vehicle> tpVehicle;
  };


  // *baseType: string
  enum TransportType
  {
    TransportTypeCar,
    TransportTypePlane,
    TransportTypeSubmarine
  };


  //! service to demonstrate abstract types support
  class Transport: public staff::IService
  {
  public:
    virtual TransportInfo GetTransport(TransportType eTransport) = 0;

    //! test operation
    virtual staff::Abstract<Vehicle> Echo(const staff::Abstract<Vehicle>& rtpVehicle) = 0;
  };

}
}

#endif // _TRANSPORT_H_

