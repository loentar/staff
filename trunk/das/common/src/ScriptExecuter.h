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

namespace rise
{
  template <typename Type> class CMutablePtr;

namespace xml
{
  class CXMLNode;
}
}

namespace staff
{
  class DataObject;

namespace das
{
  struct DataType;
  class DataSource;
  class IProvider;
  typedef rise::CMutablePtr<IProvider> PProvider;
}
}


namespace staff
{
namespace das
{
  //! script executer
  class STAFF_DAS_COMMON_EXPORT ScriptExecuter
  {
  public:
    //! construct script executer
    /*! \param rDataSource - data source
        \param rpProvicer - provider from datasource
      */
    ScriptExecuter(const DataSource& rDataSource, PProvider& rpProvider);

    //! destructor
    ~ScriptExecuter();

    //! process operation
    /*! \param rdoOperation - operation request data object
        \return operation result
      */
    DataObject Process(const DataObject& rdoOperation);

    //! process script
    /*! \param rdoContext - current context
        \param rScript - script to execute
        \param rReturnType - return type
        \param rdoResult - result
      */
    void Process(const DataObject& rdoContext, const rise::xml::CXMLNode& rScript,
                 const DataType& rReturnType, DataObject& rdoResult);

    //! process script
    /*! \param rScript - script to execute
      */
    void Process(const rise::xml::CXMLNode& rScript);

  private:
    class ScriptExecuterImpl;
    ScriptExecuterImpl* m_pImpl;
  };


} // namespace das
} // namespace staff

#endif // SCRIPTEXECUTER_H
