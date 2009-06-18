#ifndef _WSDLTYPES_H_
#define _WSDLTYPES_H_

#include <list>

namespace staff
{
  /* primitive types */
  using std::string;
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
  typedef string hexBinary;
  typedef string base64Binary;
  typedef string anyURI;
  typedef string QName;
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
  typedef unsigned long unsignedLong;
  typedef unsigned int unsignedInt;
  typedef unsigned short unsignedShort;
  typedef unsigned char unsignedByte;
  typedef unsigned int positiveInteger;


  /* SOAP types */
  template<typename Item> 
  class Array: public std::list<Item>
  {
  };
 }

#endif // _WSDLTYPES_H_