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

    //! copy operator
    SDataType& operator=(const SDataType& stDataType)
    {
      bIsConst = stDataType.bIsConst;
      bIsRef = stDataType.bIsRef;
      eType = stDataType.eType;
      sName = stDataType.sName;
      sNodeName = stDataType.sNodeName;
      sNamespace = stDataType.sNamespace;
      sUsedName = stDataType.sUsedName;
      lsParams = stDataType.lsParams;

      return *this;
    }
  };

  //!  parameter
  struct SParam
  {
    SDataType    stDataType;  //!<  param data type
    std::string  sName;       //!<  param name
    std::string  sDescr;      //!<  param description

    SParam& operator=(const SParam& rParam)
    {
      stDataType = rParam.stDataType;
      sName = rParam.sName;
      return *this;
    }
  };

  //!  service operation
  struct SMember
  {
    SParam             stReturn;    //!<  return type
    std::string        sName;       //!<  operation name
    std::list<SParam>  lsParamList; //!<  parameters
    bool               bIsConst;    //!<  operation is const
    std::string        sDescr;      //!<  operation description
    std::string        sDetail;     //!<  detailed description
    std::string        sSoapAction; //!<  soap action
    std::string        sNodeName;   //!<  node name

    SMember():
      bIsConst(false)
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
    std::string         sServiceUri;    //!<  default service URI
    bool                bLoadAtStartup; //!<  load service at startup
    std::list<SMember>  lsMember;       //!<  service operations
  };

  //! struct
  struct SStruct
  {
    std::string         sName;          //!<  struct name
    std::string         sNamespace;     //!<  namespace
    std::string         sParent;        //!<  parent struct(inherits)
    std::string         sDescr;         //!<  struct description
    std::string         sDetail;        //!<  detailed description
    std::list<SParam>   lsMember;       //!<  struct fields
    bool                bForward;       //!<  is forward declaration

    SStruct():
      bForward(true)
    {
    }
  };

  //! typedef
  struct STypedef
  {
    std::string         sName;          //!<  typedef name
    std::string         sNamespace;     //!<  namespace
    std::string         sDescr;         //!<  description
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
    std::list<SInterface>  lsInterfaces;  //!<  interface list
  };
}

#endif // _INTERFACE_H_
