#include <rise/os/osdllexport.h>

#include <iostream>
#include <fstream>
#include <strstream>
#include <stdlib.h>
#include <locale.h>

// #include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/types.h>
#include <rise/common/DataEncoder.h>
#include <rise/common/PerformanceCounter.h>
#include <rise/threading/SharedPtr.h>
#include <rise/xml/XMLFileStream.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDecl.h>
#include <rise/xml/XMLDocument.h>
#include <rise/threading/Thread.h>
#include <rise/timer/TimerHandler.h>
#include <rise/os/osprocess.h>
#include <rise/socket/Socket.h>
#include <rise/socket/RecvSocket.h>
#include <rise/socket/ClientSocket.h>
#include <rise/socket/ServerSocket.h>
#include <rise/common/StreamBuffer.h>
#include <rise/process/Process.h>
#include <rise/common/console.h>
#include <rise/signal/SignalHandler.h>
#include <rise/tools/StackTracer.h>
#include <rise/tools/FileFind.h>
#include <rise/string/Encoding.h>

#include <rise/plugin/PluginManager.h>
#include "../myplugin/src/MyPlugin.h"

class CEntry
{
public:
  CEntry()
  {
    tStdOut << "Testing ";
  }
  
  ~CEntry()
  {
    tStdOut << "End" << std::endl;
  }
  
  template<typename TDATA>
  rise::COStream& operator<<( TDATA tData )
  {
    return tStdOut << tData;
  }

};


class CMyThread: public rise::threading::CThread 
{
public:
  using CThread::Stop;
  void Run(void* /*pparam*/)
  {
    for (int i =0; i < 100; ++i)
    {
      rise::LogDebug1() << "ChildThread. no:" << i;
      Sleep(15);
      if (IsStopping())
      {
        rise::LogDebug1() << "Stopping";
        break;
      }
    }
  }
};

class CMyTimer1: public rise::CTimerHandler
{
  void OnTimer( rise::word wID )
  {
   rise::LogDebug1() << "Timer1, Id: " << wID;
  }
};

class CMyTimer2: public rise::CTimerHandler
{
  void OnTimer( rise::word wID )
  {
   rise::LogDebug1() << "Timer2, Id: " << wID;
  }
};

//////////////////////////////////////////////////////////////////////////////
//    Sockets
//////////////////////////////////////////////////////////////////////////////
  class CClientThread: private rise::threading::CThread, public rise::sockets::CRecvSocket
  {
  public:
    using CThread::Start;
    using CThread::Cancel;
  private:
    rise::CStreamBuffer cBuffer;

    void Run(void*)
    {
      rise::LogDebug1() << "Client: LocalIP: " << CSocket::GetLocalIPStr() << "; RemoteIP: " << CSocket::GetRemoteIPStr();
//      std::tStdOut << "thread #") << rise::ToStr(CThread::GetId()) << _T(" is launched!" << std::endl;
      try
      {
        rise::LogDebug1() << "waiting for data...";
        if (WaitForData(2))
          if(!CRecvSocket::Recv(cBuffer))
            rise::LogWarning() << "can't recv";
          else
          {
            rise::CString sString;
            int nCode;
            cBuffer >> sString >> nCode;

           rise::LogDebug1() << "Received: " << sString << nCode;
          }
        }
        RISE_CATCH_ALL_DESCR("exception will raised, thread ") << CThread::GetId() << " will now exit";
    }
  };


  class CMyServerSocket: private rise::threading::CThread, public rise::sockets::CServerSocket
  {
  public:
    using CThread::Start;
    using CThread::Cancel;
    using CThread::JoinThread;
  
  private:
    CClientThread m_tClient;
    void Run(void*)
    {
      try
      {
        rise::LogDebug1() << "create 8023";
        Create(8023);
        rise::LogDebug1() << "Accept";
        Accept( m_tClient );
        rise::LogDebug1() << "m_tClient.Start";
        m_tClient.Start();
        rise::LogDebug1() << "Sleep";
        rise::threading::CThread::Sleep(2000);
      }
      RISE_CATCH_ALL
    }

  };


/////////////////////////////////////////////////////////////////////
// for XML
/////////////////////////////////////////////////////////////////////

      struct SNodeSort
      {
        bool operator()( const rise::xml::CXMLNode& rNode1, const rise::xml::CXMLNode& rNode2 ) const
        {
          return rNode1.NodeName() < rNode2.NodeName();
        }
      };

      struct SNodeSortN
      {
        bool operator()( const rise::xml::CXMLNode& rNode1, const rise::xml::CXMLNode& rNode2 ) const
        {
          return rNode1.Attribute("n")).AsString() < rNode2.Attribute("n".AsString();
        }
      };
      struct SAttrSort
      {
        bool operator()(const rise::xml::SXMLAttribute& rAttr1, const rise::xml::SXMLAttribute& rAttr2 ) const
        {
          return rAttr1.sAttrName < rAttr2.sAttrName;
        }
      };

      struct SNodeFind
      {
        bool operator()( const rise::xml::CXMLNode& rNode ) const
        {
          return rNode.NodeType() == rise::xml::CXMLNode::ENTCDATA;
        }
      };

      struct SAttrFind
      {
        bool operator()( const rise::xml::SXMLAttribute& rAttr ) const
        {
          return rAttr.sAttrValue == "e";
        }
      };

      struct SNodeSortID
      {
        bool operator()( const rise::xml::CXMLNode& rNode1, const rise::xml::CXMLNode& rNode2 ) const
        {
          int nID1 = 0;
          int nID2 = 0;
          rise::FromStr(rNode1.Attribute("id"), nID1);
          rise::FromStr(rNode2.Attribute("id"), nID2);
          return nID1 < nID2;
        }
      };

class CTestCS
{
private:
  rise::threading::CCriticalSection m_tCS;

public:
  void TestCrit()
  {
    m_tCS.Enter();
    RISE_ASSERTS( m_tCS.TryEnter(), "Critical section enter failed");

    tStdOut << "testcs";
    
    m_tCS.Leave();
    m_tCS.Leave();
  }

};

static class CTestSig: public rise::CSignalHandler
{
public:
  CTestSig()
  {
    RegisterSignal(ESIGINT);
  }

  void SignalHandler(int nSignal)
  {
    rise::LogDebug1() << "Signal #") << nSignal << "(") << GetSignalStr(nSignal) << _T("";
  }

} tSigTest;



int main(int argc, const rise::TChar* argv[])
{

  tStdOut << rise::ConsoleSetWindowText("rise test");

  std::ofstream fLog; // переменная должна существовать пока ведется логгинг!!!!!

  rise::CPerformanceCounter tPerfCounter;

//   {
//     rise::process::CProcess tProcTest;
//     tProcTest.Exec("notepad\tconfig.xml");
//   }

  bool bLogToFile = false;

   if ( argc > 1 )
     if ( rise::CString(argv[1]) == "-logtofile" )
       bLogToFile = true;
       
  rise::tLog.SetLogLevel(rise::CLog::ELL_INFO);

  if (!bLogToFile)
  {
    fLog.open("out/rise_test.log", std::ios::out | std::ios::binary);
    rise::tLog.SetLogLevel(rise::CLog::ELL_DEBUG3);
//   rise::Log.SetLogStream(&fLog, rise::CLog::ELL_INFO | rise::CLog::ELL_DEBUG | rise::CLog::ELL_DEBUG1 | rise::CLog::ELL_DEBUG2 | rise::CLog::ELL_DEBUG3);
//   rise::Log.SetLogStream(&fLog, rise::CLog::ELL_ALL);
  }


  tStdOut << "Тест русского языка" << std::endl;

  //////////////////////////////////////////////////////////////////////////////
  //    Тест на правильность подключения библиотеки stdc++ к rise
  //////////////////////////////////////////////////////////////////////////////
  {
    try 
    {
      tStdOut << "rise runtime link test";
      {
        std::string sRes;
        rise::RiseLinkRuntimeTest(sRes);
      }
      tStdOut << rise::LogResultSuccess;
    } catch(...)
    {
      rise::LogError() << rise::LogResultFailed;
      abort();
    };
  }

  try
  {
    {
      CEntry() << "testcs";
      tStdOut << "Testing CS ";
      CTestCS tTestCS;
      tTestCS.TestCrit();
      tStdOut << rise::LogResultSuccess;
    }

    {
      rise::CStringA sIn = "\xd4\xc5\xd3\xd4test";
      rise::CStringA sOutWin;
      rise::CStringA sOutUtf8;
      rise::CStringA sOutSrc;
      rise::CEncoding::Convert(sIn, sOutWin, rise::CEncoding::ET_KOI8R, rise::CEncoding::ET_WIN1251);
      rise::CEncoding::Convert(sIn, sOutUtf8, rise::CEncoding::ET_KOI8R, rise::CEncoding::ET_UTF_8);

      RISE_ASSERTS( sOutWin == "\xf2\xe5\xf1\xf2test", "ConvertWin");
      RISE_ASSERTS( sOutUtf8 == "\xd1\x82\xd0\xb5\xd1\x81\xd1\x82test", "ConvertUtf8");

      rise::CEncoding::Convert(sOutUtf8, sOutSrc, rise::CEncoding::ET_UTF_8, rise::CEncoding::ET_KOI8R);
      RISE_ASSERTS( sOutSrc == sIn, "Convert back");

    }

    //////////////////////////////////////////////////////////////////////////////
    //    FileFind
    //////////////////////////////////////////////////////////////////////////////
    {
      rise::CStringList slsTest;
      rise::CFileFind::Find(".", slsTest, "*.xml", rise::CFileFind::EFA_FILE);
      rise::CFileFind::Find(".", slsTest, "*", rise::CFileFind::EFA_DIR);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    rise::CString test
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CString... ";

      // basic test ////////////////////////////////////////////////////////////
      rise::CString ss = rise::CString("asdqwe");
      RISE_ASSERTS( ss == "asdqwe"), "CString 0";

      RISE_ASSERTS( rise::CString("test")) == "test"), _T("CString 1";
//       RISE_ASSERTS( rise::CString("test")) == "test", "CString 2";
//       RISE_ASSERTS( rise::CString("test")).ToW() == L"test", "CString 3";
//       RISE_ASSERTS( (CString("test")) + "123")) == _T("test123"), _T("CString 4";

//       RISE_ASSERTS( rise::CString("1")) < "asd"), _T("CString 5";
//       RISE_ASSERTS( rise::CString("3")) > " "), _T("CString 6";

      //////////////////////////////////////////////////////////////////////////
///      tStdOut << "CString" << std::flush;
      rise::CString str("123456789");

//       StrReplace(str, 0, 1, "_");
//       StrReplace(str, 2, 6, "876543");

//       str = "123456789";
//       StrReplace(str, 2, 6, "*"); // 12*9
//       StrStr();
//       StrReplace(str, 1, 3, "");

      str = "123456789";
      rise::StrReplace(str, "9"), "<_+>";
      RISE_ASSERTS( str == "12345678<_+>"), "replace 1";

      rise::StrReplace(str, "1"), "";
      RISE_ASSERTS( str == "2345678<_+>"), "replace 2";

      rise::StrReplace(str, "5"), " ";
      RISE_ASSERTS( str == "234 678<_+>"), "replace 2";
      
      //////////////////////////////////////////////////////////////////////////
      // Trim
      str = "  11 23";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23"), "Trim1";

      str = "11 23  ";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23"), "Trim2";

      str = "  11 23  ";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23"), "Trim3";

      //////////////////////////////////////////////////////////////////////////
      // left/right/mid test
      str = "123456";

      //////////////////////////////////////////////////////////////////////////
      // Left
      RISE_ASSERTS( rise::StrLeft(str, 3) == "123"), "Left1";
      RISE_ASSERTS( rise::StrLeft(str, 0) == ""), "Left2";
      RISE_ASSERTS( rise::StrLeft(str, rise::CString::npos) == "123456"), "Left2";

      //////////////////////////////////////////////////////////////////////////
      // Right
      RISE_ASSERTS( rise::StrRight(str, 3) == "456"), "Right1";
      RISE_ASSERTS( rise::StrRight(str, 0) == ""), "Right2";

      //////////////////////////////////////////////////////////////////////////
      // Mid
      RISE_ASSERTS( rise::StrMid(str, 2, 3) == "345"), "Mid1";
      RISE_ASSERTS( rise::StrMid(str, 2) == "3456"), "Mid2";

      //////////////////////////////////////////////////////////////////////////
      // Find
//       RISE_ASSERTS( rise::StrFind("34")) == 2, _T("Find1" );
//       RISE_ASSERTS( rise::StrFind("as34")) == rise::CString::npos, _T("Find2" );
//       RISE_ASSERTS( rise::StrFindFirstOf("53")) == 2, _T("Find3" );
//       RISE_ASSERTS( rise::StrFindFirstOf(" ")) == rise::CString::npos, _T("Find4" );
//       RISE_ASSERTS( rise::StrFindLastOf("53")) == 4, _T("Find5" );
//       RISE_ASSERTS( rise::StrFindLastOf(" ")) == rise::CString::npos, _T("Find6" );
//       RISE_ASSERTS( rise::StrFindFirstNotOf("1")) == 1, _T("Find7" );
//       RISE_ASSERTS( rise::StrFindFirstNotOf(" ")) == 0, _T("Find8" );
//       RISE_ASSERTS( rise::StrFindLastNotOf("6")) == 5, _T("Find9" );
//       RISE_ASSERTS( rise::StrFindLastNotOf(" ")) == rise::CString::npos, _T("Find10" );

//      tStdOut << rise::LogResultSuccess;

    //    FromStr
      int nVal = 0;
      rise::FromStr("123", nVal);
      RISE_ASSERTS(nVal == 123, "FromStr!");

    //    ToStr
      rise::CString sRes;
      rise::ToStr(123, sRes);
      RISE_ASSERTS(sRes == "123"), "ToStr1!";
      RISE_ASSERTS(rise::ToStr(123) == "123"), "ToStr2!";
      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    rise::CProcess test
    //////////////////////////////////////////////////////////////////////////////
    {
      rise::LogNotice() << rise::process::CProcess::GetCurrentExecPath();
    }

    {
      rise::CString sTrace;
      rise::tools::CStackTracer::StackTraceStr(sTrace);
      rise::LogInfo() << "trace:\n" << sTrace;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CSharedPtr test
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CSharedPtr" << std::flush;

      rise::CSharedPtr<int> p1(new int(1));
      rise::CSharedPtr<int> p2(new int(2));
      rise::CSharedPtr<int> p11(p1);

      p2.Swap(p1);

      RISE_ASSERT( p11 == p2 );
      RISE_ASSERT( p1 > p2 );
      RISE_ASSERT( p2 < p1 );
      RISE_ASSERT( p2 != p1 );

      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CByteOrder
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CByteOrder" << std::flush;
      RISE_ASSERT(rise::CByteOrder::SwapBytesN(0x12345678) == 0x78563412);
      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CStreamBuffer
    //////////////////////////////////////////////////////////////////////////////
    {
      //CStreamBuffer
    }

    //////////////////////////////////////////////////////////////////////////////
    //    XMLCommon
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLValue" << std::flush;
      rise::CString sTest = "123";
      rise::xml::CXMLValue tValue = 123;
      RISE_ASSERT(tValue == 123.0f);
      RISE_ASSERT(tValue == 123.0);
      RISE_ASSERT(tValue == (unsigned)123);
      RISE_ASSERT(tValue == "123");
      RISE_ASSERT(tValue == sTest);

      tValue = "321.0asd";
      RISE_ASSERT(tValue == 321.0f);
      RISE_ASSERT(tValue == 321.0);
      RISE_ASSERT(tValue == (unsigned)321);
      RISE_ASSERT(tValue == "321.0asd");

      RISE_ASSERT(tValue != 321.1f);

      tStdOut << rise::LogResultSuccess;
    }
    

    //////////////////////////////////////////////////////////////////////////////
    //    CBase64Encoder
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CBase64Encoder" << std::flush;

      rise::CStringA sOut;
      rise::CByteArray baTest;
      rise::CByteArray baTestResult;
      baTest.resize(256);
      for(rise::CByteArray::iterator iter = baTest.begin(); iter != baTest.end(); ++iter)
        *iter = (unsigned char)(iter - baTest.begin());

      rise::CBase64Encoder::Encode(baTest, sOut);

      rise::CBase64Encoder::Decode(sOut, baTestResult);

      RISE_ASSERT(baTestResult == baTest);
      tStdOut << rise::LogResultSuccess;
    }

    {
      tStdOut << "CBase64Encoder_1" << std::flush;
      
      rise::CString sMessage = "Тестовое приложениs123234";
      rise::CString sStrOut;
      rise::CBase64Encoder::Encode(reinterpret_cast<rise::PCBuffer>(sMessage.c_str()), sMessage.size(), sStrOut);

/*      rise::CStringA sOut;
      bool bOk = true;
      rise::TBuffer tBuffIn[20];
      rise::TBuffer tBuffOut[512];
      rise::TSize tOutSize = sizeof(tBuffOut);

      for(int i = 0; i < sizeof(tBuffIn); ++i)
        tBuffIn[i] = 0;//static_cast<rise::TBuffer>(i);

      for(int i = 0; i < 253; ++i)
        tBuffIn[i] = static_cast<rise::TBuffer>(i);

      rise::CBase64Encoder::Encode(tBuffIn, sizeof(tBuffIn), sOut);
      rise::CBase64Encoder::Decode(sOut, tBuffOut, tOutSize);

      for(int i = 0; i < sizeof(tBuffIn); ++i)
        if(tBuffIn[i] != tBuffOut[i])
        {
          bOk = false;
          break;
        }

      RISE_ASSERT(bOk);*/
      tStdOut << rise::LogResultSuccess;
    }

    {
      tStdOut << "CBase64Encoder_2" << std::flush;

      rise::CStringA sOut;
      bool bOk = true;
      rise::TBuffer tBuffIn[256];
      rise::TBuffer tBuffOut[512];
      rise::TSize tOutSize = sizeof(tBuffOut);

      for(int i = 0; i < sizeof(tBuffIn); ++i)
        tBuffIn[i] = 0;//static_cast<rise::TBuffer>(i);

      for(int i = 0; i < 253; ++i)
        tBuffIn[i] = static_cast<rise::TBuffer>(i);

      rise::CBase64Encoder::Encode(tBuffIn, sizeof(tBuffIn), sOut);
      rise::CBase64Encoder::Decode(sOut, tBuffOut, tOutSize);

      for(int i = 0; i < sizeof(tBuffIn); ++i)
        if(tBuffIn[i] != tBuffOut[i])
        {
          bOk = false;
          break;
        }

      RISE_ASSERT(bOk);
      tStdOut << rise::LogResultSuccess;
    }


    //////////////////////////////////////////////////////////////////////////////
    //    XMLNode/XMLDecl MetaSymbols
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLNode/XMLDecl MetaSymbols" << std::flush;
      
      rise::xml::CXMLDecl cXMLDecl;
      rise::xml::CXMLNode cXMLNodeRoot;

      rise::xml::CXMLFileIStream ifStream;
      ifStream.Open("test_meta.xml");
      ifStream >> cXMLDecl;
      ifStream >> cXMLNodeRoot;
      ifStream.Close();

      rise::xml::CXMLFileOStream ofStream;
      ofStream.Open("out/test_meta_out.xml");
      ofStream << cXMLDecl;
      ofStream << cXMLNodeRoot;
      ofStream.Close();

      rise::LogDebug() << cXMLDecl;

      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    XMLNode/XMLDecl Serializing std
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLNode/XMLDecl Serializing" << std::flush;

      tPerfCounter.Start();
      rise::xml::CXMLDecl cXMLDecl;
      rise::xml::CXMLNode cXMLNodeRoot;
      rise::CIFStream ifStream;
      ifStream.open("test.xml", std::ios::in | std::ios::binary);
      RISE_ASSERT(ifStream.good());
      ifStream >> cXMLDecl;
      ifStream >> cXMLNodeRoot;
      ifStream.close();

      rise::COFStream ofStream;
      ofStream.open("out/test_out.xml");
      RISE_ASSERT(ofStream.good());
      ofStream << cXMLDecl;
      ofStream << cXMLNodeRoot;
      ofStream.close();

      tPerfCounter.Finish();
      rise::LogDebug() << "(") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond) ";
      rise::LogDebug() << "Serializing (") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond)";
      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    XML Parser/Writer
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLDocument" << std::flush;
      using namespace rise;
      using namespace xml;

      CXMLDocument tDoc;

//      CXMLDecl& rXMLDecl = tDoc.GetDecl();
      CXMLNode& rXMLNodeRoot = tDoc.GetRoot();
    
      tDoc.LoadFromFile("test.xml");

      tPerfCounter.Start();
      rXMLNodeRoot.SortNodes(SNodeSort());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SNodeSort()) (") << tPerfCounter.GetLastMeasuredValue() + _T(" microsecond)";

      tPerfCounter.Start();
      rXMLNodeRoot.SortAttrs(SAttrSort());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SAttrSort()) (") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond)";

      tDoc.SaveToFile("out/test_sorted_by_name_out.xml");

      tPerfCounter.Start();
      rXMLNodeRoot.SortNodes(SNodeSortN());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SNodeSortN()) (") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond)";

      tDoc.SaveToFile("out/test_sorted_attr_value_out.xml");

      CXMLNode::TXMLAttrConstIterator itAttr = rXMLNodeRoot.FindAttributeIf(SAttrFind());

      CXMLNode::TXMLNodeConstIterator itNode = rXMLNodeRoot.FindNodeIf(SNodeFind());

      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    XMLWriter/manual xml tree creating
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLWriter" << std::flush;

      rise::xml::CXMLDocument tDoc;
//      CXMLDecl& cXMLDecl = tDoc.GetDecl();
      rise::xml::CXMLNode& cXMLNodeRoot = tDoc.GetRoot();

      cXMLNodeRoot.NodeName() = "rootNode";
      cXMLNodeRoot.Namespace() = "myns";
      cXMLNodeRoot.GetNsList().push_back(rise::xml::SXMLNamespace("myns"), "http://tempui.org/services/myservice");

      rise::xml::CXMLNode& node1 = cXMLNodeRoot.AddSubNode("myns:node1");
      /*CXMLNode& node11 = */cXMLNodeRoot.AddSubNode("myns:node11");
      rise::xml::CXMLNode& node2 = node1.AddSubNode("myns2:node2");
      node2.GetNsList().push_back(rise::xml::SXMLNamespace("myns2"), "http://tempui.org/services/myservice2");
      
      node2.AddSubNode("comment", rise::xml::CXMLNode::ENTCOMMENT);
      rise::xml::CXMLNode& nodeCData = node1.AddSubNode("some text data <1d12d12> <asd !!", rise::xml::CXMLNode::ENTCDATA);
      nodeCData.AddSubNode("test");

      node1.AddAttribute("attr1"), "1";
      node1.AddAttribute("attr2"), "2";
      node1.AddAttribute("attr3"), "3";
      node1.NodeContent() = "some value";

      tDoc.SaveToFile("out/testxmlwr_out.xml");

      const int nNodeCount = 1000;
      cXMLNodeRoot.Clear();

      tPerfCounter.Start();
      cXMLNodeRoot.NodeName() = "root";
      for (int i = 0; i < nNodeCount; ++i)
        cXMLNodeRoot.AddSubNode("subnode")).AddAttribute(_T("id", rise::ToStr(rand() % 10000));
      tPerfCounter.Finish();
      rise::LogDebug() << "AddSubNode(") << nNodeCount << ") ("  << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond)";


      tDoc.SaveToFile("out/testxml_rand_unsorted_out.xml");


      tPerfCounter.Start();
      cXMLNodeRoot.NodeName() = "root";

      cXMLNodeRoot.SortNodes(SNodeSortID());

      tPerfCounter.Finish();
      rise::LogDebug() << "AddSubNode(") << nNodeCount << ") (" << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond, " <<
          (tPerfCounter.GetLastMeasuredValue() / (double)nNodeCount) << " microsecond per node)";


      tDoc.SaveToFile("out/testxml_rand_sorted_out.xml");

      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CXMLDocument
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLNode/XMLDecl benchmark" << std::flush;
      
      tPerfCounter.Start();
      rise::xml::CXMLDocument tDoc;
      tDoc.LoadFromFile("test_meta.xml");
      tDoc.GetDecl().m_eStandalone = rise::xml::CXMLDecl::EXMLSAYes;
      tDoc.GetRoot().AddSubNode("test", rise::xml::CXMLNode::ENTCDATA);
      tDoc.SaveToFile("out/test_meta_out.xml");

      tPerfCounter.Finish();
      rise::LogDebug() << "(") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond) ";
      rise::LogDebug() << "Serializing (") << tPerfCounter.GetLastMeasuredValue() << _T(" microsecond)";

      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    {
      rise::xml::CXMLDocument tDoc;
      tDoc.LoadFromFile("Calculator.wsdl");
      tDoc.SaveToFile("out/Calculator_copy.wsdl");
    }


    //////////////////////////////////////////////////////////////////////////////
    //    CThread
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CThread" << std::flush;
      CMyThread th;
      th.Start();
      RISE_ASSERT(th.IsWorking());

/*      for (int i =0; i < 10; ++i)
      {
       rise::LogDebug1() << "MainThread. no:" << i;
      }*/

      rise::LogDebug1() << "stopping child thread";
      th.Stop(300);
      rise::LogDebug1() << "stopped";

      th.JoinThread();
      tStdOut << rise::LogResultSuccess;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    Sockets
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "Sockets" << std::flush;
      CMyServerSocket cServerSocket;
      rise::ulong ulSent = 0;
      rise::CStreamBuffer cBuffer;
      rise::sockets::CClientSocket cClientSocket;
      cServerSocket.Start();
      
      rise::threading::CThread::Sleep(1000); // подождать пока сервер застартует...

      cClientSocket.Create();
      cClientSocket.Connect("127.0.0.1", 8023);
      cBuffer << "This is a test string for sockets. num: " << 777 << 2.0;
      cClientSocket.Send(cBuffer, &ulSent);
//      cServerSocket.JoinThread();
//      cServerSocket.m_pClient.JoinThread();
      tStdOut << rise::LogResultSuccess;

      cServerSocket.JoinThread();
    }


    //////////////////////////////////////////////////////////////////////////////
    //    plugin system
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "plugin system" << std::flush;

      rise::CString sPluginName = "myplugin";
      typedef rise::plugin::CPluginManager<plugin_template::CPluginTemplate> CMyPluginManager;
      CMyPluginManager tPluginManager;
      plugin_template::CPluginTemplate* pPlugin = tPluginManager.LoadPlugin(sPluginName);

      tStdOut << "PluginResult=" << pPlugin->Sum(2, 3);

      tPluginManager.UnLoadPlugin(sPluginName);

      tStdOut << rise::LogResultSuccess;
    }

  }
  RISE_CATCH_ALL_DESCR_ACTION("\nошибка при тестировании!", tStdOut << rise::LogResultFailed);

  return 0;
}
