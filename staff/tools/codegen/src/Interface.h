#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <string>
#include <list>
#include <map>

namespace staff
{
  typedef std::list<std::string> TStringList; //!< string list
  typedef std::map<std::string, std::string> TStringMap; //!< string map

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
    /*! non-const, not a ref, Generic datatype */
    SDataType():
      bIsConst(false), bIsRef(false), eType(EGeneric)
    {
    }
  };

  //!  parameter
  struct SParam
  {
    SDataType    stDataType;  //!<  param data type
    std::string  sName;       //!<  param name
    std::string  sDescr;      //!<  param description
    std::string  sDetail;     //!<  detailed description
    TStringMap   mOptions;    //!<  param metacomments options
  };

  //!  service operation
  struct SMember
  {
    SParam             stReturn;       //!<  return type
    std::string        sName;          //!<  operation name
    std::list<SParam>  lsParamList;    //!<  parameters
    bool               bIsConst;       //!<  operation is const
    std::string        sDescr;         //!<  operation description
    std::string        sDetail;        //!<  detailed description
    bool               bIsAsynch;      //!<  operation is asynchronous(client)
    TStringMap         mOptions;       //!<  member metacomments options

    SMember():
      bIsConst(false), bIsAsynch(false)
    {
    }
  };

  //! service class
  struct SClass
  {
    std::string         sName;          //!<  class name
    std::string         sNamespace;     //!<  namespace
    std::string         sDescr;         //!<  service description
    std::string         sDetail;        //!<  detailed description
    TStringMap          mOptions;       //!<  class metacomments options
    TStringList         lsModules;      //!<  axis2/c modules list to engage
    std::list<SMember>  lsMember;       //!<  service operations
  };

  //! struct
  struct SStruct
  {
    std::string         sName;            //!<  struct name
    std::string         sNamespace;       //!<  namespace
    std::string         sParentName;      //!<  parent struct name (with namespace as used)
    std::string         sParentNamespace; //!<  parent struct namespace (actual)
    std::string         sDescr;           //!<  struct description
    std::string         sDetail;          //!<  detailed description
    std::list<SParam>   lsMember;         //!<  struct fields
    bool                bForward;         //!<  is forward declaration
    bool                bExtern;          //!<  extern declaration
    std::string         sOwnerName;       //!<  owner struct name in format StructName[::SubStructName]
    std::list<SStruct>  lsStruct;         //!<  sub struct list

    SStruct():
      bForward(true), bExtern(false)
    {
    }
  };

  //! typedef
  struct STypedef
  {
    std::string         sName;          //!<  typedef name
    std::string         sNamespace;     //!<  namespace
    std::string         sDescr;         //!<  description
    std::string         sDetail;        //!<  detailed description
    SDataType           stDataType;     //!<  base data type
    bool                bExtern;        //!<  extern declaration

    STypedef():
      bExtern(false)
    {
    }
  };

  //! include info
  struct SInclude
  {
    std::string sInterfaceName;      //!<  interface name
    std::string sNamespace;          //!<  interface namespace
    std::string sFileName;           //!<  file name
    std::string sTargetNs;           //!<  target namespace
  };

  //! service interface
  struct SInterface
  {
    std::string           sName;          //!<  interface name, based on input filename
    std::string           sNamespace;     //!<  interface main namespace
    std::string           sFileName;      //!<  input filename
    std::string           sTargetNs;      //!<  target namespace
    std::list<SInclude>   lsInclude;      //!<  included files
    std::list<STypedef>   lsTypedef;      //!<  typedef list
    std::list<SStruct>    lsStruct;       //!<  struct list
    std::list<SClass>     lsClass;        //!<  service classes list
  };

  //! project
  struct SProject
  {
    std::string            sName;         //!<  project name
    std::string            sNamespace;    //!<  component namespace
    std::list<SInterface>  lsInterfaces;  //!<  interface list
  };
}

#endif // _INTERFACE_H_
