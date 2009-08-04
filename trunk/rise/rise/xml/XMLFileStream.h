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

#ifndef _XMLFILESTREAM_H_
#define _XMLFILESTREAM_H_

#include <rise/common/streamtypes.h>

namespace rise
{
  namespace xml
  {
    //!        internal class for stream xml file reading
    class RISE_EXPORT CXMLIStream
    {
    public:
      CXMLIStream(CIStream& rStream, const CString& sFileName, int& nLine);
      CXMLIStream(CIStream& rStream, const CString& sFileName);
      CXMLIStream(CIStream& rStream);
      CXMLIStream(CXMLIStream& rStream);
      ~CXMLIStream();
      operator CIStream&();
      bool ReadChar(TChar& rChar);
      TChar ReadChar();
      TChar ReadMetaChar();
      bool ReadMetaChar(TChar& chData);
      bool PeekChar(TChar& rChar);
      TChar PeekChar();
      void SkipWhitespace();
      int ReadString(CString& sData, const CString& sDelimiters = " \t\n\r");
      void ReadStringUntil(CString& sString, const CString& sMarker = "");
      void ReadId(CString& sId);
      bool Test(const CString& sValue);
      const CString& GetFileName() const;
      int GetLine() const;
      void UnGetChar(const TChar chData);
      void AddContent(CString& sContent);

    private:
      void UnGetString(const CString& sData);

    private:
      int                 m_nLineInternal;
      int&                m_nLine;
      const CString       m_sFileNameInternal;
      const CString&      m_sFileName;
      CIStream&           m_tIStream;

      CXMLIStream& operator =(const CXMLIStream&);

    };

    //!        internal class for stream xml file writing
    class RISE_EXPORT CXMLOStream
    {
    public:
      CXMLOStream(COStream& rOStream, const CString& sFileName);
      CXMLOStream(COStream& rOStream);
      CXMLOStream(CXMLOStream& rStream);
      ~CXMLOStream();
      void WriteString(const CString& sString);
      template<typename TDATA>
      CXMLOStream& operator<<( const TDATA& tData )
      {
        m_tOStream << tData;
        return *this;
      }
      operator COStream&();

    private:
      COStream&      m_tOStream;
      const CString& m_sFileName;
      const CString  m_sFileNameInternal;
      CXMLOStream& operator=(const CXMLOStream&);
    };

    //!        internal class for stream xml-file reading
    class RISE_EXPORT CXMLFileIStream: public CXMLIStream
    {
    public:
      CXMLFileIStream();
      ~CXMLFileIStream();
      void Open(const CString& sFileName);
      void Close();

    private:
      int             m_nLine;
      CString         m_sFileName;
      CIFStream       m_tIFStream;
    };

    //!        internal class for stream xml-file writing
    class RISE_EXPORT CXMLFileOStream: public CXMLOStream
    {
    public:
      CXMLFileOStream();
      ~CXMLFileOStream();
      void Open(const CString& sFileName);
      void Close();

    private:
      COFStream         m_tOFStream;
      CString           m_sFileName;
    };

  } // namespace xml
} // namespace rise

#endif // _XMLFILESTREAM_H_
