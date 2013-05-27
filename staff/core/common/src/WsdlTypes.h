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

#ifndef _WSDLTYPES_H_
#define _WSDLTYPES_H_

#include <string>
#include <list>

namespace staff
{
  /* primitive types */
  typedef std::string string;
  typedef bool boolean;
//  typedef float float;
//  typedef double double;
  typedef double decimal;
  typedef string duration;
  typedef string dateTime;
  typedef string time;
  typedef string date;
  typedef string gYearMonth;
  typedef string gYear;
  typedef string gMonthDay;
  typedef string gDay;
  typedef string gMonth;
  typedef class HexBinary hexBinary;
  typedef class Base64Binary base64Binary;
  typedef string anyURI;
  class QName;
  typedef string NOTATION;

  /* derived types */
  typedef string normalizedString;
  typedef normalizedString token;
  typedef token language;
  typedef string IDREFS;
  typedef string ENTITIES;
  typedef string NMTOKEN;
  typedef string NMTOKENS;
  typedef string Name;
  typedef string NCName;
  typedef string ID;
  typedef string IDREF;
  typedef string ENTITY;
  typedef int integer;
  typedef int nonPositiveInteger;
  typedef int negativeInteger;
//  typedef long long;
//  typedef int int;
//  typedef short short;
  typedef char byte;
  typedef unsigned int nonNegativeInteger;
  typedef unsigned long long unsignedLong;
  typedef unsigned int unsignedInt;
  typedef unsigned short unsignedShort;
  typedef unsigned char unsignedByte;
  typedef unsigned int positiveInteger;

  typedef string anySimpleType;
  typedef string anyType;

  class Attribute;
  typedef std::list<Attribute> anyAttribute;

  template<typename Type> class Abstract;

}

#endif // _WSDLTYPES_H_
