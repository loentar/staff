/*
 *  Copyright 2011 Utkin Dmitry
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

#include <axutil_env.h>
#include <axis2_http_client.h>
#include <axis2_http_simple_request.h>
#include <axis2_http_simple_response.h>
#include <string>
#include <rise/common/ExceptionTemplate.h>
#include "HttpClient.h"

namespace staff
{
namespace codegen
{
  bool HttpClient::Get(const std::string& sUrl, std::string& sResponse)
  {
    bool bResult = false;
    static axutil_env_t* pEnv =
        axutil_env_create_all("staff_codegen_wsdl_http_client.log", AXIS2_LOG_LEVEL_TRACE);
    RISE_ASSERTS(pEnv, "Can't create axis2 env");

    sResponse.erase();

    axis2_http_client_t* pClient = NULL;
    axis2_http_simple_request_t* pRequest = NULL;
    axis2_http_request_line_t* pRequestLine = NULL;
    axutil_url_t* pUrl = NULL;
    axis2_http_header_t* pHeader = NULL;
    axis2_http_simple_response_t* pResponse = NULL;
    char* szBody = NULL;

    // axutil_url_parse_string strips get parameters
    std::string sPath;
    std::string::size_type nPos = sUrl.find("://");
    RISE_ASSERTS(nPos != std::string::npos, "invalid url: [" + sUrl + "]");
    if (sUrl.substr(0, nPos) != "file")
    { // skip host and port
      nPos = sUrl.find('/', nPos + 3);
    }
    else
    { // skip protocol only
      nPos += 2;
    }

    if (nPos == std::string::npos)
    {
      sPath = '/';
    }
    else
    {
      sPath = sUrl.substr(nPos);
    }

    pUrl = axutil_url_parse_string(pEnv, sUrl.c_str());
    pRequestLine = axis2_http_request_line_create(pEnv, "GET", sPath.c_str(), "HTTP/1.0");
    pRequest = axis2_http_simple_request_create(pEnv, pRequestLine, NULL, 0, NULL);

    pHeader = axis2_http_header_create(pEnv, "Host", axutil_url_get_host(pUrl, pEnv));
    axis2_http_simple_request_add_header(pRequest, pEnv, pHeader);

    pClient = axis2_http_client_create(pEnv, pUrl);
    if (pClient)
    {
      // don't check return code here, because client returns AXIS2_SUCCESS only if some body was sent
      // we have'nt body, and client returns AXIS2_FAILURE
      axis2_http_client_send(pClient, pEnv, pRequest, NULL);
      int nStatus = axis2_http_client_recieve_header(pClient, pEnv);
      rise::LogDebug3() << "Status code: " << nStatus;
      if (nStatus == 200) // HTTP 200 OK
      {
        pResponse = axis2_http_client_get_response(pClient, pEnv);
        if (pResponse)
        {
          axis2_ssize_t nBodySize = 0;
#ifdef _DEBUG
          rise::LogDebug2() << "Content Type: " << axis2_http_simple_response_get_content_type(pResponse, pEnv);
          rise::LogDebug2() << "Content Length: " << axis2_http_simple_response_get_content_length(pResponse, pEnv);
#endif
          axutil_stream_t* pStream = axis2_http_simple_response_get_body(pResponse, pEnv);

          int nContentLength = axis2_http_simple_response_get_content_length(pResponse, pEnv);

          szBody = reinterpret_cast<char*>(AXIS2_MALLOC(pEnv->allocator, sizeof(char) * nContentLength));

          int nReaded = 0;
          while ((nReaded = axutil_stream_read(pStream, pEnv, szBody + nBodySize, nContentLength - nBodySize)) > 0)
          {
            nBodySize += nReaded;
          }

          sResponse.assign(szBody, nBodySize);

          AXIS2_FREE(pEnv->allocator, szBody);
          bResult = true;
        }
      }
      axis2_http_client_free(pClient, pEnv);
    }
    else
    {
      axis2_http_request_line_free(pRequestLine, pEnv);
      axutil_url_free(pUrl, pEnv);
    }

    axis2_http_simple_request_free(pRequest, pEnv);

    rise::LogDebug3() << sResponse.size() << " bytes retrieved";
    return bResult;
  }

}
}
