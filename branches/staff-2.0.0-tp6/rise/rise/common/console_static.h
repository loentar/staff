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

#ifndef _CONSOLE_STATIC_H_
#define _CONSOLE_STATIC_H_

#ifdef WIN32

////////////////////////////////////////////////////////////////////
// WINDOWS does'nt support static coloring

// basic attrs
#define COLOR_DEFAULT        ""

#define COLOR_BRIGHTNESS_ON  ""
#define COLOR_DIM_ON         ""
#define COLOR_UNDERLINE_ON   ""
#define COLOR_FLASH_ON       ""
#define COLOR_INVERSE_ON     ""

#define COLOR_BRIGHTNESS_OFF ""
#define COLOR_UNDERLINE_OFF  ""
#define COLOR_FLASH_OFF      ""
#define COLOR_INVERSE_OFF    ""

// text color
#define COLOR_INK_BLACK      ""
#define COLOR_INK_RED        ""
#define COLOR_INK_GREEN      ""
#define COLOR_INK_BROWN      ""
#define COLOR_INK_BLUE       ""
#define COLOR_INK_MAGENTA    ""
#define COLOR_INK_CYAN       ""
#define COLOR_INK_GREY       ""

// background color
#define COLOR_PAPER_BLACK    ""
#define COLOR_PAPER_RED      ""
#define COLOR_PAPER_GREEN    ""
#define COLOR_PAPER_BROWN    ""
#define COLOR_PAPER_BLUE     ""
#define COLOR_PAPER_MAGENTA  ""
#define COLOR_PAPER_CYAN     ""
#define COLOR_PAPER_GREY     ""


// cursor position management
#define CURSOR_UP(n)         ""
#define CURSOR_DOWN(n)       ""
#define CURSOR_RIGHT(n)      ""
#define CURSOR_LEFT(n)       ""
#define CURSOR_DOWN_NL(n)    ""
#define CURSOR_UP_NL(n)      ""
#define CURSOR_TAB(n)        ""
#define CURSOR_GOTO(x,y)     ""


// palette
  
// set palette. n -- color number; r, g & b -- RGB-components in hex
#define PALETTE_SET(n,r,g,b) ""
#define PALETTE_RESET        ""

// console
#define CONSOLE_SWITCHTO(n)  ""

// xterm
#define CONSOLE_ICONTEXT(s) ""
#define CONSOLE_ICON(s) ""
#define CONSOLE_TEXT(s) ""

#else  
//////////////////////////////////////////////////////////////////////////
//  LINUX

// basic attrs
#define COLOR_DEFAULT        "\033[0m"

#define COLOR_BRIGHTNESS_ON  "\033[1m"
#define COLOR_DIM_ON         "\033[2m"
#define COLOR_UNDERLINE_ON   "\033[4m"
#define COLOR_FLASH_ON       "\033[5m"
#define COLOR_INVERSE_ON     "\033[7m"

#define COLOR_BRIGHTNESS_OFF "\033[22m"
#define COLOR_UNDERLINE_OFF  "\033[24m" 
#define COLOR_FLASH_OFF      "\033[25m"
#define COLOR_INVERSE_OFF    "\033[27m"

// text color
#define COLOR_INK_BLACK      "\033[30m"
#define COLOR_INK_RED        "\033[31m"
#define COLOR_INK_GREEN      "\033[32m"
#define COLOR_INK_BROWN      "\033[33m"
#define COLOR_INK_BLUE       "\033[34m"
#define COLOR_INK_MAGENTA    "\033[35m"
#define COLOR_INK_CYAN       "\033[36m"
#define COLOR_INK_GREY       "\033[37m"

// background color
#define COLOR_PAPER_BLACK    "\033[40m"
#define COLOR_PAPER_RED      "\033[41m"
#define COLOR_PAPER_GREEN    "\033[42m"
#define COLOR_PAPER_BROWN    "\033[43m"
#define COLOR_PAPER_BLUE     "\033[44m"
#define COLOR_PAPER_MAGENTA  "\033[45m"
#define COLOR_PAPER_CYAN     "\033[46m"
#define COLOR_PAPER_GREY     "\033[47m"


// cursor position management
#define CURSOR_UP(n)         "\033["#n"A"
#define CURSOR_DOWN(n)       "\033["#n"B"
#define CURSOR_RIGHT(n)      "\033["#n"С"
#define CURSOR_LEFT(n)       "\033["#n"D"
#define CURSOR_DOWN_NL(n)    "\033["#n"E"
#define CURSOR_UP_NL(n)      "\033["#n"F"
#define CURSOR_TAB(n)        "\033["#n"G"
#define CURSOR_GOTO(x,y)     "\033["#x";"#y"H"


// palette

// set palette. n -- color number; r, g & b -- RGB-components in hex
#define PALETTE_SET(n,r,g,b) "\033]P"#n#r#g#b 
#define PALETTE_RESET        "\033]R"

// console
#define CONSOLE_SWITCHTO(n)  "\033[12;"#n"]"

// xterm
#define CONSOLE_ICONTEXT(s) "\033]0;" s "\007"
#define CONSOLE_ICON(s) "\033]1;" s "\007"
#define CONSOLE_TEXT(s) "\033]2;" s "\007"

#endif

#endif // _CONSOLE_H_
