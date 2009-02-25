#include <stdio.h>
#include <iomanip>
#include <iostream>
#include "console.h"

namespace rise
{
#ifdef WIN32
  class CConsAttr
  {
  private:
    HANDLE m_hStd;
  
  public:
    CConsAttr()
    {
      m_hStd = GetStdHandle(STD_OUTPUT_HANDLE);
    }

    ~CConsAttr()
    {
      CloseHandle(m_hStd);
    }

    void SetTitle(const CString& sTitle)
    {
      SetConsoleTitle(sTitle.c_str());
    }

    COStream& SetAttribute(COStream& rStream, WORD nAttr)
    {
      if (rStream != tStdOut) 
        return rStream;
      rStream << std::flush;
      SetConsoleTextAttribute(m_hStd, nAttr);
      return rStream;
    }

    WORD GetAttribute()
    {
      CONSOLE_SCREEN_BUFFER_INFO tConsoleScreenBufferInfo;
      if(GetConsoleScreenBufferInfo(m_hStd, &tConsoleScreenBufferInfo))
        return tConsoleScreenBufferInfo.wAttributes;
      else
        return 0xffffu;
    }

    void SetCursorPos(short shX, short shY)
    {
      COORD stCoord = {shX, shY};
      SetConsoleCursorPosition(m_hStd, stCoord);
    }

    bool GetCursorPos(short& shX, short& shY)
    {
      CONSOLE_SCREEN_BUFFER_INFO tConsoleScreenBufferInfo;
      if(!GetConsoleScreenBufferInfo(m_hStd, &tConsoleScreenBufferInfo))
        return false;

      shX = tConsoleScreenBufferInfo.dwCursorPosition.X;
      shY = tConsoleScreenBufferInfo.dwCursorPosition.X;
      return true;
    }

    bool MoveCursor(short shDeltaX, short shDeltaY)
    {
      short shCurrX = 0;
      short shCurrY = 0;
      if(!GetCursorPos(shCurrX, shCurrY))
        return false;

      SetCursorPos(shCurrX + shDeltaX, shCurrY + shDeltaY);
      return true;
    }

  } tConsAttr;
#endif

  COStream& ColorDefault(COStream& rStream)            // все атрибуты по умолчанию 
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return (rStream << "\033[0m");
#endif
  }

  COStream& ColorBright(COStream& rStream)             // яркий цвет
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, FOREGROUND_INTENSITY | tConsAttr.GetAttribute());
#else
    return (rStream << "\033[1m");
#endif
  }

  COStream& ColorDim(COStream& rStream)                // полуяркий цвет
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY) );
#else
    return (rStream << "\033[2m");
#endif
  }

  COStream& ColorBrightDefault(COStream& rStream)      // яркость по умолчанию
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY) );
#else
    return (rStream << "\033[22m");
#endif
  }

  COStream& ColorFlashOn(COStream& rStream)            // включить мигание
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, BACKGROUND_INTENSITY | tConsAttr.GetAttribute());
#else
    return (rStream << "\033[5m");
#endif
  }

  COStream& ColorFlashOff(COStream& rStream)           // отключить мигание
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ BACKGROUND_INTENSITY );
#else
    return (rStream << "\033[25m");
#endif
  }

  COStream& ColorInverseOn(COStream& rStream)          // включить инверсию
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() | COMMON_LVB_REVERSE_VIDEO);
#else
    return (rStream << "\033[7m");
#endif
  }

  COStream& ColorInverseOff(COStream& rStream)         // отключить инверсию
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ COMMON_LVB_REVERSE_VIDEO);
#else
    return (rStream << "\033[27m");
#endif
  }

  COStream& ColorUnderlineOn(COStream& rStream)       // включить подчеркивание
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() | COMMON_LVB_UNDERSCORE);
#else
    return (rStream << "\033[4m");
#endif
  }

  COStream& ColorUnderlineOff(COStream& rStream)      // отключить подчеркивание
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ COMMON_LVB_UNDERSCORE);
#else
    return (rStream << "\033[24m");
#endif
  }

  // Цвет знаков 
  COStream& ColorInkBlack(COStream& rStream)           // чёрный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE));
#else
    return (rStream << "\033[30m");
#endif
  }

  COStream& ColorInkRed(COStream& rStream)             // красный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_GREEN | FOREGROUND_BLUE) | FOREGROUND_RED);
#else
    return (rStream << "\033[31m");
#endif
  }

  COStream& ColorInkGreen(COStream& rStream)           // зелёный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_BLUE) | FOREGROUND_GREEN);
#else
    return (rStream << "\033[32m");
#endif
  }

  COStream& ColorInkBrown(COStream& rStream)           // коричневый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
#else
    return (rStream << "\033[33m");
#endif
  }

  COStream& ColorInkBlue(COStream& rStream)            // синий
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN) | FOREGROUND_BLUE);
#else
    return (rStream << "\033[34m");
#endif
  }

  COStream& ColorInkMagenta(COStream& rStream)         // фиолетовый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE);
#else
    return (rStream << "\033[35m");
#endif
  }

  COStream& ColorInkCyan(COStream& rStream)            // циановый 
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return (rStream << "\033[36m");
#endif
  }

  COStream& ColorInkGrey(COStream& rStream)            // серый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    return (rStream << "\033[37m");
#endif
  }


  // Цвет фона
  COStream& ColorPaperBlack(COStream& rStream)           // чёрный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE));
#else
    return (rStream << "\033[40m");
#endif
  }

  COStream& ColorPaperRed(COStream& rStream)             // красный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (BACKGROUND_GREEN | BACKGROUND_BLUE) | BACKGROUND_RED);
#else
    return (rStream << "\033[41m");
#endif
  }

  COStream& ColorPaperGreen(COStream& rStream)           // зелёный
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_BLUE) | BACKGROUND_GREEN);
#else
    return (rStream << "\033[42m");
#endif
  }

  COStream& ColorPaperBrown(COStream& rStream)           // коричневый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN);
#else
    return (rStream << "\033[43m");
#endif
  }

  COStream& ColorPaperBlue(COStream& rStream)            // синий
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN) | BACKGROUND_BLUE);
#else
    return (rStream << "\033[44m");
#endif
  }

  COStream& ColorPaperMagenta(COStream& rStream)         // фиолетовый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_BLUE);
#else
    return (rStream << "\033[45m");
#endif
  }

  COStream& ColorPaperCyan(COStream& rStream)            // циановый 
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() & ~ BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
#else
    return (rStream << "\033[46m");
#endif
  }

  COStream& ColorPaperGrey(COStream& rStream)            // серый
  {
#ifdef WIN32
    return tConsAttr.SetAttribute(rStream, tConsAttr.GetAttribute() | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
#else
    return (rStream << "\033[47m");
#endif
  }

  // управление позицией курсора
  COStream& _CursorUp( COStream& rStream, short nValue )
  {
#ifdef WIN32
    tConsAttr.MoveCursor(0, -nValue);
    return rStream;
#else
    return (rStream << "\033[" << nValue << "A");
#endif
  }

  // управление позицией курсора
  CManip1<short> CursorUp( short nValue )
  {
    return CManip1<short>(_CursorUp, nValue);
  }

  COStream& _CursorDown( COStream& rStream, short nValue )
  {
#ifdef WIN32
    tConsAttr.MoveCursor(0, nValue);
    return rStream;
#else
    return (rStream << "\033[" << nValue << "B");
#endif
  }

  CManip1<short> CursorDown(short nValue)     // передвинуть курсор вниз на n строк 
  {
    return CManip1<short>(_CursorDown, nValue);
  }

  COStream& _CursorRight( COStream& rStream, short nValue )
  {
#ifdef WIN32
    tConsAttr.MoveCursor(0, nValue);
    return rStream;
#else
    return (rStream << "\033[" << nValue << "C");
#endif
  }

  CManip1<short> CursorRight( short nValue )
{
    return CManip1<short>(_CursorRight, nValue);
  }

  COStream& _CursorLeft( COStream& rStream, short nValue )
  {
#ifdef WIN32
    tConsAttr.MoveCursor(0, -nValue);
    return rStream;
#else
    return (rStream << "\033[" << nValue << "D");
#endif
  }

  CManip1<short> CursorLeft( short nValue )
{
    return CManip1<short>(_CursorLeft, nValue);
  }

  COStream& _CursorDownNl( COStream& rStream, short nValue )
  {
#ifdef WIN32
    short shCurrX = 0;
    short shCurrY = 0;
    if(tConsAttr.GetCursorPos(shCurrX, shCurrY))
      tConsAttr.SetCursorPos(0, shCurrY + static_cast<short>(nValue));

    return rStream;
#else
    return (rStream << "\033[" << nValue << "E");
#endif
  }

  CManip1<short> CursorDownNl( short nValue )
{
    return CManip1<short>(_CursorDownNl, nValue);
  }

  COStream& _CursorUpNl( COStream& rStream, short nValue )
  {
#ifdef WIN32
    short shCurrX = 0;
    short shCurrY = 0;
    if(tConsAttr.GetCursorPos(shCurrX, shCurrY))
      tConsAttr.SetCursorPos(0, shCurrY - nValue);

    return rStream;
#else
    return (rStream << "\033[" << nValue << "F");
#endif
  }

  CManip1<short> CursorUpNl( short nValue )
{
    return CManip1<short>(_CursorUpNl, nValue);
  }

  COStream& _CursorTab( COStream& rStream, short nValue )
  {
#ifdef WIN32
    short shCurrX = 0;
    short shCurrY = 0;
    if(tConsAttr.GetCursorPos(shCurrX, shCurrY))
      tConsAttr.SetCursorPos(nValue, shCurrY);

    return rStream;
#else
    return (rStream << "\033[" << nValue << "G");
#endif
  }

  CManip1<short> CursorTab( short nValue )
{
    return CManip1<short>(_CursorTab, nValue);
  }

  COStream& _CursorGoto( COStream& rStream, short shX, short shY )
  {
#ifdef WIN32
    tConsAttr.SetCursorPos(shX, shY);
    return rStream;
#else
    return (rStream << "\033[" << shX << "," << shY << "H");
#endif
  }

  CManip2<short> CursorGoto(short shX, short shY) // задает абсолютные координаты курсора (строка, столбец) 
  {
    return CManip2<short>(_CursorGoto, shX, shY);
  }

  // работа с палитрой
  // изменение палитры. nPaletteIndex - номер цвета nR, nG и nB -- значения RGB-компонент
  COStream& _PaletteSet(COStream& rStream, short nPaletteIndex, short nR, short nG, short nB)
  {
#ifdef WIN32
    nPaletteIndex; nR; nG; nB;
    return rStream;
#else
    return (rStream << "\033]P" << std::setw(2) << std::setfill('0') 
        << std::hex << nPaletteIndex << nR << nG << nB);
#endif
  }

  CManip4<short> PaletteSet(short nPaletteIndex, short nR, short nG, short nB)
  {
    return CManip4<short>(_PaletteSet, nPaletteIndex, nR, nG, nB);
  }

  COStream& PaletteReset(COStream& rStream)                   // восстановление палитры, принятую по умолчанию 
  {
#ifdef WIN32
    return rStream;
#else
    return (rStream << "\033]R");                   // восстановление палитры, принятую по умолчанию 
#endif
  }

  // Работа с виртуальными консолями 
  COStream& _ConsoleSwitchTo(COStream& rStream, short nValue)    // сделать текущей консоль с указанным номером 
  {
#ifdef WIN32
    nValue;
    return rStream;
#else
    return (rStream << "\033[12;" << nValue << "]");
#endif
  }

  CManip1<short> ConsoleSwitchTo(short nValue)    // сделать текущей консоль с указанным номером 
  {
    return CManip1<short>(_ConsoleSwitchTo, nValue);
  }

  COStream& _Console(COStream& rStream, CString nValue)    // сделать текущей консоль с указанным номером 
  {
#ifdef WIN32
    return rStream;
#else
    return (rStream << "\033[12;" << nValue << "]");
#endif
  }

  CManip1<CString> Console(const CString& nValue)    // сделать текущей консоль с указанным номером 
  {
    return CManip1<CString>(_Console, nValue);
  }


  // работа с xterm 
  COStream& _ConsoleSetWindowTextAndIcon(COStream& rStream, const CString& sText) // установка заголовка окна и иконки
  {
#ifdef WIN32
    tConsAttr.SetTitle(sText);
    return rStream;
#else
    return (rStream << "\033]0;" + sText + "\007");
#endif
  }

  RISE_EXPORT CManip1<const CString&> ConsoleSetWindowTextAndIcon( const CString& sText )
  {
    return CManip1<const CString&>(_ConsoleSetWindowTextAndIcon, sText);
  }

  COStream& _ConsoleSetIcon(COStream& rStream, const CString& sText) // установка иконки
  {
#ifdef WIN32
    sText;
    return rStream;
#else
    return (rStream << "\033]1;" + sText + "\007");
#endif
  }

  CManip1<const CString&> ConsoleSetIcon(const CString& sText)                // установка иконки
  {
    return CManip1<const CString&>(_ConsoleSetIcon, sText);
  }

  COStream& _ConsoleSetWindowText(COStream& rStream, const CString& sText)          // установка заголовка окна 
  {
#ifdef WIN32
    tConsAttr.SetTitle(sText);
    return rStream;
#else
    return (rStream << "\033]2;" + sText + "\007");
#endif
  }

  CManip1<const CString&> ConsoleSetWindowText(const CString& sText)          // установка заголовка окна 
  {
    return CManip1<const CString&>(_ConsoleSetWindowText, sText);
  }

} // namespace rise
