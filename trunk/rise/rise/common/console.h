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

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>

namespace rise
{
  template<class TDATA> struct CManip1;
  template<class TDATA> struct CManip2;
  template<class TDATA> struct CManip4;

  template<class TDATA> COStream& operator<<(COStream& rStream, const CManip1<TDATA>& cManipValue);
  template<class TDATA> COStream& operator<<(COStream& rStream, const CManip2<TDATA>& cManipValue);
  template<class TDATA> COStream& operator<<(COStream& rStream, const CManip4<TDATA>& cManipValue);

  //! use default color
  RISE_EXPORT COStream& ColorDefault(COStream& rStream);

  //! bright color
  RISE_EXPORT COStream& ColorBright(COStream& rStream);
  //! dim color
  RISE_EXPORT COStream& ColorDim(COStream& rStream);
  //! default bright
  RISE_EXPORT COStream& ColorBrightDefault(COStream& rStream);
  //! enable flashing
  RISE_EXPORT COStream& ColorFlashOn(COStream& rStream);
  //! disable flashing
  RISE_EXPORT COStream& ColorFlashOff(COStream& rStream);
  //! enable inversion
  RISE_EXPORT COStream& ColorInverseOn(COStream& rStream);
  //! disable inversion
  RISE_EXPORT COStream& ColorInverseOff(COStream& rStream);
  //! enable underlining
  RISE_EXPORT COStream& ColorUnderlineOn(COStream& rStream);
  //! disable underlining
  RISE_EXPORT COStream& ColorUnderlineOff(COStream& rStream);
  
  // text color
  //! black text
  RISE_EXPORT COStream& ColorInkBlack(COStream& rStream);
  //! red text
  RISE_EXPORT COStream& ColorInkRed(COStream& rStream);
  //! green text
  RISE_EXPORT COStream& ColorInkGreen(COStream& rStream);
  //! brown text
  RISE_EXPORT COStream& ColorInkBrown(COStream& rStream);
  //! blue text
  RISE_EXPORT COStream& ColorInkBlue(COStream& rStream);
  //! magenta text
  RISE_EXPORT COStream& ColorInkMagenta(COStream& rStream);
  //! cyan text
  RISE_EXPORT COStream& ColorInkCyan(COStream& rStream);
  //! gray text
  RISE_EXPORT COStream& ColorInkGrey(COStream& rStream);
  
  // background color
  //! black background
  RISE_EXPORT COStream& ColorPaperBlack(COStream& rStream);
  //! red background
  RISE_EXPORT COStream& ColorPaperRed(COStream& rStream);
  //! green background
  RISE_EXPORT COStream& ColorPaperGreen(COStream& rStream);
  //! brown background
  RISE_EXPORT COStream& ColorPaperBrown(COStream& rStream);
  //! blue background
  RISE_EXPORT COStream& ColorPaperBlue(COStream& rStream);
  //! magenta background
  RISE_EXPORT COStream& ColorPaperMagenta(COStream& rStream);
  //! cyan background
  RISE_EXPORT COStream& ColorPaperCyan(COStream& rStream);
  //! gray background
  RISE_EXPORT COStream& ColorPaperGrey(COStream& rStream);

  // cursor position management manipulators
  //! move cursor up to n lines
  RISE_EXPORT CManip1<short> CursorUp(short nValue);
  //! move cursor down to n lines
  RISE_EXPORT CManip1<short> CursorDown(short nValue);
  //! move cursor right to n columns
  RISE_EXPORT CManip1<short> CursorRight(short nValue);
  //! move cursor left to n columns
  RISE_EXPORT CManip1<short> CursorLeft(short nValue);
  //! move cursor down to n lines, and move to line begin
  RISE_EXPORT CManip1<short> CursorDownNl(short nValue);
  //! move cursor up to n lines, and move to line begin
  RISE_EXPORT CManip1<short> CursorUpNl(short nValue);
  //! move cursor to n'th column of current line
  RISE_EXPORT CManip1<short> CursorTab(short nValue);
  //! set absolute cursor position
  RISE_EXPORT CManip2<short> CursorGoto(short nX, short nY);

  //! change palette manipulator
  /*! \param nPaletteIndex - color index
	    \param nR - red
	    \param nG - green
      \param nB - blue
  */
  RISE_EXPORT CManip4<short> PaletteSet(short nPaletteIndex, short nR, short nG, short nB);
  
  //! reset palette manipulator
  RISE_EXPORT COStream& PaletteReset(COStream& rStream);
  
  //! switch to virtual console manipulator
  RISE_EXPORT CManip1<short> ConsoleSwitchTo(short nValue);
  
  //! set console window caption and icon
  /*! \param sText - window caption and icon name
  */
  RISE_EXPORT CManip1<const CString&> ConsoleSetWindowTextAndIcon(const CString& sText);

  //! set console window icon
  /*! \param sText - window icon name
  */
  RISE_EXPORT CManip1<const CString&> ConsoleSetIcon(const CString& sText);
  
  //! set console window caption
  /*! \param sText - window caption
  */
RISE_EXPORT CManip1<const CString&> ConsoleSetWindowText(const CString& sText);

}  // namespace rise

#include "console.hpp"

#endif // #ifndef _CONSOLE_H_
