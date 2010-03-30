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

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SObject
    {
      int nId;
      std::string sName;
      std::string sDescription;
      int nParentId;
    };

    typedef std::list<SObject> TObjectList;

    class CObjects
    {
    public:
      static CObjects& Inst();

      static void FreeInst();

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

      void Add(const std::string& sName, const std::string& sDescription, int nParentId, int& nId);

      void Remove(int nId);

      void SetDescription(int nId, const std::string& sDescription);

      void GetChilds(int nId, TObjectList& rlsChilds);

      bool GetChildId(int nId, const std::string& sChildName, int& nChildId);

      void GetParent(int nId, SObject& rstParent);

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
