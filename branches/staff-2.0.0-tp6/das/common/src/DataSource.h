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

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <string>
#include <list>
#include <map>
#include <rise/xml/XMLNode.h>
#include "staffdascommonexport.h"

namespace staff
{
namespace das
{
  typedef std::list<std::string> StringList; //!< string list
  typedef std::map<std::string, std::string> StringMap; //!< string map

  struct STAFF_DAS_COMMON_EXPORT DataType;
  typedef std::list<DataType> DataTypesList; //!< data types list

  //! data type
  struct STAFF_DAS_COMMON_EXPORT DataType
  {
    //! data type class
    enum Type
    {
      Void,
      Generic,
      Struct,
      List,
      DataObject
    };

    Type eType;              //!< data type: void, generic, struct, list or dataobject
    std::string sName;       //!< name
    std::string sType;       //!< type name: int, string, struct, list, etc.
    std::string sDescr;      //!< description
    DataTypesList lsChilds;  //!< struct fields
    bool bExtern;            //!< defined in external file

    DataType();
  };

  //! include descriptor
  struct STAFF_DAS_COMMON_EXPORT Include
  {
    std::string sFileName;  //!< filename
    DataTypesList lsTypes;  //!< types list
  };

  typedef std::list<Include> IncludesList;  //!< includes list

  //! operation
  struct STAFF_DAS_COMMON_EXPORT Operation
  {
    std::string sName;            //!< operation name
    std::string sDescr;           //!< operation description
    DataTypesList lsParams;       //!< operation params
    DataType stReturn;            //!< return type
    StringMap mOptions;           //!< operation options
    rise::xml::CXMLNode tScript;  //!< operation script
  };

  typedef std::list<Operation> OperationsList; //! list of operations


  //! information about provider
  struct STAFF_DAS_COMMON_EXPORT ProviderInfo
  {
    std::string sName;            //!< provider name
    std::string sId;              //!< provider id
    rise::xml::CXMLNode tConfig;  //!< provider config
  };

  typedef std::list<ProviderInfo> ProvidersInfoList; //! list of informations about providers


  //! data source
  class STAFF_DAS_COMMON_EXPORT DataSource
  {
  public:
    //! costructor
    DataSource();

    //! destructor
    ~DataSource();

    //! load datasource
    /*! \param rDataSourceNode - datasource node
        \param sFileName - datasource's filename
        */
    void Load(rise::xml::CXMLNode& rDataSourceNode, const std::string& sFileName);

    //! get included data types
    /*! \return included data types
      */
    const IncludesList& GetIncludes() const;


    //! get datasource name
    /*! \return datasource name
      */
    const std::string& GetName() const;

    //! get datasource description
    /*! \return datasource description
      */
    const std::string& GetDescr() const;

    //! get datasource namespace
    /*! \return datasource namespace
      */
    const std::string& GetNamespace() const;

    //! get default provider id
    /*! \return default provider id
      */
    const std::string& GetDefaultProvideId() const;

    //! get datasource providers info list
    /*! \return datasource providers info list
      */
    const ProvidersInfoList& GetProviders() const;

    //! get datasource options
    /*! \return datasource options
      */
    const StringMap& GetOptions() const;

    //! get type description
    /*! \param sTypeName - type name
        \return type
      */
    const DataType& GetType(const std::string& sTypeName) const;

    //! find type description
    /*! \param sTypeName - type name
        \return pointer to type, NULL if no type found
      */
    const DataType* FindType(const std::string& sTypeName) const;

    //! get operation description
    /*! \return operation description
      */
    const Operation& GetOperation(const std::string& sOperationName) const;

    //! get types list
    /*! \return types list
      */
    const DataTypesList& GetTypes() const;

    //! get operations list
    /*! \return operations list
      */
    const OperationsList& GetOperations() const;

  private:
    //! parse datatype
    /*! \param rNode - node to parse
        \param rType - parsed type
      */
    void ParseType(const rise::xml::CXMLNode& rNode, DataType& rType);

    //! parse description
    /*! \param rNode - node to parse
        \param sDescr - parsed description
      */
    void ParseDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr);

    //! include types
    /*! \param sFileName - types file name
      */
    void IncludeTypes(const std::string& sFileName);

    //! process include directive
    /*! \param sFileName - file name
      */
    void ProcessIncludes(rise::xml::CXMLNode& rNode);

  private:
    std::string m_sName;                        //!< datasource name
    std::string m_sDescr;                       //!< datasource description
    std::string m_sNamespace;                   //!< namespace
    std::string m_sDefaultProviderId;           //!< default provider id
    ProvidersInfoList m_lsProviders;            //!< providers
    StringMap m_mOptions;                       //!< datasource options
    std::string m_sFileName;                    //!< datasource file name
    std::string m_sDataSourcesDir;              //!< datasource path extracted from file name
    DataTypesList m_lsTypes;                    //!< defined types
    OperationsList m_lsOperations;              //!< datasource's operations list
    IncludesList m_lsIncludes;                  //!< included types list
  };
}
}

#endif // DATASOURCE_H
