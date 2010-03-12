namespace rise
{
  template<class TDATA>
  struct CManip1
  {
    COStream& (*PFunction)(COStream&, TDATA);
    TDATA m_tValue;
    CManip1(COStream& (*pFunction)(COStream&, TDATA), TDATA tValue):
      PFunction(pFunction), m_tValue(tValue) {}

  private:
     CManip1& operator=(const CManip1&);
  };

  template<class TDATA>
  COStream& operator<<(COStream& rStream, const CManip1<TDATA>& cManipValue)
  {
    return cManipValue.PFunction(rStream, cManipValue.m_tValue);
  }

  //////////////////////////////////////////////////////////////////////////
  // manip 2

  template<class TDATA>
  struct CManip2
  {
    COStream& (*PFunction)(COStream&, TDATA, TDATA);
    TDATA m_tValue1;
    TDATA m_tValue2;
    CManip2(COStream& (*pFunction)(COStream&, TDATA, TDATA), TDATA tValue1, TDATA tValue2):
      PFunction(pFunction), m_tValue1(tValue1), m_tValue2(tValue2) {}
  };

  template<class TDATA>
  COStream& operator<<(COStream& rStream, const CManip2<TDATA>& cManipValue)
  {
    return cManipValue.PFunction(rStream, cManipValue.m_tValue1, cManipValue.m_tValue2);
  }

  //////////////////////////////////////////////////////////////////////////
  // manip 4


  template<class TDATA>
  struct CManip4
  {
    COStream& (*PFunction)(COStream&, TDATA, TDATA, TDATA, TDATA);
    TDATA m_tValue1;
    TDATA m_tValue2;
    TDATA m_tValue3;
    TDATA m_tValue4;
    CManip4(COStream& (*pFunction)(COStream&, TDATA, TDATA, TDATA, TDATA), TDATA tValue1, TDATA tValue2, TDATA tValue3, TDATA tValue4):
    PFunction(pFunction), m_tValue1(tValue1), m_tValue2(tValue2) , m_tValue3(tValue3) , m_tValue4(tValue4) {}
  };

  template<class TDATA>
  COStream& operator<<(COStream& rStream, const CManip4<TDATA>& cManipValue)
  {
    return cManipValue.PFunction(rStream, cManipValue.m_tValue1, cManipValue.m_tValue2, cManipValue.m_tValue3, cManipValue.m_tValue4);
  }


  ////////////////////////////////////////////////////////////////

  template<class TDATA>
  COStream& _outnum(COStream& rStream, TDATA tValue)
  {
    return (rStream << tValue);
  }

  template<class TDATA>
  CManip1<TDATA> outnum(TDATA tValue)
  {
    return CManip1<TDATA>(_outnum, tValue);
  }

} // namespace rise
