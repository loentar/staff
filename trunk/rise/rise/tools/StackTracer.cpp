#include <rise/common/Log.h>
#include <rise/tools/AddrInfoReader.h>
#include "StackTracer.h"


#if defined __linux__
#include <execinfo.h>

#define BACKTRACE_SIZE 100

namespace rise
{
  namespace tools
  {
    void CStackTracer::StackTraceStr( CString& sTraceResult, int nSkip /*= 0*/ )
    {
      int nPointerCount;
      SAddrInfo stAddrInfo;
      void* pAddr[BACKTRACE_SIZE];

      rise::LogDebug3() << "tracing stack(str)...";

      nPointerCount = backtrace(pAddr, BACKTRACE_SIZE);

      sTraceResult = "";

      for (int i = nSkip; i < nPointerCount; i++)
      {
        stAddrInfo.Reset();
        CAddrInfoReader::LookupAddr(pAddr[i], stAddrInfo);

        sTraceResult += stAddrInfo.sAddrHex;
        sTraceResult += ": ";
        sTraceResult += stAddrInfo.sContext;
        sTraceResult += "(";
        sTraceResult += stAddrInfo.sFileName;
        sTraceResult += "[";
        if(stAddrInfo.unLine == 0)
          sTraceResult += "?";
        else
          sTraceResult += ToStr(stAddrInfo.unLine);
        sTraceResult += "]): ";
        sTraceResult += stAddrInfo.sFunctionName;
        sTraceResult += "\n";
      }

      rise::LogDebug3() << "done tracing stack(str)";
      
      const char* szRiseShowStack = getenv("RISE_SHOW_STACK");
      if (szRiseShowStack != NULL && (!strcmp(szRiseShowStack, "1") || !strcmp(szRiseShowStack, "TRUE")) )
      {
        rise::LogDebug3() << "STACK:\n" << sTraceResult << "\n";
      }
    }

    void CStackTracer::StackTrace( TAddrInfoList& rTraceResult, int nSkip /*= 0*/ )
    {
      int nPointerCount;
      SAddrInfo stAddrInfo;
      void* pAddr[BACKTRACE_SIZE];

      rise::LogDebug3() << "tracing stack(list)...";

      nPointerCount = backtrace(pAddr, BACKTRACE_SIZE);

      rTraceResult.clear();

      for (int i = nSkip; i < nPointerCount; i++)
      {
        stAddrInfo.Reset();
        CAddrInfoReader::LookupAddr(pAddr[i], stAddrInfo);
        rTraceResult.push_back(stAddrInfo);
      }

      rise::LogDebug3() << "done tracing stack(list)";
    }
  }
}
#endif
#if defined WIN32
#include <windows.h>
#include <imagehlp.h>

namespace rise
{
  namespace tools
  {
    void CStackTracer::StackTraceStr( CString& sTraceResult, int nSkip /*= 0*/  )
    {
      sTraceResult = "";
      TAddrInfoList tTraceList;

      rise::LogDebug3() << "tracing stack(str)...";

      StackTrace(tTraceList, nSkip + 1);
      for(TAddrInfoList::const_iterator it = tTraceList.begin(); it != tTraceList.end(); ++it)
      {
        sTraceResult += it->sAddrHex;
        sTraceResult += ": ";
        sTraceResult += it->sContext;
        sTraceResult += "(";
        sTraceResult += it->sFileName;
        sTraceResult += "[";
        if(it->unLine == 0)
          sTraceResult += "?";
        else
          sTraceResult += ToStr(it->unLine);
        sTraceResult += "]): ";
        sTraceResult += it->sFunctionName;
        sTraceResult += "\n";
      }

      rise::LogDebug3() << "done tracing stack(str)";
    }

    void CStackTracer::StackTrace( TAddrInfoList& rTraceResult, int nSkip /*= 0*/  )
    {
      HANDLE hProc, hThread;
      CONTEXT* pContext = 0;
      STACKFRAME tStackFrame;
      DWORD machType = IMAGE_FILE_MACHINE_I386;
      DWORD filepathlen = 0;
      char szFilePath[MAX_PATH];
      char* pszLastDir = NULL;
      char* pszPath = NULL;
      DWORD tebp = 0;
      DWORD tesp = 0;

      _asm mov tesp, esp
      _asm mov tebp, ebp

      rise::LogDebug3() << "tracing stack(list)...";

      // Get image filename of the main executable
      filepathlen = GetModuleFileName ( NULL, szFilePath, sizeof(szFilePath));
      if (filepathlen == 0)
        printf ("NtStackTrace: Failed to get pathname for program\n");

      // Strip the filename, leaving the path to the executable
      pszLastDir = strrchr (szFilePath, '/');
      if (pszLastDir == NULL)
        pszLastDir = strrchr (szFilePath, '\\');
      if (pszLastDir != NULL)
        pszLastDir[0] = '\0';

      // Initialize the symbol table routines, supplying a pointer to the path
      pszPath = szFilePath;
      if (strlen (szFilePath) == 0)
        pszPath = NULL;

      hProc = GetCurrentProcess ();
      hThread = GetCurrentThread ();
      if ( !SymInitialize(hProc, pszPath, TRUE) )
        printf ("NtStackTrace: failed to initialize symbols\n");

      // Allocate and initialize frame and symbol structures
      memset (&tStackFrame, 0, sizeof(STACKFRAME));
      tStackFrame.AddrPC.Mode = AddrModeFlat; 
      tStackFrame.AddrStack.Mode = AddrModeFlat; 
      tStackFrame.AddrFrame.Mode = AddrModeFlat;
      tStackFrame.AddrStack.Offset = tesp; 
      tStackFrame.AddrFrame.Offset = tebp;
      tStackFrame.AddrPC.Offset = reinterpret_cast<DWORD>(&StackTrace) + 0x08c;

      SAddrInfo rAddr;
      while(StackWalk( machType, hProc, hThread, &tStackFrame, pContext, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL ))
      {
        if(nSkip >= 0)
          --nSkip;
        else
        {
          CAddrInfoReader::LookupAddr(reinterpret_cast<void*>(tStackFrame.AddrPC.Offset), rAddr);
          rTraceResult.push_back(rAddr);
        }
      }

      if (pContext != NULL)
        free (pContext);

      rise::LogDebug3() << "done tracing stack(list)";
    }

  }
}
#endif
