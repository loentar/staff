/*
 *  Copyright 2011 Utkin Dmitry
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

#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <string>
#include <list>
#include <map>
#include "staffcodegenparserexport.h"

namespace staff
{
namespace codegen
{
  typedef std::list<std::string> StringList; //!< string list
  typedef std::map<std::string, std::string> StringMap; //!< string map

  //! data type
  struct STAFF_CODEGENPARSER_EXPORT DataType
  {
    enum Type  //! data type enum
    {
      TypeUnknown,    //!<  unknown
      TypeGeneric,    //!<  generic(supported by staff)
      TypeString,     //!<  string type
      TypeDataObject, //!<  DataObject
      TypeEnum,       //!<  enum
      TypeStruct,     //!<  struct
      TypeTypedef,    //!<  typedef
      TypeTemplate    //!<  template container (list, map, etc)
    };

    bool                 bIsConst;    //!<  const type
    bool                 bIsRef;      //!<  reference type
    Type                 eType;       //!<  data type enum
    std::string          sUsedName;   //!<  name as used
    std::string          sPrefix;     //!<  prefix
    std::string          sName;       //!<  type name (int, string, etc.)
    std::string          sNamespace;  //!<  namespace
    std::string          sOwnerName;  //!<  owner struct name in format "StructName[::SubType]"
    std::list<DataType>  lsParams;    //!<  template parameters

    //! default constructor
    /*! non-const, not a ref, Generic datatype */
    DataType();
  };

  //!  parameter
  struct STAFF_CODEGENPARSER_EXPORT Param
  {
    DataType     stDataType;  //!<  param data type
    std::string  sName;       //!<  param name
    std::string  sDescr;      //!<  param description
    std::string  sDetail;     //!<  detailed description
    StringMap    mOptions;    //!<  param metacomments options
  };

  //!  service operation
  struct STAFF_CODEGENPARSER_EXPORT Member
  {
    Param             stReturn;   //!<  return type
    std::string       sName;      //!<  operation name
    std::list<Param>  lsParams;   //!<  parameters
    bool              bIsConst;   //!<  operation is const
    std::string       sDescr;     //!<  operation description
    std::string       sDetail;    //!<  detailed description
    bool              bIsAsynch;  //!<  operation is asynchronous(client)
    StringMap         mOptions;   //!<  member metacomments options

    Member();
  };


  //! base type for data types
  struct STAFF_CODEGENPARSER_EXPORT BaseType
  {
    //!< base type enum
    enum Type
    {
      TypeUnknown = 0, //!< unknown/unintialized
      TypeEnum    = 1, //!< type is enum
      TypeStruct  = 2, //!< type is struct
      TypeTypedef = 4, //!< type is typedef
      TypeAny = TypeEnum|TypeStruct|TypeTypedef //!< any type
    };

    Type          eType;        //!<  type
    std::string   sName;        //!<  name
    std::string   sNamespace;   //!<  namespace
    std::string   sOwnerName;   //!<  owner struct name in format "StructName[::SubType]"
    std::string   sDescr;       //!<  service description
    std::string   sDetail;      //!<  detailed description
    bool          bExtern;      //!<  imported from other interface
    bool          bForward;     //!<  is forward declaration
    StringMap     mOptions;     //!<  metacomments options

    BaseType();
  };

  //! enum
  struct STAFF_CODEGENPARSER_EXPORT Enum: public BaseType
  {
    //! enum member
    struct EnumMember
    {
      std::string sName;   //!<  enum member name
      std::string sValue;  //!<  enum member value (optional)
      std::string sDescr;  //!<  enum member description
    };

    std::list<EnumMember>  lsMembers;     //!<  members

    Enum();
  };

  //! struct
  struct STAFF_CODEGENPARSER_EXPORT Struct: public BaseType
  {
    std::string         sParentName;      //!<  parent struct name (with namespace as used)
    std::string         sParentNamespace; //!<  parent struct namespace (actual)
    std::list<Param>    lsMembers;        //!<  struct fields
    std::list<Struct>   lsStructs;        //!<  sub struct list
    std::list<Enum>     lsEnums;          //!<  sub enum list

    Struct();
  };

  //! typedef
  struct STAFF_CODEGENPARSER_EXPORT Typedef: public BaseType
  {
    DataType   stDataType;     //!<  defined data type

    Typedef();
  };

  //! service class
  struct STAFF_CODEGENPARSER_EXPORT Class: public BaseType
  {
    StringList         lsModules;  //!<  axis2/c modules list to engage
    std::list<Member>  lsMembers;  //!<  service operations
  };

  //! include info
  struct STAFF_CODEGENPARSER_EXPORT Include
  {
    std::string  sInterfaceName;    //!<  interface name
    std::string  sNamespace;        //!<  interface namespace
    std::string  sFileName;         //!<  file name
    std::string  sFilePath;         //!<  file path
    std::string  sTargetNs;         //!<  target namespace
  };

  //! service interface
  struct STAFF_CODEGENPARSER_EXPORT Interface
  {
    std::string          sName;          //!<  interface name, based on input filename
    std::string          sNamespace;     //!<  interface main namespace
    std::string          sFileName;      //!<  input file name
    std::string          sFilePath;      //!<  input file path
    StringMap            mOptions;       //!<  metacomments options
    std::list<Include>   lsIncludes;     //!<  included files
    std::list<Enum>      lsEnums;        //!<  enum list
    std::list<Typedef>   lsTypedefs;     //!<  typedef list
    std::list<Struct>    lsStructs;      //!<  struct list
    std::list<Class>     lsClasses;      //!<  service classes list
  };

  //! project
  struct STAFF_CODEGENPARSER_EXPORT Project
  {
    std::string           sName;         //!<  project name
    std::string           sNamespace;    //!<  component namespace
    std::list<Interface>  lsInterfaces;  //!<  interface list
  };
}
}

#endif // _INTERFACE_H_
