/*
 *  Copyright 2009 Utkin Dmitry
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

#ifndef _INTERFACEINFO_H_
#define _INTERFACEINFO_H_

#include <iosfwd>
#include <string>
#include <list>

//! data type
struct SDataType 
{
  enum EDataType  //! data type enum
  {
    EUnknown,     //!<  unknown
    EGeneric,     //!<  generic(supported by staff)
    EString,      //!<  string type
    EDataObject,  //!<  DataObject
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
  /* non-const, not a ref, Generic datatype */
  SDataType();

  //! copy operator
  SDataType& operator=(const SDataType& stDataType);
};

//!  parameter
struct SParam
{
  SDataType    stDataType;  //!<  param data type
  std::string  sName;       //!<  param name

  SParam& operator=(const SParam& rParam);
};

//!  service operation
struct SMember
{
  SParam             stReturn;    //!<  return type
  std::string        sName;       //!<  operation name
  std::list<SParam>  lsParamList; //!<  parameters
  bool               bIsConst;    //!<  operation is const
  std::string        sDescr;      //!<  operation description
  std::string        sSoapAction; //!<  soap action
  SMember();
};

//! service class
struct SClass
{
  std::string         sName;          //!<  class name
  std::string         sNamespace;     //!<  namespace
  std::string         sDescr;         //!<  service description
  std::string         sServiceUri;    //!<  default service URI
  std::list<SMember>  lsMember;       //!<  service operations
};

//! struct
struct SStruct
{
  std::string         sName;          //!<  struct name
  std::string         sNamespace;     //!<  namespace
  std::string         sParent;        //!<  parent struct(inherits)
  std::list<SParam>   lsMember;       //!<  struct fields
  bool                bForward;       //!<  is forward declaration

  SStruct();
};

//! typedef
struct STypedef
{
  std::string         sName;          //!<  typedef name
  std::string         sNamespace;     //!<  namespace
  SDataType           stDataType;     //!<  base data type
};

//! service interface
struct SInterface
{
  std::string           sName;          //!<  interface name, based on input filename
  std::string           sFileName;      //!<  input filename
  std::string           sTargetNs;      //!<  target namespace
  std::list<STypedef>   lsTypedef;      //!<  typedef list
  std::list<SStruct>    lsStruct;       //!<  struct list
  std::list<SClass>     lsClass;        //!<  service classes list
};

//! project
struct SProject
{
  std::string            sName;         //!<  project name
  std::string            sInDir;        //!<  input dir
  std::string            sOutDir;       //!<  output dir
  std::list<SInterface>  lsInterfaces;  //!<  interface list
};

std::istream& operator>>(std::istream& rStream, SDataType& rDataType);
std::istream& operator>>(std::istream& rStream, SParam& rParameter);
std::istream& operator>>(std::istream& rStream, SMember& rMember);
std::istream& operator>>(std::istream& rStream, SClass& rClass);
std::istream& operator>>(std::istream& rStream, SStruct& rStruct);
std::istream& operator>>(std::istream& rStream, STypedef& rTypedef);
std::istream& operator>>(std::istream& rStream, SInterface& rInterface);

#ifdef DEBUG
std::ostream& operator<<(std::ostream& rStream, const SDataType& rDataType);
std::ostream& operator<<(std::ostream& rStream, const SParam& rParameter);
std::ostream& operator<<(std::ostream& rStream, const SMember& rMember);
std::ostream& operator<<(std::ostream& rStream, const SClass& rClass);
std::ostream& operator<<(std::ostream& rStream, const SInterface& rInterface);
#endif

std::ostream& operator<<(std::ostream& rStream, const SDataType::EDataType eDataType);

void ResetLine();
int& GetLine();

#endif // _INTERFACEINFO_H_
