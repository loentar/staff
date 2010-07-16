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
      //! destructor
      virtual ~Customers() {}

      //! add customer
      /*! \param sFirstName - first name
          \param sLastName - last name
          \param nYear - year of birth
          \return added customer id
        */
      // really there must be PUT
      // *restMethod: GET
      // *restLocation: Add?sFirstName={sFirstName}&sLastName={sLastName}&nYear={nYear}
      virtual int Add(const std::string& sFirstName, const std::string& sLastName, int nYear) = 0;

      //! delete customer
      /*! \param nId - customer id to delete
        */
      // really there must be DELETE
      // *restMethod: GET
      // *restLocation: Delete?nId={nId}
      virtual void Delete(int nId) = 0;

      //! update customer
      /*! \param nId - existing customer id
          \param sFirstName - first name
          \param sLastName - last name
          \param nYear - year of birth
        */
      // really there must be POST
      // *restMethod: GET
      // *restLocation: Update?nId={nId}&sFirstName={sFirstName}&sLastName={sLastName}&nYear={nYear}
      virtual void Update(int nId, const std::string& sFirstName, const std::string& sLastName, int nYear) = 0;

      //! get all customers
      /*! \return all customers
        */
      // *responseElement: Customers
      // *restMethod: GET
      // *restLocation: List
      virtual CustomersList List() = 0;

      //! get customer with given id
      /*! \return customer with given id
        */
      // *responseElement: Customer
      // *restMethod: GET
      // *restLocation: Get?nId={nId}
      virtual Customer Get(int nId) = 0;

    };
  }
}

#endif

