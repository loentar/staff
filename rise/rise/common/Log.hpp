#ifndef _LOG_HPP_
#define _LOG_HPP_

namespace rise
{
  template<typename TDATA>
  const CLogStream& CLogStream::operator<<( TDATA tData ) const
  {
    if (m_pLogStream != NULL)
      *m_pLogStream << tData;
    return *this;
  }
}

#endif // _LOG_HPP_
