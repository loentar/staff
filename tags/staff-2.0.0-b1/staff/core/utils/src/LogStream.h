/*
 *  Copyright 2011 Utkin Dmitry
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


#ifndef _STAFF_UTILS_LOGSTREAM_H_
#define _STAFF_UTILS_LOGSTREAM_H_

#include <string>
#include <ostream>
#include "staffutilsexport.h"

namespace staff
{
  // wsdl compat
  typedef char byte;
  typedef unsigned char unsignedByte;

  STAFF_UTILS_EXPORT std::ostream& ColorDefault(std::ostream& rStream);

  //! log stream - log output helper (internal use only)
  class STAFF_UTILS_EXPORT LogStream
  {
  public:
    inline LogStream(std::ostream* pStream, bool bWriteEndLine = true):
      m_pStream(pStream), m_bWriteEndLine(bWriteEndLine)
    {
    }

    inline ~LogStream()
    {
      if (m_pStream && m_bWriteEndLine)
      {
        *m_pStream << ColorDefault << std::endl;
      }
    }

    inline LogStream& operator<<(bool bValue)
    {
      if (m_pStream)
      {
        // override std::boolalpha
        *m_pStream << (bValue ? "true" : "false");
      }
      return *this;
    }

    // for complex types
    template<typename Type>
    inline LogStream& operator<<(Type tData)
    {
      if (m_pStream)
      {
        *m_pStream << tData;
      }
      return *this;
    }

  private:
    friend LogStream& LogEndLOff(LogStream& rLogStream);
    friend LogStream& LogEndLOn(LogStream& rLogStream);

  private:
    std::ostream* m_pStream;
    bool          m_bWriteEndLine;
  };

  //! disable carriage return
  inline LogStream& LogEndLOff(LogStream& rLogStream)
  {
    rLogStream.m_bWriteEndLine = false;
    return rLogStream;
  }

  //! enable carriage return
  inline LogStream& LogEndLOn(LogStream& rLogStream)
  {
    rLogStream.m_bWriteEndLine = true;
    return rLogStream;
  }

} // namespace staff

#endif // _STAFF_UTILS_LOGSTREAM_H_
