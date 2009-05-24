#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

static const char szUploadDir[] = "/tmp/upload/";
static const char* szHtmlUploadOk = 
"<html xmlns=\"http://www.w3.org/1999/xhtml\">"
  "<head>"
    "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
  "</head>"
  "<body>"
    "<div id=\"divSuccess\"/>"
  "</body>"
"</html>";

struct SStream
{
  request_rec* pReq;
  char szBuffer[HUGE_STRING_LEN];
  int nDataSize;
  const char* szPos;
};

typedef struct SStream TStream;


/*static int PrintRequest(request_rec* pReq)
{
  int nResult = OK;

  nResult = ap_setup_client_block(pReq, REQUEST_CHUNKED_ERROR);
  if (nResult != OK)
  {
    return nResult;
  }

  if (ap_should_client_block(pReq))
  {
    char szBuffer[HUGE_STRING_LEN];
    int nReaded;

    while ((nReaded = ap_get_client_block(pReq, szBuffer, sizeof(szBuffer))) > 0) 
    {
      ap_rwrite(szBuffer, nReaded, pReq);
    }
  }

  return nResult;
}*/

static int GetBoundaryString(request_rec* pReq, char* szBoundary, unsigned nBoundarySize)
{
  unsigned nPos = 0;
  const char* szHeader = apr_table_get(pReq->headers_in, "Content-Type");
  const char* szBoundaryBegin = strstr(szHeader, "boundary=");
  
  if (szBoundaryBegin == NULL)
  {
    ap_rputs("Can't find Content-type header", pReq);
    return 505;
  }
  
  szBoundaryBegin += 9; // skip "boundary="
  
  szBoundary[0] = '-';
  szBoundary[1] = '-';
  do
  {
    if (nPos == nBoundarySize)
    {
      pReq->content_type = "text/plain";
      ap_rputs("boundary string is too long", pReq);
      return 505;
    }

    if (strchr(" \t\n ;", szBoundaryBegin[nPos]))
    {
      szBoundary[nPos + 2] = '\0';
      break;
    }

    szBoundary[nPos + 2] = szBoundaryBegin[nPos];

    ++nPos;
  }
  while (szBoundary[nPos + 1] != '\0');

  return OK;
}

static int OpenStream(request_rec* pReq, TStream* pStream)
{
  int nResult = ap_setup_client_block(pReq, REQUEST_CHUNKED_ERROR);
  if (nResult != OK)
  {
    return nResult;
  }

  if (!ap_should_client_block(pReq))
  {
    return 1;
  }
  
  pStream->nDataSize = ap_get_client_block(pReq, pStream->szBuffer, HUGE_STRING_LEN);
  if (pStream->nDataSize <= 0)
  {
    return 2;
  }

  pStream->pReq = pReq;
  pStream->szPos = pStream->szBuffer;
  
  return OK;
}

static const char* FindData(const char* szWhere, int nWhereSize, const char* szWhat, int nWhatSize)
{
  const char* szPos = szWhere;
  const char* szWhereEnd = szWhere + nWhereSize;
  
  for (szPos = szWhere; szPos < (szWhere + nWhereSize); ++szPos)
  {
    if (!strncmp(szPos, szWhat, nWhatSize))
    {
      return szPos;
    }
  }
  
  return NULL;
}

static int GetNextData(TStream* pStream, const char** szData, int* pnLineSize, const char* szBoundary, int nBoundarySize, int* pbFound)
{
  int nReaded = 0;
  int nDataRests = pStream->szBuffer + pStream->nDataSize - pStream->szPos;
  const char* szBoundaryPos = NULL;
  
  szBoundaryPos = FindData(pStream->szPos, nDataRests, szBoundary, nBoundarySize);

  // if boundary found
  if (szBoundaryPos != NULL)
  {
    *szData = pStream->szPos;
    *pnLineSize = szBoundaryPos - pStream->szPos;
    pStream->szPos = szBoundaryPos + nBoundarySize;
    *pbFound = 1;
    return OK;
  }
  else
  {
    // not found
  
    // data rests is greater that boundary size: return rest-bondary
    if (nDataRests > nBoundarySize)
    {
      // ordinary data block
      *szData = pStream->szPos;
      *pnLineSize = nDataRests - nBoundarySize;
      pStream->szPos += *pnLineSize;
      *pbFound = 0;
      return OK;
    }

    {
      char* szBufferAppend = NULL;

      // not enough data
      // need to append buffer
      // move rest of data to buffer begin

      memmove(pStream->szBuffer, pStream->szPos, nDataRests);

      pStream->nDataSize = nDataRests;
      pStream->szPos = pStream->szBuffer; // reset buffer pos
      szBufferAppend = pStream->szBuffer + pStream->nDataSize;

      // appending
      nReaded = ap_get_client_block(pStream->pReq, szBufferAppend, HUGE_STRING_LEN - pStream->nDataSize);
      

      if (nReaded == 0)
      {
        // was last data
        *szData = pStream->szPos;
        *pnLineSize = pStream->nDataSize;
        pStream->szPos += *pnLineSize;
        *pbFound = 0;
        return -1;
      }


      if (nReaded < 0)
      {
        // some error while reading
        return -2;
      }
      
      pStream->nDataSize += nReaded;
      
      // trying to search again
      szBoundaryPos = FindData(pStream->szPos, pStream->nDataSize, szBoundary, nBoundarySize);
      if (szBoundaryPos != NULL)
      { // found
        *szData = pStream->szPos;
        *pnLineSize = szBoundaryPos - pStream->szPos;
        pStream->szPos = szBoundaryPos + nBoundarySize;
        *pbFound = 1;
      }
      else
      { // not found: ordinary data block
        *szData = pStream->szPos;
        *pnLineSize = pStream->nDataSize - nBoundarySize;
        pStream->szPos += *pnLineSize;
        *pbFound = 0;
      }
    }
  }
  
  return OK;
}

static int GetFileName(const char* szBuffer, char* szFileName, int nFileNameSize, request_rec* pReq)
{
  const char* szFileNameBegin = NULL;
  
  szFileNameBegin = strstr(szBuffer, "filename=");
  if (szFileNameBegin != NULL)
  {
    const char* szFileNameEnd = NULL;
    const char* szTmp = NULL;

    szFileNameBegin += 9;
    szFileNameEnd = strchr(szFileNameBegin + 1, szFileNameBegin[0]);
    if (!szFileNameEnd)
    {
      ap_rputs("can't find end of filename\n", pReq);
      return 1;
    }

    ++szFileNameBegin;

    // cut-off file path
    for (szTmp = szFileNameEnd; szTmp != szFileNameBegin; --szTmp)
    {
      if (*szTmp == '\\' || *szTmp == '/')
      {
        szFileNameBegin = szTmp + 1;
        break;
      }
    }
    
    if (nFileNameSize < (szFileNameEnd - szFileNameBegin + sizeof(szUploadDir)))
    {
      ap_rputs("szFileName is too small\n", pReq);
      return 1;
    }
    
    strcpy(szFileName, szUploadDir);
    strncat(szFileName, szFileNameBegin, szFileNameEnd - szFileNameBegin);
    szFileName[sizeof(szUploadDir) + szFileNameEnd - szFileNameBegin] = '\0';
    return OK;
  }

  return -1;
}

static int UploadFile(request_rec* pReq, const char* szBoundary)
{
  char szFilePath[PATH_MAX];
  int nResult = OK;
  const char* szData = NULL;
  int nDataSize = 0;
  int nFileFd = 0;
  int nBoundarySize = strlen(szBoundary);
  int bEof = 0;
  int nFileSize = 0;
  
  TStream tStream;
  
  nResult = OpenStream(pReq, &tStream);
  if (nResult != OK)
  {
    return nResult;
  }
  
  
  while(!bEof)
  {
    int bFound = 0;
    nResult = GetNextData(&tStream, &szData, &nDataSize, szBoundary, nBoundarySize, &bFound);

    if(nResult == -1)
    {
      bEof = 1;
    }
    
    if(nResult == -2)
    {
      ap_rputs("error GetNextData\n\n", pReq);
      return 2;
    }

    if (nFileFd)
    {
      if (bFound)
      {
        // don't write \n\r before boundary
        nDataSize -= 2;
      }

      nResult = write(nFileFd, szData, nDataSize);
      if (nResult == -1)
      {
        ap_rputs("error writing: ", pReq);
        ap_rputs(strerror(errno), pReq);
        ap_rputs("\n\n", pReq);
        close(nFileFd);
        return 3;
      }

      nFileSize += nDataSize;
    }
    
    if (bFound)
    {
      mode_t tUmask;

      if (nFileFd)
      {
        close(nFileFd);
        nFileFd = 0;

        if (nFileSize == 0)
        {
          unlink(szFilePath);
          return 4;
        }
      }

      // reading data before \n
      nResult = GetNextData(&tStream, &szData, &nDataSize, "\n", 1, &bFound);

      if (!(strncmp(szData, "--", 2)))
      {
        //  EOF
        return OK;
      }

      if(nResult == -2)
      {
        ap_rputs("error GetNextData 3\n\n", pReq);
        return 2;
      }

      // reading header
      nResult = GetNextData(&tStream, &szData, &nDataSize, "\n", 1, &bFound);
      if(nResult == -2)
      {
        ap_rputs("error GetNextData 2\n\n", pReq);
        return 2;
      }

      nResult = GetFileName(szData, szFilePath, PATH_MAX, pReq);

      if(nResult == OK)
      {
        // skipping headers
        nResult = GetNextData(&tStream, &szData, &nDataSize, "\r\n\r\n", 4, &bFound);
        if (nResult != OK || !bFound)
        {
          ap_rputs("can\'t find file begin\n\n", pReq);
          return 2;
        }

        tUmask = umask(0);
        mkdir(szUploadDir, S_IRWXU | S_IRWXG | S_IRWXO);

        nFileFd = open(szFilePath, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (!nFileFd)
        {
          ap_rputs("error opening: ", pReq);
          ap_rputs(strerror(errno), pReq);
          ap_rputs("\n\n", pReq);
          umask(tUmask);
          return 2;
        }

        nFileSize = 0;

        umask(tUmask);
      } // if filename
    } // if bFound
  } // for

  ap_rputs("EOF not found\n", pReq);
  return 2;
}

/* content handler */
static int FileUploaderHandler(request_rec* pReq)
{
  if (strcmp(pReq->handler, "fileuploader"))
  {
    return DECLINED;
  }
  
  if (pReq->header_only || pReq->method_number != M_POST)
  {
    return OK;
  }

  {
    int nResult = OK;
    char szBoundary[64];

    nResult = GetBoundaryString(pReq, szBoundary, sizeof(szBoundary));
    if (nResult != OK)
    {
      char szTmp[256];
      sprintf(szTmp, "error: #%d\n", nResult);
      ap_rputs(szTmp, pReq);
      pReq->content_type = "text/plain";
      return 404;
    }
    
    nResult = UploadFile(pReq, szBoundary);
    if (nResult != OK)
    {
      char szTmp[256];
      sprintf(szTmp, "error: #%d\n", nResult);
      ap_rputs(szTmp, pReq);
      pReq->content_type = "text/plain";
      return 500;
    }
  }
  
  pReq->content_type = "text/html";
  ap_rputs(szHtmlUploadOk, pReq);

  return OK;
}

static void FileUploaderRegisterHooks(apr_pool_t *p)
{
  ap_hook_handler(FileUploaderHandler, NULL, NULL, APR_HOOK_MIDDLE);
}

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA fileuploader_module = 
{
  STANDARD20_MODULE_STUFF, 
  NULL,                  /* create per-dir    config structures */
  NULL,                  /* merge  per-dir    config structures */
  NULL,                  /* create per-server config structures */
  NULL,                  /* merge  per-server config structures */
  NULL,                  /* table of config file commands       */
  FileUploaderRegisterHooks  /* register hooks                      */
};

