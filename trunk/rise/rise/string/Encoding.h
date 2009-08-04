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

#ifndef _ENCODING_H_
#define _ENCODING_H_

#include <rise/os/osdllexport.h>
#include <rise/string/String.h>
#include <map>

namespace rise
{
  //!  Encoding description
  struct RISE_EXPORT SEncodingType
  {
    int       nID;       //!< encoding id
    CString   sName;     //!< encoding name
    CString   sAltName;  //!< encoding name for XML
    CStringA  asTable;   //!< single byte encoding
    CStringW  wsTable;   //!< multi byte encoding
    CStringA  asMagic;   //!< Magic-bytes for encoding detection

    //!         constructor for creating encoding description
    /*! \param  nIDInit - encoding id
        \param  sNameInit - encoding name
        \param  sAltNameInit - encoding name for XML
        \param  asTableInit - single byte encoding
        \param  wsTableInit - multi byte encoding
        \param  asMagicInit - Magic-bytes for encoding detection
        */
    SEncodingType(int nIDInit, const CString& sNameInit, const CString& sAltNameInit, const CStringA& asTableInit, const CStringW& wsTableInit = L"", const CStringA& asMagicInit = "");
  };

  //!  Encoding type converter
  class RISE_EXPORT CEncoding
  {
  public:
    //! encoding type
    enum EType
    {
      ET_UNKNOWN,    //!<  unknown
      ET_UTF_8,      //!<  utf-8
      ET_KOI8R,      //!<  koi8-r
      ET_WIN1251,    //!<  windows-1251
      ET_CP866,      //!<  dos(codepage 866)
      ET_MACINTOSH,  //!<  macintosh
      ET_ISO_8859_5, //!<  iso8859-5
      ET_LAST,       //!<  last used
      ET_USER        //!<  user encoding
    };

  public:
    //! encoding map
    typedef std::map<int, SEncodingType> TEncMap;

  public:
    //!         add user encoding
    /*! \param  rstEncType - encoding type
        */
    static void AddUserEncodingType(const SEncodingType& rstEncType);
    
    //!         get user encodings map
    /*! \return user encodings map
    */
    static const TEncMap& GetEncMap();

    //!         get encoding description by id
    /*! \param  nID - identifier
        \return encoding description
        */
    static const SEncodingType& FindEncodingTypeByID(int nID);
    
    //!         get encoding description by name
    /*! \param  sEncName - name
        \return encoding description
        */
    static const SEncodingType& FindEncodingTypeByName(const CString& sEncName);

    //!         convert string
    /*! \param  sFrom - source string
        \param  sTo - resulting string
        \param  nIdFrom - source encoding
        \param  nIdTo - resulting encoding
        \return 
        */
    static void Convert(const CString& sFrom, CString& sTo, EType nIdFrom, EType nIdTo);

  private:
    static TEncMap  m_mEncMap; //!<  encoding map
  };
}

#endif // _ENCODING_H_
