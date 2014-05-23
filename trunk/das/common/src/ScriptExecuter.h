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


#ifndef SCRIPTEXECUTER_H
#define SCRIPTEXECUTER_H

#include "staffdascommonexport.h"
#include <string>
#include <map>
#include <staff/common/DataObject.h>
#include "DataSource.h"

namespace staff
{
  template <typename Type> class SharedPtr;
  class Mutex;

namespace xml
{
  class Element;
}

namespace das
{
  class IProvider;
  typedef SharedPtr<IProvider> PProvider;
}
}


namespace staff
{
namespace das
{

  //! Script executer variable
  struct Var
  {
    DataType tType;               //!< type of variable
    std::string sValue;           //!< generic value
    staff::DataObject tdoValue;   //!< dataobject value
  };

  //! variables map
  typedef std::map<std::string, Var> VarMap;

  //! script executer
  class STAFF_DAS_COMMON_EXPORT ScriptExecuter
  {
  public:
    //! construct script executer
    /*! \param rDataSource - data source
        \param rstProviders - providers for datasource
      */
    ScriptExecuter(const DataSource& rDataSource, Providers& rstProviders);

    //! destructor
    ~ScriptExecuter();

    //! set session storage
    /*! \param rmSessionStorage - session storage
        \param rMutex - mutex for session storage
     */
    void SetSessionStorage(VarMap& rmSessionStorage, Mutex& rMutex);

    //! process operation
    /*! \param rdoOperation - operation request data object
        \param rdoResult - operation result
      */
    void Process(const DataObject& rdoOperation, DataObject& rdoResult);

    //! process script
    /*! \param rdoContext - current context
        \param rScript - script to execute
        \param rReturnType - return type
        \param rdoResult - result
      */
    void Process(const DataObject& rdoContext, const xml::Element& rScript,
                 const DataType& rReturnType, DataObject& rdoResult);

    //! process script
    /*! \param rScript - script to execute
      */
    void Process(const xml::Element& rScript);

  private:
    class ScriptExecuterImpl;
    ScriptExecuterImpl* m_pImpl;
  };


} // namespace das
} // namespace staff

#endif // SCRIPTEXECUTER_H
