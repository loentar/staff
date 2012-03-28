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

#include <string.h>
#include <iostream>
#include <staff/utils/Log.h>
#include <staff/common/DataObject.h>
#include <staff/common/Namespace.h>
#include <staff/common/Exception.h>
#include <staff/client/Options.h>
#include <staff/client/ServiceClient.h>

int main(int nArgs, const char* paszArgs[])
{
  std::string sHttpUserName = "";
  std::string sHttpPassword = "";
  std::string sProxyUserName = "";
  std::string sProxyPassword = "";
  std::string sAddress = "http://localhost:9090/axis2/services/echo/echoString";

  if (nArgs > 1)
  {
    if (strcmp(paszArgs[1], "-h") == 0)
    {
      std::cerr << "Usage: " << paszArgs[0]
                << "(-a [USERNAME] [PASSWORD]) (-p [USERNAME] [PASSWORD]) [ENDPOINT_URL]" << std::endl
                << "use -a option for HTTP Authentication" << std::endl
                << "use -p option for Proxy Authentication" << std::endl;
      return 0;
    }

    for (int nArg = 1; nArg < nArgs; ++nArg)
    {
      if (strcmp(paszArgs[nArg], "-a"))
      { // http auth
        ++nArg;
        if (paszArgs[nArg] == NULL)
        {
          std::cerr << "username for http auth is not set!" << std::endl;
          return 1;
        }

        sHttpUserName = paszArgs[nArg];

        ++nArg;
        if (paszArgs[nArg] == NULL)
        {
          std::cerr << "password for http auth is not set!" << std::endl;
          return 1;
        }

        sHttpPassword = paszArgs[nArg];
      }
      else
      if (strcmp(paszArgs[nArg], "-p"))
      { // proxy auth
        ++nArg;
        if (paszArgs[nArg] == NULL)
        {
          std::cerr << "username for proxy auth is not set!" << std::endl;
          return 1;
        }

        sProxyUserName = paszArgs[nArg];

        ++nArg;
        if (paszArgs[nArg] == NULL)
        {
          std::cerr << "password for proxy auth is not set!" << std::endl;
          return 1;
        }

        sProxyPassword = paszArgs[nArg];
      }
      else
      {
        sAddress = paszArgs[nArg];
      }
    }
  }

  std::cout << "Using address: " << sAddress << std::endl;

  try
  {
    staff::ServiceClient tClient;

    // initialize client
    tClient.Init();

    // client options
    staff::Options& rOptions = tClient.GetOptions();


    // testing auth is needed or not
    bool bHttpAuthRequired = false;
    bool bProxyAuthRequired = false;

    // setting rest method
    rOptions.EnableRest();
    rOptions.SetHttpMethod("HEAD");

    // setting dummy auth info
    if (!sHttpUserName.empty() && !sHttpPassword.empty())
    {
      rOptions.SetHttpAuthInfo("", "", "");
    }
    if (!sProxyUserName.empty() && !sProxyPassword.empty())
    {
      rOptions.SetProxyAuthInfo("", "", "");
    }

    // force authentication tests
    rOptions.SetTestHttpAuth(true);
    rOptions.SetTestProxyAuth(true);

    // uncomment line below to setup proxy server from code
    // tClient.SetProxyWithAuth("127.0.0.1", "3128", "username", "password");

    // sending robust authentication test message
    tClient.TestAuthRequired();

    // checking whether authentication is required
    if (tClient.GetHttpAuthRequired())
    {
      bHttpAuthRequired = true;
      std::cout << "Using http auth: [" << sHttpUserName << ":" << sHttpPassword << "]" << std::endl;
      rOptions.SetHttpAuthInfo(sHttpUserName, sHttpPassword, tClient.GetAuthType());
      tClient.TestAuthRequired();
    }

    if (tClient.GetProxyAuthRequired())
    {
      bProxyAuthRequired = true;
      std::cout << "Using proxy auth: [" << sProxyUserName << ":" << sProxyPassword << "]" << std::endl;
      rOptions.SetProxyAuthInfo(sProxyUserName, sProxyPassword, tClient.GetAuthType());
      tClient.TestAuthRequired();
    }

    // cancel authentication tests
    rOptions.SetTestHttpAuth(false);
    rOptions.SetTestProxyAuth(false);


    // print whether authentication was required
    std::cout << "HTTP Authentication " << (bHttpAuthRequired ? "info" : "is not") << " required." << std::endl;
    std::cout << "Proxy Authentication " << (bProxyAuthRequired ? "info" : "is not") << " required." << std::endl;

    // disabling REST for SOAP request
    rOptions.EnableRest(false);
    rOptions.SetHttpMethod("POST");



    // creating payload and invoking service

    // set namespace for each request
    rOptions.SetDefaultNamespace("http://ws.apache.org/axis2/services/echo", "ns1");

    // engage addressing module
    tClient.EngageModule("addressing");

    // set wsa:To
    rOptions.SetToAddress("http://localhost:9090/axis2/services/echo");

    // set wsa:Action
    rOptions.SetAction("http://ws.apache.org/axis2/c/samples/echoString");

    // create payload
    staff::DataObject tdoPayload("echoString");
    // create and set request
    tdoPayload.CreateChild("text").SetText("Hello World!");

    // output request
    std::cout << "Request: \n-------\n" << tdoPayload.ToString() << "\n------\n";

    // invoke service synchronously
    staff::DataObject tdoResult = tClient.Invoke(tdoPayload);

    // output result
    std::cout << "Result: \n-------\n" << tdoResult.ToString() << "\n------\n";
  }
  STAFF_CATCH_ALL

  return 0;
}

