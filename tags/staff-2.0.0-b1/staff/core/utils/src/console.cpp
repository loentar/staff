/*
 *  Copyright 2012 Utkin Dmitry
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

#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "console.h"

#ifdef __MINGW32__
#ifndef COMMON_LVB_REVERSE_VIDEO
#define COMMON_LVB_REVERSE_VIDEO   0x4000
#endif
#ifndef COMMON_LVB_UNDERSCORE
#define COMMON_LVB_UNDERSCORE      0x8000
#endif
#endif

namespace staff
{
#ifdef WIN32
  class ConsAttr
  {
  private:
    HANDLE m_hStd;
  
  public:
    ConsAttr():
      m_hStd(GetStdHandle(STD_OUTPUT_HANDLE))
    {
    }

    ~ConsAttr()
    {
      CloseHandle(m_hStd);
    }

    std::ostream& SetAttribute(std::ostream& rStream, WORD nAttr)
    {
      if (rStream != std::cout)
      {
        return rStream;
      }
      rStream << std::flush;
      SetConsoleTextAttribute(m_hStd, nAttr);
      return rStream;
    }

    WORD GetAttribute()
    {
      CONSOLE_SCREEN_BUFFER_INFO tConsoleScreenBufferInfo;
      if(GetConsoleScreenBufferInfo(m_hStd, &tConsoleScreenBufferInfo))
      {
        return tConsoleScreenBufferInfo.wAttributes;
      }
      else
      {
        return 0xffffu;
      }
    }

  };
  ConsAttr g_tConsAttr;
#endif

  std::ostream& ColorDefault(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return rStream << "\033[0m";
#endif
  }

  std::ostream& ColorBright(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, FOREGROUND_INTENSITY | g_tConsAttr.GetAttribute());
#else
    return rStream << "\033[1m";
#endif
  }

  std::ostream& ColorDim(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY));
#else
    return rStream << "\033[2m";
#endif
  }

  std::ostream& ColorBrightDefault(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY));
#else
    return rStream << "\033[22m";
#endif
  }

  std::ostream& ColorFlashOn(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, BACKGROUND_INTENSITY | g_tConsAttr.GetAttribute());
#else
    return rStream << "\033[5m";
#endif
  }

  std::ostream& ColorFlashOff(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ BACKGROUND_INTENSITY);
#else
    return rStream << "\033[25m";
#endif
  }

  std::ostream& ColorInverseOn(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() | COMMON_LVB_REVERSE_VIDEO);
#else
    return rStream << "\033[7m";
#endif
  }

  std::ostream& ColorInverseOff(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ COMMON_LVB_REVERSE_VIDEO);
#else
    return rStream << "\033[27m";
#endif
  }

  std::ostream& ColorUnderlineOn(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() | COMMON_LVB_UNDERSCORE);
#else
    return rStream << "\033[4m";
#endif
  }

  std::ostream& ColorUnderlineOff(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ COMMON_LVB_UNDERSCORE);
#else
    return rStream << "\033[24m";
#endif
  }

  std::ostream& ColorTextBlack(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE));
#else
    return rStream << "\033[30m";
#endif
  }

  std::ostream& ColorTextRed(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (FOREGROUND_GREEN | FOREGROUND_BLUE)) | FOREGROUND_RED);
#else
    return rStream << "\033[31m";
#endif
  }

  std::ostream& ColorTextGreen(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_BLUE)) | FOREGROUND_GREEN);
#else
    return rStream << "\033[32m";
#endif
  }

  std::ostream& ColorTextBrown(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ FOREGROUND_BLUE) | FOREGROUND_RED | FOREGROUND_GREEN);
#else
    return rStream << "\033[33m";
#endif
  }

  std::ostream& ColorTextBlue(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN)) | FOREGROUND_BLUE);
#else
    return rStream << "\033[34m";
#endif
  }

  std::ostream& ColorTextMagenta(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ FOREGROUND_GREEN) | FOREGROUND_RED | FOREGROUND_BLUE);
#else
    return rStream << "\033[35m";
#endif
  }

  std::ostream& ColorTextCyan(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ FOREGROUND_RED) | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return rStream << "\033[36m";
#endif
  }

  std::ostream& ColorTextGrey(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return rStream << "\033[37m";
#endif
  }


  std::ostream& ColorBackgroundBlack(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE));
#else
    return rStream << "\033[40m";
#endif
  }

  std::ostream& ColorBackgroundRed(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (BACKGROUND_GREEN | BACKGROUND_BLUE)) | BACKGROUND_RED);
#else
    return rStream << "\033[41m";
#endif
  }

  std::ostream& ColorBackgroundGreen(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_BLUE)) | BACKGROUND_GREEN);
#else
    return rStream << "\033[42m";
#endif
  }

  std::ostream& ColorBackgroundBrown(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ BACKGROUND_BLUE) | BACKGROUND_RED | BACKGROUND_GREEN);
#else
    return rStream << "\033[43m";
#endif
  }

  std::ostream& ColorBackgroundBlue(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN)) | BACKGROUND_BLUE);
#else
    return rStream << "\033[44m";
#endif
  }

  std::ostream& ColorBackgroundMagenta(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ BACKGROUND_GREEN) | BACKGROUND_RED | BACKGROUND_BLUE);
#else
    return rStream << "\033[45m";
#endif
  }

  std::ostream& ColorBackgroundCyan(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, (g_tConsAttr.GetAttribute() & ~ BACKGROUND_RED) | BACKGROUND_GREEN | BACKGROUND_BLUE);
#else
    return rStream << "\033[46m";
#endif
  }

  std::ostream& ColorBackgroundGrey(std::ostream& rStream)
  {
#ifdef WIN32
    return g_tConsAttr.SetAttribute(rStream, g_tConsAttr.GetAttribute() | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
#else
    return rStream << "\033[47m";
#endif
  }

} // namespace staff
