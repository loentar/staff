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


#if defined __linux__ && defined WITH_BFD
#include <dlfcn.h>
#include <execinfo.h>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bfd.h>
#include <limits.h>
#include <string.h>
#elif defined WIN32
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#endif
#include <string>
#include <map>
#include "tostring.h"
#include "File.h"
#include "Process.h"
#include "Error.h"
#include "Log.h"
#include "StackTracer.h"

namespace staff
{

#if defined __linux__ && defined WITH_BFD
// compat with old bfd.h
#ifndef bfd_get_section_size
#define bfd_get_section_size(ptr) ((ptr)->_raw_size)
#endif
#ifndef FALSE
#ifdef BFD_TRUE_FALSE
#define FALSE false
#else
#define FALSE bfd_fffalse
#endif
#endif
#ifndef TRUE
#ifdef BFD_TRUE_FALSE
#define TRUE true
#else
#define TRUE bfd_tttrue
#endif
#endif

  class BfdReader
  {
  public:
    struct FindAddrParam
    {
      bfd_vma tPc;
      asymbol** ppSymbols;

      const char* szFileName;
      unsigned int unLine;
      const char* szFunction;
      bool bFound;

      FindAddrParam():
        tPc(0),
        ppSymbols(NULL),
        szFileName(NULL),
        unLine(0),
        szFunction(NULL),
        bFound(false)
      {
      }
    };


    BfdReader():
      m_ppSymbols(NULL),
      m_pBfd(NULL),
      m_bInit(false)
    {
    }

    ~BfdReader()
    {
      Close();
    }

    bool Open(const char* szFileName)
    {
      char** pszMatching = NULL;

      m_bInit = true;

      Close();

      m_pBfd = bfd_openr(szFileName, NULL);
      if (!m_pBfd)
      {
        LogError() << "error while opening file: [" << szFileName << "]: " <<
                      Error::GetLastErrorStr();
        return false;
      }

      if (bfd_check_format(m_pBfd, bfd_archive))
      {
        LogError() << "Error while checking file format: [" << szFileName << "]";
        return false;
      }

      if (!bfd_check_format_matches(m_pBfd, bfd_object, &pszMatching))
      {
        LogError() << "Error while checking file format: [" << szFileName << "]";
        return false;
      }

      if ((bfd_get_file_flags(m_pBfd) & HAS_SYMS) != 0)
      {
        unsigned int unSize = 0;
        if (!bfd_read_minisymbols(m_pBfd, FALSE, (void**)&m_ppSymbols, &unSize))
        {
          free(m_ppSymbols);
          bfd_read_minisymbols(m_pBfd, TRUE, (void**)&m_ppSymbols, &unSize);
        }
      }

      return true;
    }

    void Close()
    {
      if (m_ppSymbols)
      {
        free(m_ppSymbols);
        m_ppSymbols = NULL;
      }

      if (m_pBfd)
      {
        bfd_close(m_pBfd);
        m_pBfd = NULL;
      }
    }

    bool IsInit()
    {
      return m_bInit;
    }

    void AddrToFileLine(const char* szAddrHex, bool bWriteFunction, std::string& sResult)
    {
      FindAddrParam stParam;

      stParam.tPc = bfd_scan_vma(szAddrHex, NULL, 16);
      stParam.ppSymbols = m_ppSymbols;
      stParam.bFound = false;

      if (m_pBfd)
      {
        bfd_map_over_sections(m_pBfd, FindAddrInSection, &stParam);
      }

      // FileName
      if (stParam.bFound && stParam.szFileName && *stParam.szFileName != '\0')
      {
        sResult.append(stParam.szFileName);
      }
      else
      {
        sResult.append("??");
      }

      sResult.append("[");

      // Line no
      if (stParam.bFound)
      {
        sResult.append(ToString(stParam.unLine)).append("]: ");
      }
      else
      {
        sResult.append("??]: ");
      }

      if (bWriteFunction)
      {
        if (stParam.bFound && stParam.szFunction && *stParam.szFunction != '\0')
        {
          int nStatus = 0;
          char* szDemangled = abi::__cxa_demangle(stParam.szFunction, 0, 0, &nStatus);
          if (szDemangled && szDemangled[0])
          {
            sResult.append(szDemangled);
          }
          else
          {
            sResult.append(stParam.szFunction);
          }

        }
        else
        {
          sResult.append("??");
        }
      }
    }

  private:
    static void FindAddrInSection(bfd* pBfd, asection* pSection, void* pData)
    {
      FindAddrParam* pParam = reinterpret_cast<FindAddrParam*>(pData);
      if (pParam && !pParam->bFound)
      {
        if ((bfd_get_section_flags(pBfd, pSection) & SEC_ALLOC) != 0)
        {
          bfd_vma tVma = bfd_get_section_vma(pBfd, pSection);
          if (pParam->tPc >= tVma)
          {
            bfd_size_type nSectionSize = bfd_get_section_size(pSection);
            if (pParam->tPc < tVma + nSectionSize)
            {
              pParam->bFound = bfd_find_nearest_line(pBfd, pSection, pParam->ppSymbols,
                                                     pParam->tPc - tVma, &pParam->szFileName,
                                                     &pParam->szFunction, &pParam->unLine);
            }
          }
        }
      }
    }

    asymbol** m_ppSymbols;
    bfd* m_pBfd;
    bool m_bInit;
  };


  class AddrToFileLine
  {
  public:
    static AddrToFileLine& Inst()
    {
      static AddrToFileLine tInst;
      return tInst;
    }

    void GetFileLine(const char* szFileName, const char* szAddr, bool bWriteFunction, std::string& sResult)
    {
      BfdReader& rBfdReader = m_mBfdFiles[szFileName];
      if (!rBfdReader.IsInit())
      {
        rBfdReader.Open(szFileName);
      }
      rBfdReader.AddrToFileLine(szAddr, bWriteFunction, sResult);
    }

  private:
    std::map<std::string, BfdReader> m_mBfdFiles;
  };


#endif

#ifdef WIN32
  class AddrInfoReader
  {
  public:
    static std::string LookupAddr(const void* pAddr)
    {
      std::string sResult;
      IMAGEHLP_SYMBOL* pSymbol = GetSymbol();

      HANDLE hProc = GetCurrentProcess();
#if defined _WIN64
      DWORD64 dwAddr = reinterpret_cast<DWORD64>(pAddr);
      DWORD64 symDisp1 = 0;
#elif defined WIN32
      DWORD dwAddr = reinterpret_cast<DWORD>(pAddr);
      DWORD symDisp1 = 0;
#else
#error unsupported arch
#endif
      DWORD symDisp2 = 0;
      char szAddrHex[11];
      IMAGEHLP_LINE stLine;
      IMAGEHLP_MODULE stModule;

      ToHexCString(pAddr, szAddrHex, sizeof(szAddrHex));
      sResult.append(szAddrHex).append(" ");

      // context
      memset(&stModule, 0, sizeof(stLine));
      stModule.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

      if (SymGetModuleInfo(hProc, dwAddr, &stModule))
      {
        sResult.append(stModule.ImageName);
      }
      else
      {
        sResult.append("??");
      }


      // file name and line
      memset(&stLine, 0, sizeof(stLine));
      stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);
      stLine.Address = dwAddr;

      if (SymGetLineFromAddr(hProc, dwAddr, &symDisp2, &stLine))
      {
        sResult.append(stLine.FileName).append("[").append(ToString(stLine.LineNumber)).append("]: ");
      }
      else
      {
        sResult.append("??[??]: ");
      }

      // function name
      if (SymGetSymFromAddr(hProc, dwAddr, &symDisp1, pSymbol))
      {
        sResult.append(pSymbol->Name);
      }
      else
      {
        sResult.append("??");
      }

      return sResult;
    }

  private:
#define sizeof_Name 128
#define sizeof_symbol sizeof(IMAGEHLP_SYMBOL)+sizeof_Name
    static IMAGEHLP_SYMBOL* GetSymbol()
    {
      if (!m_pSymbol)
      {
        m_pSymbol = reinterpret_cast<IMAGEHLP_SYMBOL*>(&m_tSymbolBuff);
        memset(m_pSymbol, 0, sizeof_symbol);
        m_pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
        m_pSymbol->MaxNameLength = sizeof_Name - 1;
      }

      return m_pSymbol;
    }

    static char m_tSymbolBuff[sizeof_symbol];
    static IMAGEHLP_SYMBOL* m_pSymbol;
  };

  char AddrInfoReader::m_tSymbolBuff[sizeof_symbol];
  IMAGEHLP_SYMBOL* AddrInfoReader::m_pSymbol = NULL;
#endif


  void StackTracer::GetStackTraceStr(std::string& sResult, unsigned nMaxDepth, unsigned nSkip)
  {
#if defined __linux__ && defined WITH_BFD
    Dl_info tDlInfo;
    void** pAddresses = new void*[nMaxDepth];
    unsigned nAddressesSize = backtrace(pAddresses, nMaxDepth);
    char** ppSymbols = backtrace_symbols(pAddresses, nAddressesSize);
    int nStatus = 0;
    char szAddress[19];
    char* szSymbol = NULL;
    char* szDemangled = NULL;
    char* szFunction = NULL;
    char* szEnd = NULL;
    const char* szContext = NULL;
    const void* pAddr = NULL;

    sResult.erase();

    for (unsigned nIndex = nSkip; nIndex < nAddressesSize; ++nIndex)
    {
      pAddr = pAddresses[nIndex];

      szSymbol = ppSymbols[nIndex];
      if (szSymbol)
      {
        szFunction = strchr(szSymbol, '(');
        if (szFunction)
        {
          *szFunction = '\0';
          ++szFunction;
          szEnd = strchr(szFunction, '+');
          if (!szEnd)
          {
            szEnd = strchr(szFunction, ')');
          }
          if (szEnd)
          {
            *szEnd = '\0';
          }

          szDemangled = abi::__cxa_demangle(szFunction, 0, 0, &nStatus);
          if (szDemangled && szDemangled[0])
          {
            szFunction = szDemangled;
          }
        }

        szContext = szSymbol;

        if (dladdr(pAddr, &tDlInfo))
        {
          if (tDlInfo.dli_sname) // shared library
          {
            szContext = reinterpret_cast<const char*>(tDlInfo.dli_fname);
            pAddr = reinterpret_cast<const void*>(reinterpret_cast<long>(pAddr) -
                                                  reinterpret_cast<long>(tDlInfo.dli_fbase));
          }
          else
          {
            static const std::string& sExecPath = Process::GetCurrentExecPath();
            szContext = sExecPath.c_str();
          }
        }

        if (!ToHexCString(pAddr, szAddress, sizeof(szAddress)))
        { // shouldn't happen
          continue;
        }


        bool bWriteFunction = !szFunction || !szFunction[0];

        // address
        sResult.append(szAddress);

        // binary file
        sResult.append(" ").append(szContext).append(" ");

        // source file and line
        AddrToFileLine::Inst().GetFileLine(szContext, szAddress, bWriteFunction, sResult);

        if (!bWriteFunction)
        {
          // source function name
          sResult.append(szFunction ? szFunction : "??");
        }
        sResult.append("\n");

        if (szDemangled)
        {
          free(szDemangled); // allocated with malloc/realloc
          szDemangled = NULL;
        }
      }
    }

    free(ppSymbols);
    delete[] pAddresses;
#else
#if defined _WIN64
#if defined __MINGW64__
#if !defined __in
#define __in
#endif
#if !defined __out
#define __out
#endif
#if !defined __out_opt
#define __out_opt
#endif
#endif
     typedef USHORT (WINAPI *PCaptureStackBackTrace)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
     PCaptureStackBackTrace pCaptureStackBackTrace =
       (PCaptureStackBackTrace)(GetProcAddress(LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace"));

    if (pCaptureStackBackTrace)
    {
      const int nMaxStackSize = 62;

      void* pStack[nMaxStackSize];
      int nCount = pCaptureStackBackTrace(0, nMaxStackSize, pStack, NULL);
      for (int i = 0; i < nCount; i++)
      {
        if (i >= nSkip)
        {
          sResult += AddrInfoReader::LookupAddr(reinterpret_cast<void*>(pStack[i]));
        }
      }
    }

#elif defined WIN32
    HANDLE hProc, hThread;
    CONTEXT* pContext = 0;
    STACKFRAME tStackFrame;
    DWORD machType = IMAGE_FILE_MACHINE_I386;
    DWORD dwFilepathLen = 0;
    char szFilePath[MAX_PATH];
    char* pszLastDir = NULL;
    char* pszPath = NULL;
    DWORD tebp = 0;
    DWORD tesp = 0;

#ifdef _MSC_VER
    _asm mov tesp, esp
    _asm mov tebp, ebp
#else
    asm("movl %%esp, %0":"=d"(tesp));
    asm("movl %%ebp, %0":"=d"(tebp));
#endif

    // Get image filename of the main executable
    dwFilepathLen = GetModuleFileNameA(NULL, szFilePath, sizeof(szFilePath));
    if (dwFilepathLen)
    {
      LogError() << "NtStackTrace: Failed to get pathname for program\n";
    }

    // Strip the filename, leaving the path to the executable
    pszLastDir = strrchr(szFilePath, '/');
    if (!pszLastDir)
    {
      pszLastDir = strrchr(szFilePath, '\\');
    }
    if (pszLastDir)
    {
      pszLastDir[0] = '\0';
    }

    // Initialize the symbol table routines, supplying a pointer to the path
    pszPath = szFilePath;
    if (strlen(szFilePath) == 0)
      pszPath = NULL;

    hProc = GetCurrentProcess ();
    hThread = GetCurrentThread ();
    if (!SymInitialize(hProc, pszPath, TRUE))
    {
      LogError() << "NtStackTrace: failed to initialize symbols\n";
    }

    // Allocate and initialize frame and symbol structures
    memset(&tStackFrame, 0, sizeof(STACKFRAME));
    tStackFrame.AddrPC.Mode = AddrModeFlat;
    tStackFrame.AddrStack.Mode = AddrModeFlat;
    tStackFrame.AddrFrame.Mode = AddrModeFlat;
    tStackFrame.AddrStack.Offset = tesp;
    tStackFrame.AddrFrame.Offset = tebp;
    tStackFrame.AddrPC.Offset = reinterpret_cast<DWORD>(&GetStackTraceStr) + 0x08c;

    while (StackWalk(machType, hProc, hThread, &tStackFrame, pContext, NULL,
                      SymFunctionTableAccess, SymGetModuleBase, NULL))
    {
      if (nSkip > 0)
      {
        --nSkip;
      }
      else
      {
        sResult += AddrInfoReader::LookupAddr(reinterpret_cast<void*>(tStackFrame.AddrPC.Offset));
      }
    }

    if (pContext)
    {
      free(pContext);
    }
#endif

    LogDebug3() << "done tracing stack(list)";
#endif
  }


}
