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


#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <string>
#include "staffdascommonexport.h"

namespace staff
{
  template <typename Type> class SharedPtr;
  class DataObject;

  namespace das
  {
    typedef std::list<std::string> StringList; //!< string list
    struct STAFF_DAS_COMMON_EXPORT DataType;

#define STAFF_DAS_NULL_VALUE "(_STAFF_DAS_NULL_VALUE_)"

    //! Executor interface
    class IExecutor
    {
    public:
      //! provider type
      enum Type
      {
        TypeUnknown,  //!< unknown type
        TypeRaw,      //!< raw executor
        TypeQuery     //!< query executor
      };

    public:
      //! destructor
      virtual ~IExecutor() {}

      //! get executor type
      /*! \return executor type
        */
      virtual Type GetType() const = 0;
    };

    //! smart pointer to executor object
    typedef SharedPtr<IExecutor> PExecutor;



    //! Query executor interface
    class IQueryExecutor: public IExecutor
    {
    public:
      //! get executor type
      /*! \return executor type
        */
      virtual Type GetType() const
      {
        return TypeQuery;
      }

      //! reset executor and free result
      virtual void Reset() = 0;

      //! execute query
      /*! \param  sExecute - query to execute, may contain sql params placeholders
          \param  rlsParams - query parameters
        */
      virtual void Execute(const std::string& sExecute,
                           const StringList& rlsParams = StringList()) = 0;

      //! get fields names
      /*! \sa Execute
          \param  rNames - resulting field names
        */
      virtual void GetFieldsNames(StringList& rNames) = 0;

      //! get next row
      /*! \sa Execute
          \param  rResult - resulting row
          \return true if result was get
        */
      virtual bool GetNextResult(StringList& rResult) = 0;
    };

    //! smart pointer to Query executor object
    typedef SharedPtr<IQueryExecutor> PQueryExecutor;



    //! Raw executor interface
    class IRawExecutor: public IExecutor
    {
    public:
      //! get executor type
      /*! \return executor type
        */
      virtual Type GetType() const
      {
        return TypeRaw;
      }

      //! execute query
      /*! \param  sSessionId - session id
          \param  sExecute - query to execute
          \param  rdoContext - context
          \param  rReturnType - return type
          \param  rdoResult - result
        */
      virtual void Execute(const std::string& sSessionId,
                           const std::string& sExecute, const DataObject& rdoContext,
                           const DataType& rReturnType, DataObject& rdoResult) = 0;
    };

    //! smart pointer to RawExecutor object
    typedef SharedPtr<IRawExecutor> PRawExecutor;

  }
}

#endif // EXECUTOR_H
