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

#include <iostream>
#include <staff/utils/Log.h>
#include <staff/common/DataObject.h>
#include <staff/common/Namespace.h>
#include <staff/common/Exception.h>
#include <staff/client/Options.h>
#include <staff/client/ServiceClient.h>

int main(int /*nArgs*/, const char* /*paszArgs*/[])
{
  try
  {
    staff::ServiceClient tClient;

    // initialize client
    tClient.Init();

    // client options
    staff::Options& rOptions = tClient.GetOptions();

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

