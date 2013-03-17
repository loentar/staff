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

#ifndef _Customer_H_
#define _Customer_H_

#include <string>
#include <list>
#include <staff/common/IService.h>

namespace samples
{
  namespace rest
  {
    //! customer
    struct Customer
    {
      int nId;                 //!< customer id
      std::string sFirstName;  //!< first name
      std::string sLastName;   //!< last name
      int nYear;               //!< year of birth
    };

    typedef std::list<Customer> CustomersList; //!< customers list

    //! REST demo: Customers service
    class Customers: public staff::IService
    {
    public:
      //! add customer
      /*! \param sFirstName - first name
          \param sLastName - last name
          \param nYear - year of birth
          \return added customer id
        */
      // *restEnable: true
      // *restMethod: POST
      // *restLocation: Add
      virtual int Add(const std::string& sFirstName, const std::string& sLastName, int nYear) = 0;

      //! delete customer
      /*! \param nId - customer id to delete
          THIS OPERATION MAY NOT WORK DUE TO AXIS2/C PROBLEM:
          https://issues.apache.org/jira/browse/AXIS2C-1418
          To fix the problem install the patch from http://code.google.com/p/staff/downloads/detail?name=http_transport_utils.c.REST_DELETE.patch
        */
      // *restEnable: true
      // *restMethod: DELETE
      // *restLocation: Delete/id={nId}
      virtual void Delete(int nId) = 0;

      //! update customer
      /*! \param nId - existing customer id
          \param sFirstName - first name
          \param sLastName - last name
          \param nYear - year of birth
        */
      // *restEnable: true
      // *restMethod: PUT
      // *restLocation: Update/id={nId}&firstname={sFirstName}&lastname={sLastName}&year={nYear}
      virtual void Update(int nId, const std::string& sFirstName, const std::string& sLastName, int nYear) = 0;

      //! get all customers
      /*! \return all customers
        */
      // *restEnable: true
      // *responseElement: Customers
      // *restMethod: GET
      // *restLocation: List
      virtual CustomersList List() = 0;

      //! get customer with given id
      /*! \return customer with given id
        */
      // *restEnable: true
      // *responseElement: Customer
      // *restMethod: GET
      // *restLocation: Get/id={nId}
      virtual Customer Get(int nId) = 0;

    };
  }
}

#endif

