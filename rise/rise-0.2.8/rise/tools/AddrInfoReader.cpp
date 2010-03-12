
#ifdef __linux__
#define _GNU_SOURCE 1
#include <dlfcn.h>
#include <map>
#include <memory>
#include <iostream>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/common/MutablePtr.h>
#include <rise/process/Process.h>
#include <rise/tools/BfdReader.h>
#include <rise/os/oserror.h>
#include "AddrInfoReader.h"

namespace rise
{
  namespace tools
  {
    class CAddrInfoReader::CAddrInfoReaderImpl
    {
    private:
      typedef CMutablePtr<CBfdReader> TPAutoBfdReader;
      typedef std::map<CString, TPAutoBfdReader> TBfdMap;
      
    public:
      bool LookupAddr(const void* pAddr, SAddrInfo& rAddrInfo)
      {
        Dl_info tInfo;
        if(dladdr(pAddr, &tInfo) != 0)
        {
        // library
          rAddrInfo.sContext = tInfo.dli_fname;
          pAddr = reinterpret_cast<const void*>(reinterpret_cast<long>(pAddr) - reinterpret_cast<long>(tInfo.dli_fbase));
        }
        else
        {
        // executable
          rAddrInfo.sContext = m_sExeName;
        }
        
        TPAutoBfdReader& rpReader = m_tBfdMap[rAddrInfo.sContext];
        if(rpReader.Get() == NULL)
        {
          rpReader = new CBfdReader;
          if(!rpReader->Open(rAddrInfo.sContext))
          {
            return false;
          }
        }
        
        return rpReader->LookupAddrPtr(pAddr, rAddrInfo);
      }
      
      CAddrInfoReaderImpl():
        m_sExeName(rise::process::CProcess::GetCurrentExecPath())
      {
      }

    private:
      CString m_sExeName;
      TBfdMap m_tBfdMap;
    };
  }
}
#else
#ifdef WIN32
#include <windows.h>
#include <imagehlp.h>
#include <stdio.h>
#include <rise/os/oserror.h>
#include "AddrInfoReader.h"

namespace rise
{
  namespace tools
  {
    class CAddrInfoReader::CAddrInfoReaderImpl
    {
    public:
      bool LookupAddr(const void* pAddr, SAddrInfo& rAddrInfo)
      {
        IMAGEHLP_SYMBOL* pSymbol = GetSymbol();

        HANDLE hProc = GetCurrentProcess();
        DWORD dwAddr = reinterpret_cast<DWORD>(pAddr);
        DWORD symDisp = 0;
        char szAddrHex[11];
        IMAGEHLP_LINE stLine;
        IMAGEHLP_MODULE stModule;

        // Decode the closest routine symbol name
        if ( SymGetSymFromAddr( hProc, dwAddr, &symDisp, pSymbol ) ) 
          rAddrInfo.sFunctionName = pSymbol->Name;
        else
          rAddrInfo.sFunctionName = "??";

        memset(&stLine, 0, sizeof(stLine));
        stLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);
        stLine.Address = dwAddr;

        symDisp = 0;
        if(SymGetLineFromAddr(hProc, dwAddr, &symDisp, &stLine))
        {
          rAddrInfo.unLine = stLine.LineNumber;
          rAddrInfo.sFileName = stLine.FileName;
        }
        else
        {
          rAddrInfo.unLine = 0;
          rAddrInfo.sFileName = "??";
        }

        memset(&stModule, 0, sizeof(stLine));
        stModule.SizeOfStruct = sizeof(IMAGEHLP_MODULE);

        if(SymGetModuleInfo(hProc, dwAddr, &stModule))
          rAddrInfo.sContext = stModule.ImageName;
        else
          rAddrInfo.sContext = "??";

        _snprintf(szAddrHex, sizeof(szAddrHex), "0x%08x", pAddr);
        rAddrInfo.sAddrHex = szAddrHex;

        return true;
      }

    private:
#define sizeof_Name 128
#define sizeof_symbol sizeof(IMAGEHLP_SYMBOL)+sizeof_Name
      static IMAGEHLP_SYMBOL* GetSymbol()
      {
        if (m_pSymbol == NULL)
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

    char CAddrInfoReader::CAddrInfoReaderImpl::m_tSymbolBuff[sizeof_symbol];
    IMAGEHLP_SYMBOL* CAddrInfoReader::CAddrInfoReaderImpl::m_pSymbol = NULL;
  }
}
#endif
#endif

namespace rise
{
  namespace tools
  {
    CAddrInfoReader::CAddrInfoReaderImpl& CAddrInfoReader::Impl()
    {
      if(m_pImpl == NULL)
        m_pImpl = new CAddrInfoReaderImpl;

      return *m_pImpl;
    }

    bool CAddrInfoReader::LookupAddr(const void* pAddr, SAddrInfo& rAddrInfo)
    {
      return Impl().LookupAddr(pAddr, rAddrInfo);
    }

    CAddrInfoReader::CAddrInfoReaderImpl* CAddrInfoReader::m_pImpl = NULL;
  }
}
