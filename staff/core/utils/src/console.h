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

#ifndef _STAFF_UTILS_CONSOLE_H_
#define _STAFF_UTILS_CONSOLE_H_

#include <iosfwd>
#include "staffutilsexport.h"

namespace staff
{
  //! use default color
  STAFF_UTILS_EXPORT std::ostream& ColorDefault(std::ostream& rStream);

  //! bright color
  STAFF_UTILS_EXPORT std::ostream& ColorBright(std::ostream& rStream);
  //! dim color
  STAFF_UTILS_EXPORT std::ostream& ColorDim(std::ostream& rStream);
  //! default bright
  STAFF_UTILS_EXPORT std::ostream& ColorBrightDefault(std::ostream& rStream);
  //! enable flashing
  STAFF_UTILS_EXPORT std::ostream& ColorFlashOn(std::ostream& rStream);
  //! disable flashing
  STAFF_UTILS_EXPORT std::ostream& ColorFlashOff(std::ostream& rStream);
  //! enable inversion
  STAFF_UTILS_EXPORT std::ostream& ColorInverseOn(std::ostream& rStream);
  //! disable inversion
  STAFF_UTILS_EXPORT std::ostream& ColorInverseOff(std::ostream& rStream);
  //! enable underlining
  STAFF_UTILS_EXPORT std::ostream& ColorUnderlineOn(std::ostream& rStream);
  //! disable underlining
  STAFF_UTILS_EXPORT std::ostream& ColorUnderlineOff(std::ostream& rStream);
  
  // text color
  //! black text
  STAFF_UTILS_EXPORT std::ostream& ColorTextBlack(std::ostream& rStream);
  //! red text
  STAFF_UTILS_EXPORT std::ostream& ColorTextRed(std::ostream& rStream);
  //! green text
  STAFF_UTILS_EXPORT std::ostream& ColorTextGreen(std::ostream& rStream);
  //! brown text
  STAFF_UTILS_EXPORT std::ostream& ColorTextBrown(std::ostream& rStream);
  //! blue text
  STAFF_UTILS_EXPORT std::ostream& ColorTextBlue(std::ostream& rStream);
  //! magenta text
  STAFF_UTILS_EXPORT std::ostream& ColorTextMagenta(std::ostream& rStream);
  //! cyan text
  STAFF_UTILS_EXPORT std::ostream& ColorTextCyan(std::ostream& rStream);
  //! gray text
  STAFF_UTILS_EXPORT std::ostream& ColorTextGrey(std::ostream& rStream);
  
  // background color
  //! black background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundBlack(std::ostream& rStream);
  //! red background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundRed(std::ostream& rStream);
  //! green background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundGreen(std::ostream& rStream);
  //! brown background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundBrown(std::ostream& rStream);
  //! blue background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundBlue(std::ostream& rStream);
  //! magenta background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundMagenta(std::ostream& rStream);
  //! cyan background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundCyan(std::ostream& rStream);
  //! gray background
  STAFF_UTILS_EXPORT std::ostream& ColorBackgroundGrey(std::ostream& rStream);

}  // namespace staff

#endif
