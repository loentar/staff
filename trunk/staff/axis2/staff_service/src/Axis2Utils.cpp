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

#include <axutil_qname.h>
#include <axis2_svc.h>
#include <string>
#include <rise/os/oscommon.h>
#include <rise/common/Log.h>
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/component/ServiceWrapper.h>
#include <staff/component/Component.h>

bool Axis2UtilsCreateVirtualService(const staff::CServiceWrapper* pServiceWrapper,
                                    const struct axutil_env* pEnv, struct axis2_conf* pConf)
{  
  if (!pServiceWrapper)
  {
    rise::LogError() << "pService is NULL";
    return false;
  }

  const std::string& sServiceName = pServiceWrapper->GetName();

  std::string sServiceGroupName = sServiceName + "SvcGroup";

  std::string sServiceUri = "http://staff.tempui.org:9090/axis2/services/";
  sServiceUri += sServiceName.c_str();
  std::string sWsdlPath = staff::CRuntime::Inst().GetComponentHome(pServiceWrapper->GetComponent()->GetName())
      + RISE_PATH_SEPARATOR + pServiceWrapper->GetName() + ".wsdl";

  axutil_qname_t* pQName = axutil_qname_create(pEnv, sServiceName.c_str(), sServiceUri.c_str(), NULL);
  if (!pQName)
  {
    rise::LogError() << "pQName";
    return false;
  }

  axis2_svc_t* pAxis2Service = axis2_svc_create_with_qname(pEnv, pQName);
  if (!pAxis2Service)
  {
    axutil_qname_free(pQName, pEnv);
    rise::LogError() << "pAxis2Service";
    return false;
  }
  axutil_qname_free(pQName, pEnv);

  // set "virtual" service flag
  static char szParamName[] = "IsStaffVirtualService";
  // allocate data C-way, so axis2/c can free it
  int* pnParam = reinterpret_cast<int*>(AXIS2_MALLOC(pEnv->allocator, sizeof(int)));
  if (!pnParam)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "can't allocate param";
    return false;
  }
  *pnParam = 1;
  axutil_param_t* pParam = axutil_param_create(pEnv, static_cast<axis2_char_t*>(szParamName), pnParam);
  if (!pParam)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "pParam";
    return false;
  }

  if (axis2_svc_add_param(pAxis2Service, pEnv, pParam) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "pParam";
    return false;
  }

  { // adding operations
    const staff::CDataObject& rdoOperations = pServiceWrapper->GetOperations();
    for (staff::CDataObject tdoOperation = rdoOperations.FirstChild();
        !tdoOperation.IsNull(); tdoOperation.SetNextSibling())
    {
      const std::string& sOpName = tdoOperation.GetChildByLocalName("Name").GetText();
      struct axis2_op* pOperation = axis2_op_create(pEnv);
      pQName = axutil_qname_create(pEnv, sOpName.c_str(), NULL, NULL);
      if (!pQName)
      {
        axis2_svc_free(pAxis2Service, pEnv);
        rise::LogError() << "pQName(op)";
        return false;
      }

      if (axis2_op_set_qname(pOperation, pEnv, pQName) != AXIS2_SUCCESS)
      {
        axutil_qname_free(pQName, pEnv);
        axis2_svc_free(pAxis2Service, pEnv);
        rise::LogError() << "axis2_op_set_qname";
        return false;
      }

      axutil_qname_free(pQName, pEnv);

      // add REST params if exists
      std::string sRestMethod;
      std::string sRestLocation;

      {
        const staff::CDataObject& rdoOperationConst = tdoOperation;
        staff::CDataObject::ConstIterator itRestTmp = rdoOperationConst.FindChildByLocalName("RestMethod");
        if (itRestTmp != rdoOperationConst.End())
        {
          sRestMethod = itRestTmp->GetText();
        }

        itRestTmp = rdoOperationConst.FindChildByLocalName("RestLocation");
        if (itRestTmp != rdoOperationConst.End())
        {
          sRestLocation = itRestTmp->GetText();
        }
      }

      if (!sRestLocation.empty() && !sRestMethod.empty())
      {
        if (axis2_op_set_rest_http_method(pOperation, pEnv, sRestMethod.c_str()) != AXIS2_SUCCESS)
        {
          rise::LogError() << "Failed to set \"RESTMethod\" to service ["
              << sServiceName <<"] operation " << sOpName;
        }
        else
        {
          rise::LogDebug2() << "\"RESTMethod=" << sRestMethod << "\" param set to service ["
              << sServiceName <<"] operation " << sOpName;
        }

        if (axis2_op_set_rest_http_location(pOperation, pEnv, sRestLocation.c_str()) != AXIS2_SUCCESS)
        {
          rise::LogError() << "Failed to set \"RESTLocation\" to service ["
              << sServiceName <<"] operation " << sOpName;
        }
        else
        {
          rise::LogDebug2() << "\"RESTLocation=" << sRestLocation << "\" param set to service ["
              << sServiceName <<"] operation " << sOpName;
        }

        if (axis2_svc_add_rest_mapping(pAxis2Service, pEnv, sRestMethod.c_str(), sRestLocation.c_str(),
                                       pOperation) != AXIS2_SUCCESS)
        {
          rise::LogError() << "Failed to set REST mapping to service ["
              << sServiceName <<"] operation " << sOpName;
        }
      }

      // add service operation
      if (axis2_svc_add_op(pAxis2Service, pEnv, pOperation) != AXIS2_SUCCESS)
      {
        axis2_svc_free(pAxis2Service, pEnv);
        rise::LogError() << "axis2_svc_add_op";
        return false;
      }
    }
  }

  std::string sServiceDescr = pServiceWrapper->GetDescr();
  FILE* pFile = fopen(sWsdlPath.c_str(), "rb");
  if (pFile)
  {
    fclose(pFile);
    sServiceDescr += "&nbsp;<sup>[<a style='font-size: x-small' href='/axis2/services/staff.wsdl.Wsdl/get/"
                     + pServiceWrapper->GetComponent()->GetName() + "/"
                     + pServiceWrapper->GetName() + ".wsdl'>wsdl</a>]</sup>";
    if (axis2_svc_set_svc_wsdl_path(pAxis2Service, pEnv, sWsdlPath.c_str()) != AXIS2_SUCCESS)
    {
      rise::LogError() << "axis2_svc_set_svc_wsdl_path";
    }
  }

  if (axis2_svc_set_svc_desc(pAxis2Service, pEnv, sServiceDescr.c_str()) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "axis2_svc_set_svc_desc";
    return false;
  }

  // creating service group
  axis2_svc_grp_t* pServiceGroup = axis2_svc_grp_create_with_conf(pEnv, pConf);
  if (!pServiceGroup)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "axis2_svc_grp_create_with_conf";
    return false;
  }

  if (axis2_svc_grp_set_name(pServiceGroup, pEnv, sServiceGroupName.c_str()) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "axis2_svc_grp_set_name";
    return false;
  }

  if (axis2_svc_grp_add_svc(pServiceGroup, pEnv, pAxis2Service) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "axis2_svc_grp_add_svc";
    return false;
  }

  // adding to configuration
  if (axis2_conf_add_svc_grp(pConf, pEnv, pServiceGroup) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    axis2_svc_free(pAxis2Service, pEnv);
    rise::LogError() << "axis2_conf_add_svc_grp";
    return false;
  }

  return true;
}

bool Axis2UtilsRemoveVirtualService(const std::string& sServiceName, 
                                    const struct axutil_env* pEnv,
                                    struct axis2_conf* pConf,
                                    bool bIsShuttingDown)
{
  axis2_svc* pService = axis2_conf_get_svc(pConf, pEnv, sServiceName.c_str());
  if (!pService)
  {
    rise::LogError() << "axis2_conf_get_svc";
    return false;
  }

  if (!bIsShuttingDown)
  {
    std::string sServiceGroupName = sServiceName.c_str();
    sServiceGroupName += "SvcGroup";

    // removing from conf
    axis2_svc_grp* pServiceGroup = axis2_conf_get_svc_grp(pConf, pEnv, sServiceGroupName.c_str());
    if (!pServiceGroup)
    {
      rise::LogError() << "axis2_conf_get_svc_grp";
      return false;
    }

    axutil_hash_t* pAllServiceGroups = axis2_conf_get_all_svc_grps(pConf, pEnv);
    if (!pAllServiceGroups)
    {
      rise::LogError() << "axis2_conf_get_all_svc_grps";
      return false;
    }

    axutil_hash_set(pAllServiceGroups, sServiceGroupName.c_str(), AXIS2_HASH_KEY_STRING, NULL);

    axis2_svc_grp_free(pServiceGroup, pEnv);
  }

  // axis2_svc_grp_free does not destroy the services in group
  if (axis2_conf_remove_svc(pConf, pEnv, sServiceName.c_str()) != AXIS2_SUCCESS)
  {
    rise::LogError() << "axis2_conf_remove_svc";
    return false;
  }

  axis2_svc_free(pService, pEnv);

  return true;
}
