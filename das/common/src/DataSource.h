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

namespace staff
{
namespace das
{
  typedef std::list<std::string> StringList; //!< string list

  struct Type;
  typedef std::list<Type> TypesList; //!< data types list

  //! data type
  struct Type
  {
    //! data type
    enum EType
    {
      Void,
      Generic,
      Struct,
      List,
      DataObject
    };

    EType eType;        //!< data type: void, generic, struct, list or dataobject
    std::string sName;  //!< name
    std::string sType;  //!< type name: int, string, struct, list, etc.
    std::string sDescr; //!< description
    TypesList lsChilds; //!< struct fields
    bool bExtern;       //!< defined in external file

    Type();
  };

  typedef std::list<Type> TypesList;  //!< types list

  //! include descriptor
  struct Include
  {
    std::string sFileName;  //!< filename
    TypesList lsTypes;      //!< types list
  };

  typedef std::list<Include> IncludesList;  //!< includes list

  //! operation
  struct Operation
  {
    std::string sName;            //!< operation name
    std::string sDescr;           //!< operation description
    TypesList lsParams;           //!< operation params
    Type stReturn;                //!< return type
    rise::xml::CXMLNode tScript;  //!< operation script
  };

  typedef std::list<Operation> OperationsList; //! list of operations

  //! data source
  class DataSource
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
    void Load(const rise::xml::CXMLNode& rDataSourceNode, const std::string& sFileName);

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

    //! get datasource provider name
    /*! \return datasource provider name
      */
    const std::string& GetProviderName() const;


    //! get datasource provider config
    /*! \return datasource provider config
      */
    const rise::xml::CXMLNode& GetProviderConfig() const;

    //! get type description
    /*! \param sTypeName - type name
        \return type
      */
    const Type& GetType(const std::string& sTypeName) const;

    //! find type description
    /*! \param sTypeName - type name
        \return pointer to type, NULL if no type found
      */
    const Type* FindType(const std::string& sTypeName) const;

    //! get operation description
    /*! \return operation description
      */
    const Operation& GetOperation(const std::string& sOperationName) const;


    //! get types list
    /*! \return types list
      */
    const TypesList& GetTypes() const;

    //! get operations map
    /*! \return operations map
      */
    const OperationsList& GetOperations() const;

  private:
    //! parse datatype
    /*! \param rNode - node to parse
        \param rType - parsed type
      */
    void ParseType(const rise::xml::CXMLNode& rNode, Type& rType);

    //! parse description
    /*! \param rNode - node to parse
        \param sDescr - parsed description
      */
    void ParseDescr(const rise::xml::CXMLNode& rNode, std::string& sDescr);

    //! include types
    /*! \param sFileName - types file name
      */
    void IncludeTypes(const std::string& sFileName);

  private:
    std::string m_sName;                        //!< datasource name
    std::string m_sDescr;                       //!< datasource description
    std::string m_sNamespace;                   //!< namespace
    std::string m_sProviderName;                //!< provider name
    std::string m_sFileName;                    //!< datasource file name
    rise::xml::CXMLNode m_tNodeProviderConfig;  //!< provider config
    TypesList m_lsTypes;                        //!< defined types
    OperationsList m_lsOperations;                //!< datasource's operations map
    IncludesList m_lsIncludes;                  //!< included types map
  };
}
}

#endif // DATASOURCE_H
