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

#include "BfdReader.h"

#if defined OS_Linux
#define _GNU_SOURCE 1

#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <bfd.h>
#include <limits.h>
#include <rise/common/Log.h>

#ifdef RISE_USE_DEMANGLE
extern "C" const char* cplus_demangle(const char* sym, int opts);
#endif

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

namespace rise
{
  namespace tools
  {
    typedef bfd* (*t_bfd_openr) (const char *filename, const char *target);
    typedef bfd_boolean (*t_bfd_close) (bfd *abfd);
    typedef bfd_boolean (*t_bfd_check_format_matches)(bfd *abfd, bfd_format format, char ***matching);
    typedef bfd_boolean (*t_bfd_check_format) (bfd *abfd, bfd_format format);
    typedef bfd_vma (*t_bfd_scan_vma) (const char *string, const char **end, int base);
    typedef void (*t_bfd_map_over_sections) (bfd *abfd, void (*func) (bfd *abfd, asection *sect, void *obj), void *obj);

    t_bfd_openr p_bfd_openr = NULL;
    t_bfd_close p_bfd_close = NULL;
    t_bfd_check_format_matches p_bfd_check_format_matches = NULL;
    t_bfd_check_format p_bfd_check_format = NULL;
    t_bfd_scan_vma p_bfd_scan_vma = NULL;
    t_bfd_map_over_sections p_bfd_map_over_sections = NULL;

#define RISE_LOAD_SYM(RISE_LOCAL_SYM)\
      {\
        /* avoiding the warning "ISO C++ forbids casting between pointer-to-function and pointer-to-object" */ \
        void* pSymbol = dlsym(m_hBfdLib, #RISE_LOCAL_SYM);\
        p_##RISE_LOCAL_SYM = *reinterpret_cast<t_##RISE_LOCAL_SYM*>(&pSymbol);\
        if (p_##RISE_LOCAL_SYM == NULL)  \
          LogError() << "can't load symbol: "  #RISE_LOCAL_SYM  ": " << dlerror();\
      }

    class CBfdLoader
    {
    public:
      static CBfdLoader& Inst()
      {
        if (m_pInst == NULL)
          m_pInst = new CBfdLoader;
        return *m_pInst;
      }

    protected:
      CBfdLoader()
      {
        m_hBfdLib = dlopen("/usr/lib/libbfd.so", RTLD_LAZY);
        if (!m_hBfdLib) 
        {
          LogError() << "can't load libfsd: " << dlerror();
          return;
        }

        LogDebug3() << "libfsd load ok.";

        RISE_LOAD_SYM(bfd_openr);
        RISE_LOAD_SYM(bfd_close);
        RISE_LOAD_SYM(bfd_check_format_matches);
        RISE_LOAD_SYM(bfd_check_format);
        RISE_LOAD_SYM(bfd_scan_vma);
        RISE_LOAD_SYM(bfd_map_over_sections);
      }

    private:
      void* m_hBfdLib;
      static CBfdLoader* m_pInst;
    };

    CBfdLoader* CBfdLoader::m_pInst = NULL;

    class CBfdReader::CBfdReaderImpl
    {
    public:
      struct SFindAddrParam
      {
        bfd_vma tPc;
        asymbol** ppSymbols;

        const char* szFileName;
        const char* szFunctionName;
        unsigned int unLine;
        bool bFound;

        SFindAddrParam():
          tPc(0), 
          ppSymbols(NULL), 
          szFileName(NULL),
          szFunctionName(NULL),
          unLine(0),
          bFound(false)
        {
        }
      };


      CBfdReaderImpl():
        m_ppSymbols(NULL),
        m_pBfd(NULL)
      {
        CBfdLoader::Inst();
      }

      void ReadSymbols()
      {
        long lSymCount;
        unsigned int unSize;

        if ((bfd_get_file_flags(m_pBfd) & HAS_SYMS) == 0)
          return;

        lSymCount = bfd_read_minisymbols(m_pBfd, FALSE, (void**)&m_ppSymbols, &unSize);
        if (lSymCount == 0)
          lSymCount = bfd_read_minisymbols(m_pBfd, TRUE, (void**)&m_ppSymbols, &unSize);
      }

      static void FindAddrInSection( bfd* pBfd, asection* pSection, void* pData )
      {
        SFindAddrParam* pParam = reinterpret_cast<SFindAddrParam*>(pData);
        if(pParam == NULL)
          return;

        if (pParam->bFound)
          return;

        if ((bfd_get_section_flags(pBfd, pSection) & SEC_ALLOC) == 0)
          return;

        bfd_vma tVma = bfd_get_section_vma(pBfd, pSection);
        if (pParam->tPc < tVma)
          return;

        bfd_size_type size = bfd_get_section_size(pSection);
        if (pParam->tPc >= tVma + size)
          return;

        pParam->bFound = bfd_find_nearest_line(pBfd, pSection, pParam->ppSymbols, pParam->tPc - tVma,
          &pParam->szFileName, &pParam->szFunctionName, &pParam->unLine);
      }

      asymbol** m_ppSymbols;
      bfd* m_pBfd;
      CString m_sFileName;
#ifdef RISE_USE_DEMANGLE
      static bool m_bExtractFunctionParams;
#endif
    };

#ifdef RISE_USE_DEMANGLE
    bool CBfdReader::CBfdReaderImpl::m_bExtractFunctionParams = getenv("RISE_DEMANGLE_PARAMS") != NULL;
#endif


    CBfdReader::CBfdReader()
    {
      m_pImpl = new CBfdReaderImpl;
    }

    CBfdReader::~CBfdReader()
    {
      if(m_pImpl != NULL)
        delete m_pImpl;

      m_pImpl = NULL;
    }

    bool CBfdReader::Open( const CString& sFileName )
    {
      char** pszMatching;

      Close();

      m_pImpl->m_pBfd = p_bfd_openr(sFileName.c_str(), NULL);
      if (m_pImpl->m_pBfd == NULL)
      {
        fprintf(stderr, "error opening file: \"%s\": ", sFileName.c_str());
        perror("");
        return false;
      }

      if (p_bfd_check_format(m_pImpl->m_pBfd, bfd_archive))
      {
        perror("p_bfd_check_format");
        return false;
      }

      if (!p_bfd_check_format_matches(m_pImpl->m_pBfd, bfd_object, &pszMatching))
      {
        perror("p_bfd_check_format_matches");
        return false;
      }

      m_pImpl->ReadSymbols();
      m_pImpl->m_sFileName = sFileName;

      return true;
    }

    void CBfdReader::Close()
    {
      if (m_pImpl->m_ppSymbols != NULL)
      {
        free(m_pImpl->m_ppSymbols);
        m_pImpl->m_ppSymbols = NULL;
      }

      if(m_pImpl->m_pBfd != NULL)
        p_bfd_close(m_pImpl->m_pBfd);
    }

    bool CBfdReader::IsOpen()
    {
      return m_pImpl->m_pBfd != NULL;
    }

    const rise::CString& CBfdReader::GetFileName()
    {
      return m_pImpl->m_sFileName;
    }

    bool CBfdReader::LookupAddrPtr( const void* pAddr, SAddrInfo& rAddrInfo )
    {
#ifdef _M_X64
      char szAddrHex[19];
      snprintf(szAddrHex, sizeof(szAddrHex), "0x%016lx", reinterpret_cast<const unsigned long>(pAddr));
#else
      char szAddrHex[11];
      snprintf(szAddrHex, sizeof(szAddrHex), "0x%08x", reinterpret_cast<const unsigned int>(pAddr));
#endif

      return LookupAddr(szAddrHex, rAddrInfo);
    }

    bool CBfdReader::LookupAddr( const CString& sAddrHex, SAddrInfo& rAddrInfo )
    {
      CBfdReaderImpl::SFindAddrParam stParam;

      stParam.tPc = p_bfd_scan_vma( sAddrHex.c_str(), NULL, 16 );
      stParam.ppSymbols = m_pImpl->m_ppSymbols;
      stParam.bFound = false;

      if(m_pImpl->m_pBfd)
      {
        p_bfd_map_over_sections(m_pImpl->m_pBfd, CBfdReaderImpl::FindAddrInSection, &stParam);
      }

      // szFileName
      if (stParam.bFound && stParam.szFileName != NULL && *stParam.szFileName != '\0')
        rAddrInfo.sFileName = stParam.szFileName;
      else
        rAddrInfo.sFileName = "??";

      // unLine no
      if (stParam.bFound)
        rAddrInfo.unLine = stParam.unLine;
      else
        rAddrInfo.unLine = 0;

      // function name
      if (stParam.bFound && stParam.szFunctionName != NULL && *stParam.szFunctionName != '\0')
      {
#ifdef RISE_USE_DEMANGLE
        const char* szDemangled = cplus_demangle(stParam.szFunctionName, CBfdReaderImpl::m_bExtractFunctionParams ? 1 : 0);
        rAddrInfo.sFunctionName = szDemangled != NULL ? szDemangled : stParam.szFunctionName;
#else
        rAddrInfo.sFunctionName = stParam.szFunctionName;
#endif
      }
      else
      {
        rAddrInfo.sFunctionName = "??";
      }

      rAddrInfo.sAddrHex = sAddrHex;

      return stParam.bFound;
    }

  }
}
#else
namespace rise
{
  namespace tools
  {
    CBfdReader::CBfdReader()
    {
    }

    CBfdReader::~CBfdReader()
    {
    }

    bool CBfdReader::Open( const CString& /*sFileName*/ )
    {
      return false;
    }

    void CBfdReader::Close()
    {
    }

    bool CBfdReader::IsOpen()
    {
      return false;
    }

    bool CBfdReader::LookupAddrPtr( const void* /*pAddr*/, SAddrInfo& /*rAddrInfo*/ )
    {
      return false;
    }

    bool CBfdReader::LookupAddr( const CString& /*sAddrHex*/, SAddrInfo& /*rAddrInfo*/ )
    {
      return false;
    }

    const rise::CString& CBfdReader::GetFileName()
    {
      static CString sFileName;
      return sFileName;
    }

  }
}
#endif
