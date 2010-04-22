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

#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include "staffsecurityexport.h"
#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    //! security object
    struct STAFF_SECURITY_EXPORT SObject
    {
      int nId;                  //!< object id
      std::string sName;        //!< object name
      std::string sDescription; //!< object description
      int nParentId;            //!< parent object id
    };

    typedef std::list<SObject> TObjectList; //!< list of objects

    //! security objects
    class STAFF_SECURITY_EXPORT CObjects
    {
    public:
      //! get objects instance
      /*! \return objects instance
        */
      static CObjects& Inst();

      //! free objects instance
      static void FreeInst();

      //! get object by id
      /*! \param  nId - object id
          \param  rstObject - resulting object
          \sa GetIdByPathName
        */
      void GetById(int nId, SObject& rstObject);

      //! get object by path name
      /*! \param  sName - object name path (for example: "components.samples.calc.Calculator")
          \param  rstObject - resulting object
          \sa GetIdByPathName
        */
      void GetByPathName(const std::string& sName, SObject& rstObject);

      //! get object id by path name
      /*! \param  sName - object name path (for example: "components.samples.calc.Calculator")
          \param  nId - resulting object id
          \sa GetByPathName
        */
      bool GetIdByPathName(const std::string& sName, int& nId);


      //! create new object
      /*! \param sName - object name
          \param sDescription - object description
          \param nParentId - parent object id
          \param nId - id of created object
          */
      void Add(const std::string& sName, const std::string& sDescription, int nParentId, int& nId);

      //! remove object by id
      /*! \param nId - object id
        */
      void Remove(int nId);

      //! set object description
      /*! \param nId - object id
          \param sDescription - new description
          */
      void SetDescription(int nId, const std::string& sDescription);

      //! get object's childs
      /*! \param nId - object id
          \param rlsChlids - list of object childrens
          */
      void GetChilds(int nId, TObjectList& rlsChilds);

      //! get object's child id
      /*! \param nId - object id
          \param sChildName - child's name
          \param nChildId - resulting child id
          */
      bool GetChildId(int nId, const std::string& sChildName, int& nChildId);

      //! get parent object
      /*! \param nId - object id
          \param rstParent - parent object
          */
      void GetParent(int nId, SObject& rstParent);

      //! get parent object id
      /*! \param nId - object id
          \param nParent - parent object id
          */
      void GetParentId(int nId, int& nParentId);

    private:
      CObjects();
      ~CObjects();

    private:
      static CObjects* m_pInst;
    };
  }
}

#endif // _OBJECTS_H_
