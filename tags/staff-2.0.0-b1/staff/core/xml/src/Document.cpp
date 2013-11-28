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

#include "Exception.h"
#include "Document.h"

namespace staff
{
namespace xml
{

  const Declaration& Document::GetDeclaration() const
  {
    return m_tDeclaration;
  }

  Declaration& Document::GetDeclaration()
  {
    return m_tDeclaration;
  }

  const Element& Document::GetRootElement() const
  {
    return m_tRootElement;
  }

  Element& Document::GetRootElement()
  {
    return m_tRootElement;
  }

} // namespace xml
} // namespace staff
