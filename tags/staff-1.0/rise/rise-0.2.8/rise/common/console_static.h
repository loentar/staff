#ifndef _CONSOLE_STATIC_H_
#define _CONSOLE_STATIC_H_

#ifdef WIN32

////////////////////////////////////////////////////////////////////
// WINDOWS
#define COLOR_DEFAULT        "" //! все атрибуты по умолчанию 

#define COLOR_BRIGHTNESS_ON  "" //! яркий цвет
#define COLOR_DIM_ON         "" //! полуяркий цвет
#define COLOR_UNDERLINE_ON   "" //! подчеркивание
#define COLOR_FLASH_ON       "" //! мигающий 
#define COLOR_INVERSE_ON     "" //! инверсия

#define COLOR_BRIGHTNESS_OFF "" //! отмена яркого цвета
#define COLOR_UNDERLINE_OFF  "" //! отмена подчеркивание 
#define COLOR_FLASH_OFF      "" //! отмена мигание 
#define COLOR_INVERSE_OFF    "" //! отмена инверсию 

//! Цвет знаков 
#define COLOR_INK_BLACK      "" //! чёрный
#define COLOR_INK_RED        "" //! красный
#define COLOR_INK_GREEN      "" //! зелёный
#define COLOR_INK_BROWN      "" //! коричневый
#define COLOR_INK_BLUE       "" //! синий
#define COLOR_INK_MAGENTA    "" //! фиолетовый
#define COLOR_INK_CYAN       "" //! циановый 
#define COLOR_INK_GREY       "" //! серый

//! Цвет фона
#define COLOR_PAPER_BLACK    "" //! чёрный
#define COLOR_PAPER_RED      "" //! красный
#define COLOR_PAPER_GREEN    "" //! зелёный 
#define COLOR_PAPER_BROWN    "" //! коричневый
#define COLOR_PAPER_BLUE     "" //! синий
#define COLOR_PAPER_MAGENTA  "" //! фиолетовый
#define COLOR_PAPER_CYAN     "" //! циановый
#define COLOR_PAPER_GREY     "" //! серый


//! управление позицией курсора
#define CURSOR_UP(n)         "" //! передвинуть курсор вверх на n строк 
#define CURSOR_DOWN(n)       "" //! передвинуть курсор вниз на n строк 
#define CURSOR_RIGHT(n)      "" //! передвинуть курсор вправо на n столбцов 
#define CURSOR_LEFT(n)       "" //! передвинуть курсор влево на n столбцов 
#define CURSOR_DOWN_NL(n)    "" //! передвинуть курсор вниз на n строк и поставить в начало строки 
#define CURSOR_UP_NL(n)      "" //! передвинуть курсор вверх на n строк и поставить в начало строки 
#define CURSOR_TAB(n)        "" //! переместить курсор в n-й столбец текущей строки 
#define CURSOR_GOTO(x,y)     "" //! задает абсолютные координаты курсора (строка, столбец) 


// Работа с палитрой 
  
//! Изменяет палитру. n -- номер цвета; rr, gg и bb -- значения RGB-компонент в шестнадцатеричной форме 
#define PALETTE_SET(n,r,g,b) ""
#define PALETTE_RESET        "" //! Восстанавливает палитру, принятую по умолчанию 

//! Работа с виртуальными консолями 
#define CONSOLE_SWITCHTO(n)  ""  //! сделать текущей консоль с указанным номером 

// Работа с xterm 
   
#define CONSOLE_ICONTEXT(s) ""   //! Присваивает имя окну и иконке 
#define CONSOLE_ICON(s) "" //! Присваивает имя иконке 
#define CONSOLE_TEXT(s) "" //! Присваивает имя окну 

#else  
//////////////////////////////////////////////////////////////////////////
//  LINUX

#define COLOR_DEFAULT        "\033[0m" //! все атрибуты по умолчанию 

#define COLOR_BRIGHTNESS_ON  "\033[1m" //! яркий цвет
#define COLOR_DIM_ON         "\033[2m" //! полуяркий цвет
#define COLOR_UNDERLINE_ON   "\033[4m" //! подчеркивание
#define COLOR_FLASH_ON       "\033[5m" //! мигающий 
#define COLOR_INVERSE_ON     "\033[7m" //! инверсия

#define COLOR_BRIGHTNESS_OFF "\033[22m" //! отмена яркого цвета
#define COLOR_UNDERLINE_OFF  "\033[24m" //! отмена подчеркивание 
#define COLOR_FLASH_OFF      "\033[25m" //! отмена мигание 
#define COLOR_INVERSE_OFF    "\033[27m" //! отмена инверсию 

//! Цвет знаков 
#define COLOR_INK_BLACK      "\033[30m" //! чёрный
#define COLOR_INK_RED        "\033[31m" //! красный
#define COLOR_INK_GREEN      "\033[32m" //! зелёный
#define COLOR_INK_BROWN      "\033[33m" //! коричневый
#define COLOR_INK_BLUE       "\033[34m" //! синий
#define COLOR_INK_MAGENTA    "\033[35m" //! фиолетовый
#define COLOR_INK_CYAN       "\033[36m" //! циановый 
#define COLOR_INK_GREY       "\033[37m" //! серый

//! Цвет фона
#define COLOR_PAPER_BLACK    "\033[40m" //! чёрный
#define COLOR_PAPER_RED      "\033[41m" //! красный
#define COLOR_PAPER_GREEN    "\033[42m" //! зелёный 
#define COLOR_PAPER_BROWN    "\033[43m" //! коричневый
#define COLOR_PAPER_BLUE     "\033[44m" //! синий
#define COLOR_PAPER_MAGENTA  "\033[45m" //! фиолетовый
#define COLOR_PAPER_CYAN     "\033[46m" //! циановый
#define COLOR_PAPER_GREY     "\033[47m" //! серый


//! управление позицией курсора
#define CURSOR_UP(n)         "\033["#n"A" //! передвинуть курсор вверх на n строк 
#define CURSOR_DOWN(n)       "\033["#n"B" //! передвинуть курсор вниз на n строк 
#define CURSOR_RIGHT(n)      "\033["#n"С" //! передвинуть курсор вправо на n столбцов 
#define CURSOR_LEFT(n)       "\033["#n"D" //! передвинуть курсор влево на n столбцов 
#define CURSOR_DOWN_NL(n)    "\033["#n"E" //! передвинуть курсор вниз на n строк и поставить в начало строки 
#define CURSOR_UP_NL(n)      "\033["#n"F" //! передвинуть курсор вверх на n строк и поставить в начало строки 
#define CURSOR_TAB(n)        "\033["#n"G" //! переместить курсор в n-й столбец текущей строки 
#define CURSOR_GOTO(x,y)     "\033["#x";"#y"H" //! задает абсолютные координаты курсора (строка, столбец) 


// Работа с палитрой 
  
// Изменяет палитру. n -- номер цвета; rr, gg и bb -- значения RGB-компонент в шестнадцатеричной форме 
#define PALETTE_SET(n,r,g,b) "\033]P"#n#r#g#b 
#define PALETTE_RESET        "\033]R" //! Восстанавливает палитру, принятую по умолчанию 

// Работа с виртуальными консолями 
#define CONSOLE_SWITCHTO(n)  "\033[12;"#n"]"  //! сделать текущей консоль с указанным номером 

// Работа с xterm 
   
#define CONSOLE_ICONTEXT(s) "\033]0;" s "\007"   //! Присваивает имя окну и иконке 
#define CONSOLE_ICON(s) "\033]1;" s "\007" //! Присваивает имя иконке 
#define CONSOLE_TEXT(s) "\033]2;" s "\007" //! Присваивает имя окну 

#endif

#endif // _CONSOLE_H_
