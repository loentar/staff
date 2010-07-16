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

#include <rise/common/Log.h>
#include <staff/common/DataObject.h>
#include <staff/common/Namespace.h>
#include <staff/common/Exception.h>
#include <staff/client/Options.h>
#include <staff/client/ServiceClient.h>

int main(int nArgs, const char* paszArgs[])
{
  std::string sRestMethod = "POST";
  std::string sAddress = "http://localhost:9090/axis2/services/echo/echoString";

  if (nArgs > 1)
  {
    if (strcmp(paszArgs[1], "-h") == 0)
    {
      std::cerr << "Usage: " << paszArgs[0] << "[ -m<REST_METHOD>][ <ENDPOINT_URL>]" << std::endl
                << " Valid <REST_METHOD> are: POST(default), GET, HEAD, DELETE, PUT" << std::endl
                << "NOTE: You can test for other HTTP methods by changing the services.xml of the echo service" << std::endl
                << " and providing the correct REST HTTP method and the location to be used for operation." << std::endl
                << " Also note that you have to restart the server after changing the services.xml." << std::endl;
      return 0;
    }

    for (int nArg = 1; nArg < nArgs; ++nArg)
    {
      if (strncmp(paszArgs[nArg], "-m", 2) == 0)
      {
        sRestMethod = paszArgs[nArg] + 2;
      }
      else
      {
        sAddress = paszArgs[nArg];
      }
    }
  }

  std::cout << "Using method: " << sRestMethod << std::endl;
  std::cout << "Using address: " << sAddress << std::endl;

  try
  {
    staff::CServiceClient tClient;

    // initialize client
    tClient.Init(sAddress);

    // client options
    staff::COptions& rOptions = tClient.GetOptions();

    // set namespace for each request
    rOptions.SetDefaultNamespace("http://ws.apache.org/axis2/services/echo", "ns1");

    // enabling rest
    rOptions.EnableRest();
    // set rest method
    rOptions.SetHttpMethod(sRestMethod);


    // build payload
    staff::CDataObject tdoPayload("echoString");
    tdoPayload.CreateChild("text").SetText("Hello World!");

    // output request
    std::cout << "Request: \n-------\n" << tdoPayload.ToString() << "\n------\n";

    // invoke service synchronously
    staff::CDataObject tdoResult = tClient.Invoke(tdoPayload);

    // output result
    std::cout << "Result: \n-------\n" << tdoResult.ToString() << "\n------\n";
  }
  RISE_CATCH_ALL

  return 0;
}

