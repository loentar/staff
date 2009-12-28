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
#include <staff/common/Runtime.h>
#include <staff/common/DataObject.h>
#include <staff/common/Value.h>
#include <staff/component/Service.h>
#include <staff/component/Component.h>

bool Axis2UtilsCreateVirtualService( const std::string& sServiceName, const staff::CService* pService, void* pSvcClass, const struct axutil_env* pEnv, struct axis2_conf* pConf )
{  
  if(pService == NULL)
  {
    printf("pService is NULL\n");
    return false;
  }

  std::string sServiceGroupName = sServiceName + "SvcGroup";

  std::string sServiceUri = "http://staff.tempui.org:9090/axis2/services/";
  sServiceUri += sServiceName.c_str();
  std::string sWsdlPath = staff::CRuntime::Inst().GetComponentHome(pService->GetComponent()->GetName()) 
      + RISE_PATH_SEPARATOR + pService->GetName() + ".wsdl";

  axutil_qname_t* pQName = axutil_qname_create(pEnv, sServiceName.c_str(), sServiceUri.c_str(), NULL);
  if(pQName == NULL)
  {
    printf("error pQName\n");
    return false;
  }

  axis2_svc_t* pAxis2Service = axis2_svc_create_with_qname(pEnv, pQName);
  if(pAxis2Service == NULL)
  {
    axutil_qname_free(pQName, pEnv);
    printf("error pAxis2Service\n");
    return false;
  }
  axutil_qname_free(pQName, pEnv);

  // set "virtual" service flag
  axutil_param_t* pParam = axutil_param_create(pEnv, "IsStaffVirtualService", new bool(true));
  if(pParam == NULL)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error pParam\n");
    return false;
  }

  if(axis2_svc_add_param(pAxis2Service, pEnv, pParam) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error pParam\n");
    return false;
  }

  { // adding operations
    const staff::CDataObject& rdoOperations = pService->GetOperations();
    for (staff::CDataObject::ConstIterator it = rdoOperations.Begin(); it != rdoOperations.End(); ++it)
    {
      std::string sOpName = (*it)["Name"];
      struct axis2_op* pOperation = axis2_op_create(pEnv);
      pQName = axutil_qname_create(pEnv, sOpName.c_str(), NULL, NULL);
      if(pQName == NULL)
      {
        axis2_svc_free(pAxis2Service, pEnv);
        printf("error pQName(op)\n");
        return false;
      }

      if(axis2_op_set_qname(pOperation, pEnv, pQName) != AXIS2_SUCCESS)
      {
        axutil_qname_free(pQName, pEnv);
        axis2_svc_free(pAxis2Service, pEnv);
        printf("error axis2_op_set_qname\n");
        return false;
      }

      axutil_qname_free(pQName, pEnv);

      if(axis2_svc_add_op(pAxis2Service, pEnv, pOperation) != AXIS2_SUCCESS)
      {
        axis2_svc_free(pAxis2Service, pEnv);
        printf("error axis2_svc_add_op\n");
        return false;
      }
    }
  }

  // set service description
  if(axis2_svc_set_name(pAxis2Service, pEnv, sServiceName.c_str()) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_set_name\n");
    return false;
  }

  std::string sServiceDescr = pService->GetDescr();
  FILE* pFile = fopen(sWsdlPath.c_str(), "rb");
  if (pFile)
  {
    fclose(pFile);
    sServiceDescr += "&nbsp;<sup><a style='font-size: x-small' href='/axis2/services/" + sServiceName + "?wsdl'>[wsdl]</a></sup>";
    if(axis2_svc_set_svc_wsdl_path(pAxis2Service, pEnv, sWsdlPath.c_str()) != AXIS2_SUCCESS)
    {
      printf("error axis2_svc_set_svc_wsdl_path\n");
    }
  }

  if(axis2_svc_set_svc_desc(pAxis2Service, pEnv, sServiceDescr.c_str()) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_set_svc_desc\n");
    return false;
  }

  if(axis2_svc_set_impl_class(pAxis2Service, pEnv, pSvcClass) != AXIS2_SUCCESS)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_set_impl_class\n");
    return false;
  }

  // creating service group
  axis2_svc_grp_t* pServiceGroup = axis2_svc_grp_create_with_conf(pEnv, pConf);
  if(pServiceGroup == NULL)
  {
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_grp_create_with_conf\n");
    return false;
  }

  if(axis2_svc_grp_set_name(pServiceGroup, pEnv, sServiceGroupName.c_str()) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_grp_set_name\n");
    return false;
  }

  if(axis2_svc_grp_add_svc(pServiceGroup, pEnv, pAxis2Service) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    axis2_svc_free(pAxis2Service, pEnv);
    printf("error axis2_svc_grp_add_svc\n");
    return false;
  }

  // adding to configuration
  if(axis2_conf_add_svc_grp(pConf, pEnv, pServiceGroup) != AXIS2_SUCCESS)
  {
    axis2_svc_grp_free(pServiceGroup, pEnv);
    printf("error axis2_conf_add_svc_grp\n");
    return false;
  }

  return true;
}

bool Axis2UtilsRemoveVirtualService(const std::string& sServiceName, 
                                    const struct axutil_env* pEnv, struct axis2_conf* pConf)
{
  std::string sServiceGroupName = sServiceName.c_str();
  sServiceGroupName += "SvcGroup";
  
  // removing from conf
  axis2_svc_grp* pServiceGroup = axis2_conf_get_svc_grp(pConf, pEnv, sServiceGroupName.c_str());
  if(pServiceGroup == NULL)
  {
    printf("error axis2_conf_get_svc_grp\n");
    return false;
  }

  axis2_svc* pService = axis2_conf_get_svc(pConf, pEnv, sServiceName.c_str());
  if(pService == NULL)
  {
    printf("error axis2_conf_get_svc\n");
    return false;
  }

  if(axis2_svc_set_impl_class(pService, pEnv, NULL) != AXIS2_SUCCESS)
  {
    printf("error axis2_svc_set_impl_class\n");
    return false;
  }

  if(axis2_conf_remove_svc(pConf, pEnv, sServiceName.c_str()) != AXIS2_SUCCESS)
  {
    printf("error axis2_conf_remove_svc\n");
    return false;
  }

  axutil_hash_t *pAllServiceGroups = axis2_conf_get_all_svc_grps(pConf, pEnv);
  if(pAllServiceGroups == NULL)
  {
    printf("error axis2_conf_get_all_svc_grps\n");
    return false;
  }

  axutil_hash_set(pAllServiceGroups, sServiceGroupName.c_str(), AXIS2_HASH_KEY_STRING, NULL);

  axis2_svc_grp_free(pServiceGroup, pEnv);
  return true;
}
