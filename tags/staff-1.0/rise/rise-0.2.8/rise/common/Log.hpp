#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <rise/string/Encoding.h>

namespace rise
{
  template<typename TDATA>
  const CLogStream& CLogStream::operator<<( TDATA tData ) const
  {
    if (m_pLogStream != NULL)
    {
      if(m_nSrcRecode != m_nDstRecode)
      {
        COStringStream tOStringStream;
        tOStringStream << tData << '\0' << '\0';
        OutRecoded(tOStringStream.str());
      }
      else
      {
        *m_pLogStream << tData;
      }
    }
    
    return *this;
  }
}

#endif // _LOG_HPP_
