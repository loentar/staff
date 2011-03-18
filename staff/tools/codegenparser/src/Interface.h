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
  typedef std::list<std::string> TStringList; //!< string list
  typedef std::map<std::string, std::string> TStringMap; //!< string map

  //! data type
  struct STAFF_CODEGENPARSER_EXPORT SDataType
  {
    enum EDataType  //! data type enum
    {
      EUnknown,     //!<  unknown
      EGeneric,     //!<  generic(supported by staff)
      EString,      //!<  string type
      EDataObject,  //!<  DataObject
      EEnum,        //!<  enum
      EStruct,      //!<  struct
      ETypedef,     //!<  typedef
      ETemplate     //!<  template container (list, map, etc)
    };

    bool                    bIsConst;    //!<  const type
    bool                    bIsRef;      //!<  reference type
    EDataType               eType;       //!<  data type enum
    std::string             sNodeName;   //!<  SOAP node name
    std::string             sUsedName;   //!<  name as used
    std::string             sName;       //!<  type name (int, string, etc.)
    std::string             sNamespace;  //!<  namespace
    std::list<SDataType>    lsParams;    //!<  template parameters

    //! default constructor
    /*! non-const, not a ref, Generic datatype */
    SDataType();
  };

  //!  parameter
  struct STAFF_CODEGENPARSER_EXPORT SParam
  {
    SDataType    stDataType;  //!<  param data type
    std::string  sName;       //!<  param name
    std::string  sDescr;      //!<  param description
    std::string  sDetail;     //!<  detailed description
    TStringMap   mOptions;    //!<  param metacomments options
  };

  //!  service operation
  struct STAFF_CODEGENPARSER_EXPORT SMember
  {
    SParam             stReturn;       //!<  return type
    std::string        sName;          //!<  operation name
    std::list<SParam>  lsParams;       //!<  parameters
    bool               bIsConst;       //!<  operation is const
    std::string        sDescr;         //!<  operation description
    std::string        sDetail;        //!<  detailed description
    bool               bIsAsynch;      //!<  operation is asynchronous(client)
    TStringMap         mOptions;       //!<  member metacomments options

    SMember();
  };


  //! base type for data types
  struct STAFF_CODEGENPARSER_EXPORT SBaseType
  {
    //!< base type enum
    enum EType
    {
      EUnknown = 0, //!< unknown/unintialized
      EEnum    = 1, //!< type is enum
      EStruct  = 2, //!< type is struct
      ETypedef = 4, //!< type is typedef
      EAny = EEnum|EStruct|ETypedef //!< any type
    };

    EType               eType;          //!<  type
    std::string         sName;          //!<  name
    std::string         sNamespace;     //!<  namespace
    std::string         sOwnerName;     //!<  owner struct name in format StructName[::SubType]
    std::string         sDescr;         //!<  service description
    std::string         sDetail;        //!<  detailed description
    bool                bExtern;        //!<  imported from other interface
    bool                bForward;       //!<  is forward declaration
    TStringMap          mOptions;       //!<  metacomments options

    SBaseType();
  };

  //! enum
  struct STAFF_CODEGENPARSER_EXPORT SEnum: public SBaseType
  {
    //! enum member
    struct SEnumMember
    {
      std::string sName;   //!<  enum member name
      std::string sValue;  //!<  enum member value (optional)
    };

    std::list<SEnumMember>  lsMembers;     //!<  members

    SEnum();
  };

  //! struct
  struct STAFF_CODEGENPARSER_EXPORT SStruct: public SBaseType
  {
    std::string         sParentName;      //!<  parent struct name (with namespace as used)
    std::string         sParentNamespace; //!<  parent struct namespace (actual)
    std::list<SParam>   lsMembers;        //!<  struct fields
    std::list<SStruct>  lsStructs;        //!<  sub struct list
    std::list<SEnum>    lsEnums;          //!<  sub enum list

    SStruct();
  };

  //! typedef
  struct STAFF_CODEGENPARSER_EXPORT STypedef: public SBaseType
  {
    SDataType           stDataType;     //!<  defined data type

    STypedef();
  };

  //! service class
  struct STAFF_CODEGENPARSER_EXPORT SClass: public SBaseType
  {
    TStringList         lsModules;      //!<  axis2/c modules list to engage
    std::list<SMember>  lsMembers;      //!<  service operations
  };

  //! include info
  struct STAFF_CODEGENPARSER_EXPORT SInclude
  {
    std::string sInterfaceName;      //!<  interface name
    std::string sNamespace;          //!<  interface namespace
    std::string sFileName;           //!<  file name
    std::string sTargetNs;           //!<  target namespace
  };

  //! service interface
  struct STAFF_CODEGENPARSER_EXPORT SInterface
  {
    std::string           sName;          //!<  interface name, based on input filename
    std::string           sNamespace;     //!<  interface main namespace
    std::string           sFileName;      //!<  input filename
    std::string           sTargetNs;      //!<  target namespace
    std::list<SInclude>   lsIncludes;     //!<  included files
    std::list<SEnum>      lsEnums;        //!<  enum list
    std::list<STypedef>   lsTypedefs;     //!<  typedef list
    std::list<SStruct>    lsStructs;      //!<  struct list
    std::list<SClass>     lsClasses;      //!<  service classes list
  };

  //! project
  struct STAFF_CODEGENPARSER_EXPORT SProject
  {
    std::string            sName;         //!<  project name
    std::string            sNamespace;    //!<  component namespace
    std::list<SInterface>  lsInterfaces;  //!<  interface list
  };
}
}

#endif // _INTERFACE_H_
