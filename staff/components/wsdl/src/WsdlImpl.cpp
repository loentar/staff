// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// Service Implementation

#include <fstream>
#include <sstream>
#include <staff/utils/Log.h>
#include <staff/utils/File.h>
#include <staff/common/Exception.h>
#include <staff/common/Runtime.h>
#include <staff/common/Operation.h>
#include <staff/common/Attribute.h>
#include <staff/common/MessageContext.h>
#include "WsdlImpl.h"

namespace staff
{
namespace wsdl
{

void WsdlImpl::Get(const std::string& sComponent, const std::string& sFile, Operation& rOperation)
{
  const std::string& sHost = rOperation.GetMessageContext().GetPropertyValue("HttpHost");

  // check filename
  STAFF_ASSERT(sFile.find_first_of("/\\") == std::string::npos, "Invalid filename");
  STAFF_ASSERT(sComponent.find_first_of("/\\") == std::string::npos, "Invalid component");

  // path to file
  const std::string& sFilePath = staff::Runtime::Inst().GetComponentHome(sComponent) +
                                     STAFF_PATH_SEPARATOR + sFile;

  std::ifstream ifsWsdlFile(sFilePath.c_str());
  STAFF_ASSERT(ifsWsdlFile.good(), "Can't open [" + sFilePath + "]");

  staff::DataObject tdoWsdl;

  std::stringstream ssWsdl;
  ssWsdl << ifsWsdlFile.rdbuf();
  tdoWsdl.FromString(ssWsdl.str());

  // finding service url..
  staff::DataObject::Iterator itService = tdoWsdl.FindChildByLocalName("service");
  if (itService != tdoWsdl.End())
  {
    staff::DataObject tdoService = *itService;
    for (staff::DataObject::Iterator itPort = tdoService.Begin(); itPort != tdoService.End(); ++itPort)
    {
      if (itPort->GetLocalName() == "port")
      {
        staff::DataObject tdoPort = *itPort;
        staff::DataObject::Iterator itAddress = tdoPort.FindChildByLocalName("address");
        if (itAddress != tdoPort.End())
        {
          staff::DataObject tdoAddress = *itAddress;
          staff::DataObject::AttributeIterator itAttrLocation =
              tdoAddress.FindAttributeByLocalName("location");
          if (itAttrLocation != tdoAddress.AttributeEnd())
          {
            // modyfying location
            std::string sLocation = itAttrLocation->GetText();

            std::string::size_type nPosBegin = sLocation.find("://");
            if (nPosBegin != std::string::npos)
            {
              nPosBegin += 3;

              std::string::size_type nPosEnd = sLocation.find('/', nPosBegin);
              if (nPosEnd != std::string::npos)
              {
                sLocation.replace(nPosBegin, nPosEnd - nPosBegin, sHost);
                itAttrLocation->SetText(sLocation);
              }
              else
              {
                LogWarning() << "Can't find host end in wsdl";
              }
            }
            else
            {
              LogWarning() << "Can't find protocol specification in wsdl";
            }

          }
        }
      }
    }
  }

  rOperation.SetResponse(tdoWsdl);
}


}
}

