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

#ifndef _DATAACCESSSERVICE_H_
#define _DATAACCESSSERVICE_H_

#include <staff/common/IService.h>
#include <string>
#include <list>

namespace staff
{
  class CDataObject;

  namespace das
  {
    typedef std::list<std::string> StringList; //!< string list

    //! Data service
    class DataAccessService: public staff::IService
    {
    public:
      virtual ~DataAccessService() {}

      //! get providers
      /*! \return provider list
        */
      virtual StringList GetProviders() = 0;

      //! get data sources
      /*! \return data sources list
        */
      virtual StringList GetDataSources() = 0;

      //! use data source
      /*! \param sDataSource - data source name
        */
      virtual void SetDataSource(const std::string& sDataSource) = 0;

      //! free data source
      virtual void FreeDataSource() = 0;

      //! get interdace definition
      /*! \return interface
        */
      virtual CDataObject GetInterface() const = 0;

      //! invoke the operation
      /*! \param  rdoOperation - operation name and params
          \return operation result
          */
      virtual CDataObject Invoke(const CDataObject& rdoOperation) = 0;

    };
  }
}

#endif // _DATAACCESSSERVICE_H_
