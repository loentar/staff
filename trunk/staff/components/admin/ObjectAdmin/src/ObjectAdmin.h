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

#ifndef _OBJECTADMIN_H_
#define _OBJECTADMIN_H_

#include <list>
#include <string>
#include <staff/common/IService.h>

namespace staff
{
  namespace admin
  {
    //! object description
    struct SObject
    {
      int nId;                   //!<  object id
      std::string sName;         //!<  name
      std::string sDescription;  //!<  description
      int nParentId;             //!<  parent object id
    };

    typedef std::list<SObject> TObjectList; //!<  list of objects

    //!  object-admin service
    class CObjectAdmin: public staff::IService
    {
    public:
      virtual ~CObjectAdmin() {}

      //!         get object by id
      /*! \param  nId - object id
          \return object
          */
      virtual SObject GetById(int nId) = 0;

      //!         get object by name
      /*! \param  sPathName - full object name (for example: components.samples.calc.Calculator.Add)
          \return object
          */
      virtual SObject GetByPathName(const std::string& sPathName) = 0;

      //!         get childs list
      /*! \param  nId - object id
          \return objectid list
      */
      virtual TObjectList GetChilds(int nId) = 0;

      //!         add new object
      /*! SObject::nObjectId is ignored
          \param  pstObject - new object
          \return added object id
          */
      virtual int Add(const SObject& rstObject) = 0;

      //!         remove object by id
      /*! \param  nObjectId - object id
          */
      virtual void Remove(int nObjectId) = 0;

      //!         set object description
      /*! \param  nObjectId - object id
          \param  sName - new object description
          */
      virtual void SetDescription(int nObjectId, const std::string& sDescription) = 0;

    };
  }
}

#endif // _OBJECTADMIN_H_
