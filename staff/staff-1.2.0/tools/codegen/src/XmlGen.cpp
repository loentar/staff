#include <list>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include <rise/string/String.h>
#include "InterfaceInfo.h"
#include "XmlGen.h"

using rise::xml::CXMLNode;

template<typename TType>
CXMLNode& operator<<(CXMLNode& rStream, const std::list<TType>& rList)
{
  for(typename std::list<TType>::const_iterator it = rList.begin(); it != rList.end(); ++it)
    rStream << *it;
  return rStream;
}

bool GetDataType(std::string& sOut, const SDataType& rDataType)
{
  sOut += (rDataType.bIsConst ? "const " : "") + 
    (rDataType.sNamespace == "::" ? "" : rDataType.sNamespace) + rDataType.sName;

  bool bIsTemplate = rDataType.lsParams.size() != 0;
  if (!bIsTemplate)
  {
    sOut += (rDataType.bIsRef ? "&" : "");
    return false;
  }

  sOut += "< ";
  for(std::list<SDataType>::const_iterator it = rDataType.lsParams.begin(); it != rDataType.lsParams.end(); ++it)
  {
    if (it != rDataType.lsParams.begin())
      sOut += ", ";
    bIsTemplate = GetDataType(sOut, *it);
  }
  if(bIsTemplate)
    sOut += ' ';
  sOut += " >";

  sOut += (rDataType.bIsRef ? "&" : "");
  return true;
}

CXMLNode& operator<<(CXMLNode& rNodeDataTypes, const SDataType& rDataType)
{
  std::string sMangledName = (rDataType.sNamespace.substr(0, 2) == "::") ?
    rDataType.sNamespace.substr(2) : rDataType.sNamespace;
  rise::StrReplace(sMangledName, "::", "_", true);

  rNodeDataTypes.AddSubNode(" const ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["IsConst"] = rDataType.bIsConst;
  rNodeDataTypes.AddSubNode(" Ссылка ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["IsRef"] = rDataType.bIsRef;
  rNodeDataTypes.AddSubNode(" Имя типа ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["Name"] = (rDataType.sNamespace == "::" ? "" : rDataType.sNamespace) + rDataType.sName;
  rNodeDataTypes.AddSubNode(" Тип ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["Type"] = rDataType.eType;
  rNodeDataTypes.AddSubNode(" Имя типа с пространством имен ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["MangledName"] = sMangledName + rDataType.sName;

  rNodeDataTypes.NodeContent() = "";
  GetDataType(rNodeDataTypes.NodeContent().AsString(), rDataType);

  bool bIsTemplate = rDataType.lsParams.size() != 0;
  rNodeDataTypes.AddSubNode(" Шаблон ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["IsTemplate"] = bIsTemplate;
  
  rNodeDataTypes.AddSubNode(" Параметры шаблона ", CXMLNode::ENTCOMMENT);
  CXMLNode& rNodeTemplateParams = rNodeDataTypes.AddSubNode("TemplateParams");
  int nNum = 1;
  for(std::list<SDataType>::const_iterator it = rDataType.lsParams.begin(); it != rDataType.lsParams.end(); ++it, ++nNum)
    rNodeTemplateParams.AddSubNode("TemplateParam" + rise::ToStr(nNum)) << *it;

  if (rDataType.eType == SDataType::EUnknown)
  {
    std::cerr << "Warning: Unknown datatype: " << (rDataType.sNamespace + rDataType.sName) << std::endl;
  }
  

  return rNodeDataTypes;
}

CXMLNode& operator<<(CXMLNode& rNodeParams, const SParam& rParam)
{
  rNodeParams.AddSubNode(" Параметр ", CXMLNode::ENTCOMMENT);
  CXMLNode& rNodeParam = rNodeParams.AddSubNode("Param");
  
  rNodeParam.AddSubNode(" Имя параметра ", CXMLNode::ENTCOMMENT);
  rNodeParam["Name"] = rParam.sName;
  rNodeParam.AddSubNode(" Тип параметра ", CXMLNode::ENTCOMMENT);
  rNodeParam.AddSubNode("DataType") << rParam.stDataType;

  if (rNodeParams.NodeContent() != "")
    rNodeParams.NodeContent().AsString() += ", ";
    
  GetDataType(rNodeParams.NodeContent().AsString(), rParam.stDataType);
  rNodeParams.NodeContent().AsString() += " " + rParam.sName;

  return rNodeParams;
}

CXMLNode& operator<<(CXMLNode& rNodeMembers, const SMember& rMember)
{
  rNodeMembers.AddSubNode(" Функция ", CXMLNode::ENTCOMMENT);
  CXMLNode& rNodeMember = rNodeMembers.AddSubNode("Member");

  rNodeMember.AddSubNode(" Возвращаемый тип ", CXMLNode::ENTCOMMENT);
  rNodeMember.AddSubNode("Return") << rMember.stReturn;
  rNodeMember["Name"] = rMember.sName;
  rNodeMember.AddSubNode(" Является ли функция константной ", CXMLNode::ENTCOMMENT);
  rNodeMember["IsConst"] = rMember.bIsConst;
  rNodeMember["Const"] = rMember.bIsConst ? " const" : "";
  rNodeMember.AddSubNode("Params") << rMember.lsParamList;

  return rNodeMembers;
}

CXMLNode& operator<<(CXMLNode& rNodeClasses, const SClass& rClass)
{
  CXMLNode& rNodeClass = rNodeClasses.AddSubNode("Class");
  std::string sClassNs = 
    (rClass.sNamespace.substr(0, 2) == "::") ? rClass.sNamespace.substr(2) : rClass.sNamespace;
  std::string sServiceName = 
    ((rClass.sName[0] == 'C' || rClass.sName[0] == 'I') ? rClass.sName.substr(1) : rClass.sName);

  std::string sServiceNamespace = sClassNs;
  rise::StrReplace(sServiceNamespace, "::", ".", true);
  std::string sServiceNsName = sServiceNamespace + sServiceName;

  rNodeClass.AddSubNode(" Имя класса ", CXMLNode::ENTCOMMENT);
  rNodeClass["Name"] = rClass.sName;
  rNodeClass.AddSubNode(" Имя класса с пространством имен ", CXMLNode::ENTCOMMENT);
  rNodeClass["NsName"] = rClass.sNamespace + rClass.sName;
  rNodeClass.AddSubNode(" Имя сервиса ", CXMLNode::ENTCOMMENT);
  rNodeClass["ServiceName"] = sServiceName;
  rNodeClass.AddSubNode(" Имя сервиса с пространством имен ", CXMLNode::ENTCOMMENT);
  rNodeClass["ServiceNsName"] = sServiceNsName;
  rNodeClass.AddSubNode(" Пространство имен сервиса ", CXMLNode::ENTCOMMENT);
  rNodeClass["ServiceNamespace"] = sServiceNamespace;
  rNodeClass.AddSubNode(" Имя компонента ", CXMLNode::ENTCOMMENT);
  rNodeClass["ComponentName"] = 
    sServiceNamespace.size() == 0 ? sServiceNamespace : sServiceNamespace.substr(0, sServiceNamespace.size() - 1);
  rNodeClass.AddSubNode(" Пространство имен класса ", CXMLNode::ENTCOMMENT);
  rNodeClass["Namespace"] = rClass.sNamespace;
  rNodeClass.AddSubNode(" Функции класса ", CXMLNode::ENTCOMMENT);
  rNodeClass.AddSubNode("Members") << rClass.lsMember;

  std::string sOpeningNs;
  std::string sEndingNs;
  std::string::size_type nPos = 0;
  std::string::size_type nPrevPos = 0;
  while((nPos = sClassNs.find("::", nPos)) != std::string::npos)
  {
    sOpeningNs += "namespace " + sClassNs.substr(nPrevPos, nPos - nPrevPos) + "\n{\n";
    sEndingNs += "}\n";
    nPos += 2;
    nPrevPos = nPos;
  }

  rNodeClass.AddSubNode(" Открывающее пространство имен ", CXMLNode::ENTCOMMENT);
  rNodeClass["OpeningNs"] = sOpeningNs;
  rNodeClass.AddSubNode(" Закрывающее пространство имен ", CXMLNode::ENTCOMMENT);
  rNodeClass["EndingNs"] = sEndingNs;

  return rNodeClasses;
}

CXMLNode& operator<<(CXMLNode& rNodeStructs, const SStruct& rStruct)
{
  CXMLNode& rNodeStruct = rNodeStructs.AddSubNode("Struct");

  std::string sMangledName = (rStruct.sNamespace.substr(0, 2) == "::") ?
    rStruct.sNamespace.substr(2) : rStruct.sNamespace;
  rise::StrReplace(sMangledName, "::", "_", true);

  rNodeStruct.AddSubNode(" Значение структуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct["NativeName"] = (rStruct.sName[0] == 'S' ? rStruct.sName.substr(1) : rStruct.sName);
  rNodeStruct.AddSubNode(" Имя структуры с пространством имен ", CXMLNode::ENTCOMMENT);
  rNodeStruct["MangledName"] = sMangledName + rStruct.sName;
  
  rNodeStruct.AddSubNode(" Имя стурктуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct["Name"] = rStruct.sNamespace + rStruct.sName;
  rNodeStruct.AddSubNode(" Свойства структуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct.AddSubNode("Members") << rStruct.lsMember;

  return rNodeStructs;
}

CXMLNode& operator<<(CXMLNode& rNodeTypedefs, const STypedef& rTypedef)
{
  CXMLNode& rNodeTypedef = rNodeTypedefs.AddSubNode("Typedef");
  std::string sMangledName = (rTypedef.sNamespace.substr(0, 2) == "::") ?
    rTypedef.sNamespace.substr(2) : rTypedef.sNamespace;
  rise::StrReplace(sMangledName, "::", "_", true);

  rNodeTypedef.AddSubNode(" Имя определения типа ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["Name"] = rTypedef.sNamespace + rTypedef.sName;
  rNodeTypedef.AddSubNode(" Имя определения типа с пространством имен ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["MangledName"] = sMangledName + rTypedef.sName;
  rNodeTypedef.AddSubNode(" Определяемый тип данных ", CXMLNode::ENTCOMMENT);
  rNodeTypedef.AddSubNode("DataType") << rTypedef.stDataType;

  return rNodeTypedefs;
}

CXMLNode& operator<<(CXMLNode& rNodeInterfaces, const SInterface& rInterface)
{
  CXMLNode& rNodeInterface = rNodeInterfaces.AddSubNode("Interface");
  
  rNodeInterface.AddSubNode(" Имя интерфейса ", CXMLNode::ENTCOMMENT);
  rNodeInterface["Name"] = rInterface.sName;
  rNodeInterface.AddSubNode(" Имя файла с интерфейсом ", CXMLNode::ENTCOMMENT);
  rNodeInterface["FileName"] = rInterface.sFileName;

  rNodeInterface.AddSubNode(" Классы в интерфейсе ", CXMLNode::ENTCOMMENT);
  rNodeInterface.AddSubNode("Classes") << rInterface.lsClass;

  rNodeInterface.AddSubNode(" Структуры в интерфейсе ", CXMLNode::ENTCOMMENT);
  rNodeInterface.AddSubNode("Structs") << rInterface.lsStruct;

  rNodeInterface.AddSubNode(" Типы данных в интерфейсе ", CXMLNode::ENTCOMMENT);
  rNodeInterface.AddSubNode("Typedefs") << rInterface.lsTypedef;

  return rNodeInterfaces;
}

CXMLNode& operator<<(CXMLNode& rRootNode, const SProject& rProject)
{
  rRootNode.NodeName() = "Project";
  rRootNode["Name"] = rProject.sName;
  rRootNode.AddSubNode(" Интерфейсы в проекте ", CXMLNode::ENTCOMMENT);
  rRootNode.AddSubNode("Interfaces") << rProject.lsInterfaces;

  return rRootNode;
}
