// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// Client skeleton

#include <stdio.h>
#include <iostream>
#include <rise/threading/SharedPtr.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/console.h>
#include <staff/common/Exception.h>
#include "ServiceFactory.h"
#foreach $(Project.Interfaces)
#include "$(Interface.Name).h"
#end

int main(int nArgs, const char* paszArgs[])
{
#foreach $(Project.Interfaces)
#ifneq($(Interface.Classes.$Count),0)
  try
  {
#foreach $(Interface.Classes)
    {
      rise::CSharedPtr< $(Class.NsName) > p$(Class.ServiceName) = 
        ::staff::CServiceFactory::Inst().GetService< $(Class.NsName) >();

      RISE_ASSERTES(p$(Class.ServiceName) != NULL, rise::CLogicNoItemException, "Cannot get client for service $(Class.ServiceNsName)!");

      // Invoke Your service here:
#foreach $(Class.Members)
      // \
#ifneq($(Member.Return.Name),void)
$(Member.Return.Name) $(Member.Return.NodeName) = \
#else
\
#ifeqend
p$(Class.ServiceName)->$(Member.Name)(\
#foreach $(Member.Params)
#ifeq($(Param.$Num),0)
\
#else
, \
#ifeqend
$(Param.Name)\
#end
);
#end
    }
#end
  }
  catch(const staff::CRemoteException& rEx)
  {
    rise::LogError() << rEx.GetDescr();
  }
  RISE_CATCH_ALL

#else
\
#ifeqend
#end
  return 0;
}
