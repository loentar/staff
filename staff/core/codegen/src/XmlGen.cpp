#include <list>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
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
  sOut += (rDataType.bIsConst ? "const " : "") + rDataType.sName;

  bool bIsTemplate = rDataType.lsParams.size() != 0;
  if (!bIsTemplate)
  {
    sOut += (rDataType.bIsRef ? "&" : "");
    return false;
  }

  sOut += '<';
  for(std::list<SDataType>::const_iterator it = rDataType.lsParams.begin(); it != rDataType.lsParams.end(); ++it)
  {
    if (it != rDataType.lsParams.begin())
      sOut += ", ";
    bIsTemplate = GetDataType(sOut, *it);
  }
  if(bIsTemplate)
    sOut += ' ';
  sOut += '>';

  sOut += (rDataType.bIsRef ? "&" : "");
  return true;
}

CXMLNode& operator<<(CXMLNode& rNodeDataTypes, const SDataType& rDataType)
{
  rNodeDataTypes.AddSubNode(" const ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["IsConst"] = rDataType.bIsConst;
  rNodeDataTypes.AddSubNode(" Ссылка ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["IsRef"] = rDataType.bIsRef;
  rNodeDataTypes.AddSubNode(" Имя типа ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["Name"] = rDataType.sName;
  rNodeDataTypes.AddSubNode(" Тип ", CXMLNode::ENTCOMMENT);
  rNodeDataTypes["Type"] = rDataType.eType;

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

  rNodeClass.AddSubNode(" Имя сервиса ", CXMLNode::ENTCOMMENT);
  rNodeClass["ServiceName"] = ((rClass.sName[0] == 'C' || rClass.sName[0] == 'I') ? rClass.sName.substr(1) : rClass.sName);
  rNodeClass.AddSubNode(" Имя класса ", CXMLNode::ENTCOMMENT);
  rNodeClass["Name"] = rClass.sName;
  rNodeClass.AddSubNode(" Функции класса ", CXMLNode::ENTCOMMENT);
  rNodeClass.AddSubNode("Members") << rClass.lsMember;

  return rNodeClasses;
}

CXMLNode& operator<<(CXMLNode& rNodeStructs, const SStruct& rStruct)
{
  CXMLNode& rNodeStruct = rNodeStructs.AddSubNode("Struct");

  rNodeStruct.AddSubNode(" Значение структуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct["NativeName"] = (rStruct.sName[0] == 'S' ? rStruct.sName.substr(1) : rStruct.sName);
  rNodeStruct.AddSubNode(" Имя стурктуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct["Name"] = rStruct.sName;
  rNodeStruct.AddSubNode(" Свойства структуры ", CXMLNode::ENTCOMMENT);
  rNodeStruct.AddSubNode("Members") << rStruct.lsMember;

  return rNodeStructs;
}

CXMLNode& operator<<(CXMLNode& rNodeTypedefs, const STypedef& rTypedef)
{
  CXMLNode& rNodeTypedef = rNodeTypedefs.AddSubNode("Typedef");

  rNodeTypedef.AddSubNode(" Имя определения типа ", CXMLNode::ENTCOMMENT);
  rNodeTypedef["Name"] = rTypedef.sName;
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
  rNodeInterface.AddSubNode(" Пространство имен интерфейса ", CXMLNode::ENTCOMMENT);
  rNodeInterface["Namespace"] = rInterface.sNamespace;

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
