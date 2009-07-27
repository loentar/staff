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

  RISE_EXPORT COStream& ColorDefault(COStream& rStream);            //! все атрибуты по умолчанию 

  RISE_EXPORT COStream& ColorBright(COStream& rStream);             //! яркий цвет
  RISE_EXPORT COStream& ColorDim(COStream& rStream);                //! полуяркий цвет
  RISE_EXPORT COStream& ColorBrightDefault(COStream& rStream);      //! яркость по умолчанию
  RISE_EXPORT COStream& ColorFlashOn(COStream& rStream);            //! включить мигание
  RISE_EXPORT COStream& ColorFlashOff(COStream& rStream);           //! отключить мигание
  RISE_EXPORT COStream& ColorInverseOn(COStream& rStream);          //! включить инверсию
  RISE_EXPORT COStream& ColorInverseOff(COStream& rStream);         //! отключить инверсию
  RISE_EXPORT COStream& ColorUnderlineOn(COStream& rStream);        //! включить подчеркивание
  RISE_EXPORT COStream& ColorUnderlineOff(COStream& rStream);       //! отключить подчеркивание
  
  //! Цвет знаков 
  RISE_EXPORT COStream& ColorInkBlack(COStream& rStream);           //! чёрный
  RISE_EXPORT COStream& ColorInkRed(COStream& rStream);             //! красный
  RISE_EXPORT COStream& ColorInkGreen(COStream& rStream);           //! зелёный
  RISE_EXPORT COStream& ColorInkBrown(COStream& rStream);           //! коричневый
  RISE_EXPORT COStream& ColorInkBlue(COStream& rStream);            //! синий
  RISE_EXPORT COStream& ColorInkMagenta(COStream& rStream);         //! фиолетовый
  RISE_EXPORT COStream& ColorInkCyan(COStream& rStream);            //! циановый 
  RISE_EXPORT COStream& ColorInkGrey(COStream& rStream);            //! серый
  
  //! Цвет фона
  RISE_EXPORT COStream& ColorPaperBlack(COStream& rStream);         //! чёрный
  RISE_EXPORT COStream& ColorPaperRed(COStream& rStream);           //! красный
  RISE_EXPORT COStream& ColorPaperGreen(COStream& rStream);         //! зелёный 
  RISE_EXPORT COStream& ColorPaperBrown(COStream& rStream);         //! коричневый
  RISE_EXPORT COStream& ColorPaperBlue(COStream& rStream);          //! синий
  RISE_EXPORT COStream& ColorPaperMagenta(COStream& rStream);       //! фиолетовый
  RISE_EXPORT COStream& ColorPaperCyan(COStream& rStream);          //! циановый
  RISE_EXPORT COStream& ColorPaperGrey(COStream& rStream);          //! серый

  //! управление позицией курсора
  RISE_EXPORT CManip1<short> CursorUp(short nValue);                //! передвинуть курсор вверх на n строк 
  RISE_EXPORT CManip1<short> CursorDown(short nValue);              //! передвинуть курсор вниз на n строк 
  RISE_EXPORT CManip1<short> CursorRight(short nValue);             //! передвинуть курсор вправо на n столбцов 
  RISE_EXPORT CManip1<short> CursorLeft(short nValue);              //! передвинуть курсор влево на n столбцов 
  RISE_EXPORT CManip1<short> CursorDownNl(short nValue);            //! передвинуть курсор вниз на n строк и поставить в начало строки 
  RISE_EXPORT CManip1<short> CursorUpNl(short nValue);              //! передвинуть курсор вверх на n строк и поставить в начало строки 
  RISE_EXPORT CManip1<short> CursorTab(short nValue);               //! переместить курсор в n-й столбец текущей строки 
  RISE_EXPORT CManip2<short> CursorGoto(short nX, short nY);        //! задает абсолютные координаты курсора (строка, столбец) 

  //! работа с палитрой
  /*  изменение палитры. 
     \param nPaletteIndex - номер цвета;
     \param nR nG nB - значения RGB-компонент
  */
  RISE_EXPORT CManip4<short> PaletteSet(short nPaletteIndex, short nR, short nG, short nB);
  
  // сброс палитры
  RISE_EXPORT COStream& PaletteReset(COStream& rStream);       //! восстановление палитры, принятую по умолчанию 
  
  // Работа с виртуальными консолями 
  RISE_EXPORT CManip1<short> ConsoleSwitchTo(short nValue);    //! сделать текущей консоль с указанным номером 
  
  // работа с xterm 
  RISE_EXPORT CManip1<const CString&> ConsoleSetWindowTextAndIcon(const CString& sText);   //! установка заголовка окна и иконки
  RISE_EXPORT CManip1<const CString&> ConsoleSetIcon(const CString& sText);                //! установка иконки
  RISE_EXPORT CManip1<const CString&> ConsoleSetWindowText(const CString& sText);          //! установка заголовка окна 

}  //! namespace rise

#include "console.hpp"

#endif //! #ifndef _CONSOLE_H_
