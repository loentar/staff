// This file generated by staff_codegen
// Service Implementation

#include <rise/common/Log.h>
#include <rise/xml/XMLNode.h>
#include <rise/string/String.h>
#include <rise/string/Encoding.h>
#include <rise/xml/XMLException.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/security/Security.h>
#include "WidgetManagerContext.h"
#include "WidgetManagerImpl.h"

namespace widget
{

  CWidgetManagerImpl::CWidgetManagerImpl():
    m_nUserId(-1)  
  {
  }

  CWidgetManagerImpl::~CWidgetManagerImpl()
  {
  }

  void CWidgetManagerImpl::Open(const rise::CString& sWidgetDB)
  {
    try
    {
      Close();

      m_sWidgetDB = sWidgetDB;

      m_sFileName = staff::CRuntime::Inst().GetComponentHome("widget");
      m_sFileName += "/db/widgetdb." + sWidgetDB + "." + rise::ToStr(GetUserId()) + ".xml";
      try
      {
        m_tDoc.LoadFromFile(m_sFileName);
      }
      catch(rise::xml::CXMLOpenException&)
      {
        rise::LogWarning() << "Widget DB will be created in " + m_sFileName;
        m_tDoc.LoadFromFile(staff::CRuntime::Inst().GetComponentHome("widget") + "/db/widgetdb_default.xml");
      }
      
      GetWidgetClasses();
      
      // widgets
      m_mWidgets.clear();
      rise::xml::CXMLNode& rNodeWidgets = m_tDoc.GetRoot().Subnode("Widgets");
      for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeWidgets = rNodeWidgets.NodeBegin(); 
            itNodeWidgets != rNodeWidgets.NodeEnd(); ++itNodeWidgets)
      {
        if ((itNodeWidgets->NodeType() == rise::xml::CXMLNode::ENTGENERIC) && 
            (itNodeWidgets->NodeName() == "WidgetItem"))
        {
          const rise::xml::CXMLNode& rNodeWidget = itNodeWidgets->Subnode("Widget");
          const rise::CString& sWidgetClassName = rNodeWidget["sClass"].AsString();
 
          for(TWidgetClassList::const_iterator itClass = m_lsWidgetClasses.begin(); itClass != m_lsWidgetClasses.end(); ++itClass)
          {
            if(itClass->sClass == sWidgetClassName)
            {
              SWidget stWidget;
              int nID = itNodeWidgets->Subnode("nID").NodeContent();
              const rise::xml::CXMLNode& rNodeProperties = rNodeWidget.Subnode("Properties");

              stWidget.sClass = rNodeWidget["sClass"].AsString();
              stWidget.sName = rNodeWidget["sName"].AsString();
              for (rise::xml::CXMLNode::TXMLNodeConstIterator itNodeProperties = rNodeProperties.NodeBegin(); 
                itNodeProperties != rNodeProperties.NodeEnd(); ++itNodeProperties)
              {
                if ((itNodeProperties->NodeType() == rise::xml::CXMLNode::ENTGENERIC) && 
                    (itNodeProperties->NodeName() == "Property"))
                {
                  SProperty stProperty;
                  stProperty.sName = (*itNodeProperties)["sName"].AsString();
                  stProperty.tValue = (*itNodeProperties)["tValue"].AsString();
                  stWidget.lsProperties.push_back(stProperty);
                }
              }
  
              m_mWidgets[nID] = stWidget;
              break;
            }
          }
          
        }
      } // for
    }
    catch(rise::CFileOpenException& /*rEx*/)
    {
      rise::LogWarning() << "first run";
    }
  }

  void CWidgetManagerImpl::Close()
  {
    if (m_sFileName == "")
    {
      return;
    }

    try
    {
      Commit();
    }
    RISE_CATCH_ALL

    m_sFileName = "";
  }

  TWidgetMap CWidgetManagerImpl::GetWidgetList() const
  {
    return m_mWidgets;
  }

  int CWidgetManagerImpl::AddWidget(const SWidget& rWidget)
  {
    int nID = 0;
    if(m_mWidgets.size() != 0)
      nID = m_mWidgets.rbegin()->first + 1;

    m_mWidgets[nID] = rWidget;
    
    return nID;
  }

  void CWidgetManagerImpl::DeleteWidget(int nWidgetID)
  {
    m_mWidgets.erase(nWidgetID);
  }

  void CWidgetManagerImpl::AlterWidget(int nWidgetID, const SWidget& rWidget)
  {
    TWidgetMap::iterator itWidget = m_mWidgets.find(nWidgetID);
    RISE_ASSERTES(itWidget != m_mWidgets.end(), staff::CRemoteException, "Widget does not exist");

    itWidget->second = rWidget;
  }

  void CWidgetManagerImpl::Commit()
  {
    RISE_ASSERTES(m_sFileName != "", staff::CRemoteException, "DB was not opened");

    rise::xml::CXMLNode& rNodeWidgets = m_tDoc.GetRoot().Subnode("Widgets");
    rNodeWidgets.Clear();
    for(TWidgetMap::iterator itWidget = m_mWidgets.begin(); itWidget != m_mWidgets.end(); ++itWidget)
    {
      rise::xml::CXMLNode& rNodeWidget1 = rNodeWidgets.AddSubNode("WidgetItem");
      rNodeWidget1.AddSubNode("nID").NodeContent() = itWidget->first;

      rise::xml::CXMLNode& rNodeWidget2 = rNodeWidget1.AddSubNode("Widget");
      rNodeWidget2.AddSubNode("sClass").NodeContent().AsString() = itWidget->second.sClass;
      rNodeWidget2.AddSubNode("sName").NodeContent().AsString() = itWidget->second.sName;
      rise::xml::CXMLNode& rNodeProperties = rNodeWidget2.AddSubNode("Properties");
      for (TPropertyList::const_iterator itProp = itWidget->second.lsProperties.begin();
        itProp != itWidget->second.lsProperties.end(); ++itProp)
      {
        rise::xml::CXMLNode& rNodeProperty = rNodeProperties.AddSubNode("Property");
        rNodeProperty.AddSubNode("sName").NodeContent().AsString() = itProp->sName;
        rNodeProperty.AddSubNode("tValue").NodeContent().AsString() = itProp->tValue.AsString();
      }
    }

    m_tDoc.SaveToFile(m_sFileName);
  }

  TWidgetClassList CWidgetManagerImpl::GetWidgetClassList()
  {
    return GetWidgetClasses();
  }

  const std::string& CWidgetManagerImpl::GetSessionId()
  {
    if(m_sSessionId.size() == 0)
    {
      m_sSessionId = CWidgetManagerContext::GetContext().GetServiceID(this);
    }
    
    return m_sSessionId;
  }

  int CWidgetManagerImpl::GetUserId()
  {
    if (m_nUserId == -1)
    {
      const std::string& sSession = GetSessionId();

      if(!StaffSecurityGetUserIdBySessionId(sSession.c_str(), &m_nUserId))
      {
        RISE_THROWS(staff::CRemoteException, "Unknown userid");
      }
    }
    
    return m_nUserId;
  }

  const TWidgetClassList& CWidgetManagerImpl::GetWidgetClasses()
  {
    // widget classes
    m_lsWidgetClasses.clear();

    m_sFileNameClasses = staff::CRuntime::Inst().GetComponentHome("widget");
    m_sFileNameClasses += "/db/widgetdb_classes." + m_sWidgetDB + ".xml";

    try
    {
      m_tDocClasses.LoadFromFile(m_sFileNameClasses);
    }
    catch(...)
    {
      m_sFileNameClasses = staff::CRuntime::Inst().GetComponentHome("widget");
      m_sFileNameClasses += "/db/widgetdb_classes.default.xml";
      rise::LogWarning() << "Loading default Widget class list!";
      m_tDocClasses.LoadFromFile(m_sFileNameClasses);
    }
    
    const rise::xml::CXMLNode& rNodeClasses = m_tDocClasses.GetRoot().Subnode("Classes");

    //!!! TODO: get type from db !!!
    TPermission tRootWidgetPerm;
    TObject tRootWidget;
    int nRootObjectId = 0;
    RISE_ASSERTS(StaffSecurityGetObjectByName("ROOTWIDGET", 4, &nRootObjectId, &tRootWidget), "Can't get root widget");

    TObject* pWidgetList = NULL;
    int nWidgetListSize = 0;
    RISE_ASSERTS(StaffSecurityGetObjectListByType(4, &tRootWidget.nObjectId, &pWidgetList, &nWidgetListSize), "can't get objects list");


    if(!StaffSecurityGetPermissionForUser(&tRootWidget, m_nUserId, &tRootWidgetPerm))
    {
      rise::LogWarning() << "can't get permission for root object " << tRootWidget.szObjectName << " setting no access(0-0-0)";
      tRootWidgetPerm.bRead = 0;
      tRootWidgetPerm.bWrite = 0;
      tRootWidgetPerm.bExecute = 0;
    }

    try
    {
      TPermission tPerm;
      
      for(rise::xml::CXMLNode::TXMLNodeConstIterator itClass = rNodeClasses.NodeBegin(); itClass != rNodeClasses.NodeEnd(); ++itClass)
      {
        bool bFound = false;
        SWidgetClass stWidgetClass;
        stWidgetClass.sClass = (*itClass)["Name"].AsString();
        stWidgetClass.sDescr = (*itClass)["Descr"].AsString();
        
        for(int i = 0; i < nWidgetListSize; ++i)
        {
          if(stWidgetClass.sClass == pWidgetList[i].szObjectName)
          {
            if(!StaffSecurityGetPermissionForUser(&pWidgetList[i], m_nUserId, &tPerm))
            {
              rise::LogWarning() << "can't get permission for object " << pWidgetList[i].szObjectName << " skipping..";
              continue;
            }
          
            if(tPerm.bRead)
            {
              rise::LogDebug1() << "adding object " << stWidgetClass.sClass << "(" << stWidgetClass.sDescr << ") to user " << m_nUserId;
              m_lsWidgetClasses.push_back(stWidgetClass);
            } 
            else
            {
              rise::LogDebug1() << "skipping object " << stWidgetClass.sClass << "(" << stWidgetClass.sDescr << ") to user " << m_nUserId;
            }

            bFound = true;
            break;
          }
        } // for
        
        if(!bFound)
        { // 
          if(tRootWidgetPerm.bRead)
          {
            rise::LogDebug1() << "adding object " << stWidgetClass.sClass << "(" << stWidgetClass.sDescr << ") to user " << m_nUserId << " by root object rights";
            m_lsWidgetClasses.push_back(stWidgetClass);
          } 
          else
          {
            rise::LogDebug1() << "skipping object " << stWidgetClass.sClass << "(" << stWidgetClass.sDescr << ") to user " << m_nUserId << " by root object rights";
          }
        }
      }
    }
    catch(...)
    {
      StaffSecurityFreeObjectList(pWidgetList);
      throw;
    }

    StaffSecurityFreeObjectList(pWidgetList);

    return m_lsWidgetClasses;
  }

}
