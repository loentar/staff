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

#include <rise/os/osdllexport.h>

#include <iostream>
#include <fstream>
#include <strstream>
#include <stdlib.h>
#include <locale.h>
#include <iomanip>
#ifdef WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif


// #include <rise/common/Exception.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/common/Log.h>
#include <rise/common/types.h>
#include <rise/common/DataEncoder.h>
#include <rise/common/PerformanceCounter.h>
#include <rise/common/SharedPtr.h>
#include <rise/xml/XMLFileStream.h>
#include <rise/xml/XMLNode.h>
#include <rise/xml/XMLDecl.h>
#include <rise/xml/XMLDocument.h>
#include <rise/threading/Thread.h>
#include <rise/threading/Event.h>
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
#include "../../myplugin/src/MyPlugin.h"

static void OnSignal(int nSignal)
{
  if(nSignal == SIGSEGV)
  {
    std::string sTracedStack;
    rise::tools::CStackTracer::StackTraceStr(sTracedStack);
    std::cout << "\n\n";
    rise::LogError() << "Segmentation fault in thread " << rise::threading::CThread::GetCurrentId()
        << ".\nTraced stack:\n" << sTracedStack;
    exit(1);
  }
}

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

class CMyThread1: public rise::threading::CThread 
{
public:
  void Run(void* /*pparam*/)
  {
    rise::LogDebug1() << "ChildThread. start";
    Sleep(1000);
    rise::LogDebug1() << "ChildThread. end";
  }
};

class CMyTimer: public rise::CTimerHandler
{
public:
  CMyTimer():
    m_nCount(0)
  {
  }

  void OnTimer( rise::word wID )
  {
    ++m_nCount;
    rise::LogDebug() << "OnTimer: " << wID;
  }

  int GetCount()
  {
    return m_nCount;
  }

private:
  int m_nCount;
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
        {
          if(!CRecvSocket::Recv(cBuffer))
          {
            rise::LogWarning() << "can't recv";
          }
          else
          {
            rise::CString sString;
            int nCode;
            cBuffer >> sString >> nCode;

           rise::LogDebug1() << "Received: " << sString << nCode;
          }
        }
      }
      RISE_CATCH_ALL_DESCR("exception will raised, thread " << CThread::GetId() << " will now exit");
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
          return rNode1.Attribute("n").AsString() < rNode2.Attribute("n").AsString();
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
    rise::LogDebug1() << "Signal #" << nSignal << "(" << GetSignalStr(nSignal) << "";
  }

} tSigTest;



int main(int argc, const rise::TChar* argv[])
{
#ifndef WIN32
  signal(SIGSEGV, OnSignal);
#endif

  tStdOut << rise::ConsoleSetWindowText("rise test");

  std::ofstream fLog; // custom log: variable must be in log scope!!!!!

  rise::CPerformanceCounter tPerfCounter;

//   {
//     rise::process::CProcess tProcTest;
//     tProcTest.Exec("notepad\tconfig.xml");
//   }

  bool bLogToFile = false;

   if ( argc > 1 )
     if ( rise::CString(argv[1]) == "-logtofile" )
       bLogToFile = true;

/*  rise::tLog.SetLogLevel(rise::CLog::ELL_INFO);*/

#ifdef WIN32
  _mkdir("out");
#else
  mkdir("out", 0777);
#endif

  if (!bLogToFile)
  {
    fLog.open("out/rise_test.log", std::ios::out | std::ios::binary);
//    rise::tLog.SetLogLevel(rise::CLog::ELL_DEBUG3);
//   rise::Log.SetLogStream(&fLog, rise::CLog::ELL_INFO | rise::CLog::ELL_DEBUG | rise::CLog::ELL_DEBUG1 | rise::CLog::ELL_DEBUG2 | rise::CLog::ELL_DEBUG3);
//   rise::Log.SetLogStream(&fLog, rise::CLog::ELL_ALL);
  }


  //////////////////////////////////////////////////////////////////////////////
  //    testing std c++ linking to rise
  //////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
  {
    try 
    {
      tStdOut << "rise runtime link test";
      {
        std::string sRes;
        rise::RiseLinkRuntimeTest(sRes);
      }
      tStdOut << rise::LogResultSuccess << std::endl;
    } catch(...)
    {
      rise::LogError() << rise::LogResultFailed;
      abort();
    };
  }
#endif


  try
  {
    {
      tStdOut << "Test timer";

      CMyTimer tTimer1;
      CMyTimer tTimer2;
      tTimer1.AddTimer(100, 2);
      tTimer2.AddTimer(50, 3);

      for (int i = 0; i < 5; ++i)
      {
        rise::threading::CThread::Sleep(110);
      }

      RISE_ASSERT(tTimer1.GetCount() == 2 && tTimer2.GetCount() == 3);

      tStdOut << rise::LogResultSuccess << std::endl;
    }

    {
      tStdOut << "Testing CS ";
      CTestCS tTestCS;
      tTestCS.TestCrit();
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    {
      tStdOut << "CEncoding::Convert" << std::flush;

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

      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FileFind
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "FileFind" << std::flush;
      rise::CStringList slsTest;
      rise::CFileFind::Find(".", slsTest, "data/*.xml", rise::CFileFind::EFA_FILE);
      rise::CFileFind::Find(".", slsTest, "data/*", rise::CFileFind::EFA_DIR);
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    rise::CString test
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CString... ";

      // basic test ////////////////////////////////////////////////////////////
      rise::CString ss = rise::CString("asdqwe");
      RISE_ASSERTS( ss == "asdqwe", "CString 0");

      RISE_ASSERTS( rise::CString("test") == "test", "CString 1");
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
      rise::StrReplace(str, "9", "<_+>");
      RISE_ASSERTS( str == "12345678<_+>", "replace 1");

      rise::StrReplace(str, "1", "");
      RISE_ASSERTS( str == "2345678<_+>", "replace 2");

      rise::StrReplace(str, "5", " ");
      RISE_ASSERTS( str == "234 678<_+>", "replace 2");
      
      //////////////////////////////////////////////////////////////////////////
      // Trim
      str = "  11 23";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23", "Trim1");

      str = "11 23  ";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23", "Trim2");

      str = "  11 23  ";
      rise::StrTrim(str);
      RISE_ASSERTS( str == "11 23", "Trim3");

      //////////////////////////////////////////////////////////////////////////
      // left/right/mid test
      str = "123456";

      //////////////////////////////////////////////////////////////////////////
      // Left
      RISE_ASSERTS( rise::StrLeft(str, 3) == "123", "Left1");
      RISE_ASSERTS( rise::StrLeft(str, 0) == "", "Left2");
      RISE_ASSERTS( rise::StrLeft(str, rise::CString::npos) == "123456", "Left2");

      //////////////////////////////////////////////////////////////////////////
      // Right
      RISE_ASSERTS( rise::StrRight(str, 3) == "456", "Right1");
      RISE_ASSERTS( rise::StrRight(str, 0) == "", "Right2");

      //////////////////////////////////////////////////////////////////////////
      // Mid
      RISE_ASSERTS( rise::StrMid(str, 2, 3) == "345", "Mid1");
      RISE_ASSERTS( rise::StrMid(str, 2) == "3456", "Mid2");

      //////////////////////////////////////////////////////////////////////////
      // Find
    //    FromStr
      int nVal = 0;
      rise::FromStr("123", nVal);
      RISE_ASSERTS(nVal == 123, "FromStr!");

    //    ToStr
      rise::CString sRes;
      rise::ToStr(123, sRes);
      RISE_ASSERTS(sRes == "123", "ToStr1!");
      RISE_ASSERTS(rise::ToStr(123) == "123", "ToStr2!");
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    rise::CProcess test
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CProcess:GetCurrentExecPath" << std::flush;
      rise::CString sPath = rise::process::CProcess::GetCurrentExecPath();
      RISE_ASSERTS(sPath != "", "GetCurrentExecPath");
      rise::LogDebug() << sPath;
      tStdOut << rise::LogResultSuccess << std::endl;
    }

#ifndef OS_FreeBSD
    {
      tStdOut << "StackTracer" << std::flush;
      rise::CString sTrace;
      rise::tools::CStackTracer::StackTraceStr(sTrace);
      rise::LogDebug() << "trace:\n" << sTrace;
      RISE_ASSERTS(sTrace != "", "StackTraceStr");
      tStdOut << rise::LogResultSuccess << std::endl;
    }
#endif

    //////////////////////////////////////////////////////////////////////////////
    //    CSharedPtr test
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CSharedPtr" << std::flush;

      rise::CSharedPtr<int> p1(new int(1));
      rise::CSharedPtr<int> p2(new int(2));

      RISE_ASSERT(p1.GetRefCount() == 1);
      RISE_ASSERT(p1.IsUnique());
      {
        rise::CSharedPtr<int> p11(p1);
        RISE_ASSERT(*p11 == 1);
        RISE_ASSERT(*p11 == *p1);
        RISE_ASSERT(p11.GetRefCount() == 2);
        RISE_ASSERT(p1.GetRefCount() == 2);
        RISE_ASSERT(!p1.IsUnique());

      }
      RISE_ASSERT(p1.GetRefCount() == 1);
      RISE_ASSERT(p1.IsUnique());

      p2.Swap(p1);
      RISE_ASSERT(*p1 == 2);

      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CByteOrder
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CByteOrder" << std::flush;
      RISE_ASSERT(rise::CByteOrder::SwapBytesN(0x12345678) == 0x78563412);
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CStreamBuffer
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CStreamBuffer" << std::flush;
      rise::CStreamBuffer sb1;
      rise::CStreamBuffer sb2;

      RISE_ASSERT(sb1 == sb2);
     
      sb1 << "asd" << 1 << 2.0;
      
      std::string a;
      int b = 0;
      double c = 0.0;

      RISE_ASSERT(sb1 != sb2);
      
      sb1 >> a >> b >> c;
      
      RISE_ASSERT(a == "asd" );
      RISE_ASSERT(b == 1 );
      RISE_ASSERT(c == 2.0 );
      
      RISE_ASSERT(sb1 == sb2);


      sb1 << "asd" << 1 << 2.0;
      sb2 << "asd" << 1 << 2.0;
      
      RISE_ASSERT(sb1 == sb2);
      
      tStdOut << rise::LogResultSuccess << std::endl;
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

      tStdOut << rise::LogResultSuccess << std::endl;
    }
    

    //////////////////////////////////////////////////////////////////////////////
    //    CBase64Encoder
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CBase64Encoder" << std::flush;

      rise::CStringA sOut;
      rise::CStreamBuffer baTest;
      rise::CStreamBuffer baTestResult;

      for(int i = 0; i < 256; ++i)
      {
        baTest << static_cast<rise::byte>(i);
      }

      rise::CBase64Encoder::Encode(baTest, sOut);

      rise::CBase64Encoder::Decode(sOut, baTestResult);

      RISE_ASSERT(baTestResult == baTest);
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    {
      tStdOut << "CBase64Encoder 2" << std::flush;

      rise::CStringA sOut;
      bool bOk = true;
      rise::TBuffer tBuffIn[256];
      rise::TBuffer tBuffOut[512];
      rise::TSize tOutSize = sizeof(tBuffOut);

      for(rise::TSize i = 0; i < sizeof(tBuffIn); ++i)
        tBuffIn[i] = 0;//static_cast<rise::TBuffer>(i);

      for(rise::TSize i = 0; i < 253; ++i)
        tBuffIn[i] = static_cast<rise::TBuffer>(i);

      rise::CBase64Encoder::Encode(tBuffIn, sizeof(tBuffIn), sOut);
      rise::CBase64Encoder::Decode(sOut, tBuffOut, tOutSize);

      for(rise::TSize i = 0; i < sizeof(tBuffIn); ++i)
        if(tBuffIn[i] != tBuffOut[i])
        {
          bOk = false;
          break;
        }

      RISE_ASSERT(bOk);
      tStdOut << rise::LogResultSuccess << std::endl;
    }


    //////////////////////////////////////////////////////////////////////////////
    //    XMLNode/XMLDecl MetaSymbols
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLNode/XMLDecl MetaSymbols" << std::flush;
      
      rise::xml::CXMLDecl cXMLDecl;
      rise::xml::CXMLNode cXMLNodeRoot;

      rise::xml::CXMLFileIStream ifStream;
      ifStream.Open("data/test_meta.xml");
      ifStream >> cXMLDecl;
      ifStream >> cXMLNodeRoot;
      ifStream.Close();

      rise::xml::CXMLFileOStream ofStream;
      ofStream.Open("out/test_meta_out.xml");
      ofStream << cXMLDecl;
      ofStream << cXMLNodeRoot;
      ofStream.Close();

      rise::LogDebug() << cXMLDecl;

      tStdOut << rise::LogResultSuccess << std::endl;
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
      ifStream.open("data/test.xml", std::ios::in | std::ios::binary);
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
      rise::LogDebug() << "(" << tPerfCounter.GetLastMeasuredValue() << " microsecond) ";
      rise::LogDebug() << "Serializing (" << tPerfCounter.GetLastMeasuredValue() << " microsecond)";
      tStdOut << rise::LogResultSuccess << std::endl;
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
    
      tDoc.LoadFromFile("data/test.xml");

      tPerfCounter.Start();
      rXMLNodeRoot.SortNodes(SNodeSort());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SNodeSort()) (" << tPerfCounter.GetLastMeasuredValue() + " microsecond)";

      tPerfCounter.Start();
      rXMLNodeRoot.SortAttrs(SAttrSort());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SAttrSort()) (" << tPerfCounter.GetLastMeasuredValue() << " microsecond)";

      tDoc.SaveToFile("out/test_sorted_by_name_out.xml");

      tPerfCounter.Start();
      rXMLNodeRoot.SortNodes(SNodeSortN());
      tPerfCounter.Finish();
      rise::LogDebug() << "SortNodes(SNodeSortN()) (" << tPerfCounter.GetLastMeasuredValue() << " microsecond)";

      tDoc.SaveToFile("out/test_sorted_attr_value_out.xml");

      CXMLNode::TXMLAttrConstIterator itAttr = rXMLNodeRoot.FindAttributeIf(SAttrFind());
      RISE_ASSERT(itAttr != rXMLNodeRoot.AttrEnd());

      /*CXMLNode::TXMLNodeConstIterator itNode =*/ rXMLNodeRoot.FindNodeIf(SNodeFind());
//      RISE_ASSERT(itNode != rXMLNodeRoot.NodeEnd());

      tStdOut << rise::LogResultSuccess << std::endl;
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
      cXMLNodeRoot.GetNsList().push_back(rise::xml::SXMLNamespace("myns", "http://tempui.org/services/myservice"));

      rise::xml::CXMLNode& node1 = cXMLNodeRoot.AddSubNode("myns:node1");
      /*CXMLNode& node11 = */cXMLNodeRoot.AddSubNode("myns:node11");
      rise::xml::CXMLNode& node2 = node1.AddSubNode("myns2:node2");
      node2.GetNsList().push_back(rise::xml::SXMLNamespace("myns2", "http://tempui.org/services/myservice2"));
      
      node2.AddSubNode("comment", rise::xml::CXMLNode::ENTCOMMENT);
      rise::xml::CXMLNode& nodeCData = node1.AddSubNode("some text data <1d12d12> <asd !!", rise::xml::CXMLNode::ENTCDATA);
      nodeCData.AddSubNode("test");

      node1.AddAttribute("attr1", "1");
      node1.AddAttribute("attr2", "2");
      node1.AddAttribute("attr3", "3");
      node1.NodeContent() = "some value";

      tDoc.SaveToFile("out/testxmlwr_out.xml");

      const int nNodeCount = 1000;
      cXMLNodeRoot.Clear();

      tPerfCounter.Start();
      cXMLNodeRoot.NodeName() = "root";
      for (int i = 0; i < nNodeCount; ++i)
        cXMLNodeRoot.AddSubNode("subnode").AddAttribute("id", rise::ToStr(rand() % 10000));
      tPerfCounter.Finish();
      rise::LogDebug() << "AddSubNode(" << nNodeCount << ") ("  << tPerfCounter.GetLastMeasuredValue() << " microsecond)";


      tDoc.SaveToFile("out/testxml_rand_unsorted_out.xml");


      tPerfCounter.Start();
      cXMLNodeRoot.NodeName() = "root";

      cXMLNodeRoot.SortNodes(SNodeSortID());

      tPerfCounter.Finish();
      rise::LogDebug() << "AddSubNode(" << nNodeCount << ") (" << tPerfCounter.GetLastMeasuredValue() << " microsecond, " <<
          (tPerfCounter.GetLastMeasuredValue() / (double)nNodeCount) << " microsecond per node)";


      tDoc.SaveToFile("out/testxml_rand_sorted_out.xml");

      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CXMLDocument
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "XMLNode/XMLDecl benchmark" << std::flush;
      
      tPerfCounter.Start();
      rise::xml::CXMLDocument tDoc;
      tDoc.LoadFromFile("data/test_meta.xml");
      tDoc.GetDecl().m_eStandalone = rise::xml::CXMLDecl::EXMLSAYes;
      tDoc.GetRoot().AddSubNode("test", rise::xml::CXMLNode::ENTCDATA);
      tDoc.SaveToFile("out/test_meta_out.xml");

      tPerfCounter.Finish();
      rise::LogDebug() << "(" << tPerfCounter.GetLastMeasuredValue() << " microsecond) ";
      rise::LogDebug() << "Serializing (" << tPerfCounter.GetLastMeasuredValue() << " microsecond)";

      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    {
      rise::xml::CXMLDocument tDoc;
      tDoc.LoadFromFile("data/Calculator.wsdl");
      tDoc.SaveToFile("out/Calculator_copy.wsdl");
    }


    //////////////////////////////////////////////////////////////////////////////
    //    CThread
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CThread(Stop)" << std::flush;
      CMyThread th;
      th.Start();
      RISE_ASSERT(th.IsWorking());

      rise::LogDebug1() << "stopping child thread";
      th.Stop(300);
      rise::LogDebug1() << "stopped";

      th.JoinThread();
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CThread
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CThread(Wait)" << std::flush;
      CMyThread1 th;
      th.Start();
      RISE_ASSERT(th.IsWorking());
      
      rise::threading::CThread::Sleep(100);

      rise::LogDebug1() << "Joining thread";

      th.Wait();
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CThread
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CThread(Limited Wait)" << std::flush;
      CMyThread1 th;
      th.Start();
      RISE_ASSERT(th.IsWorking());
      
      rise::threading::CThread::Sleep(100);

      rise::LogDebug1() << "Joining thread";

      RISE_ASSERT(th.Wait(2100));
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CThread
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "CThread(Join)" << std::flush;
      CMyThread1 th;
      th.Start(NULL, true);
      RISE_ASSERT(th.IsWorking());
      
      rise::threading::CThread::Sleep(100);

      rise::LogDebug1() << "Joining thread";

      th.JoinThread();
      tStdOut << rise::LogResultSuccess << std::endl;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CEvent
    //////////////////////////////////////////////////////////////////////////////

    {
      struct SEventTest
      {
        rise::threading::CEvent tEvent;
        bool bSignaled;
        SEventTest(): bSignaled(false) {}
      };

      class CMyThreadEvent: public rise::threading::CThread
      {
      public:
        void Run(void* pParam)
        {
          SEventTest* pEventTest = reinterpret_cast<SEventTest*>(pParam);
          RISE_ASSERT(pEventTest);
          Sleep(200);
          pEventTest->bSignaled = true;
          pEventTest->tEvent.Signal();
        }
      };

      { // test timeout
        tStdOut << "CEvent timeout infinite" << std::flush;
        CMyThreadEvent th;
        SEventTest tEventTest;
        th.Start(&tEventTest);

        RISE_ASSERT(tEventTest.tEvent.Wait());
        RISE_ASSERT(tEventTest.bSignaled);

        th.Wait(100);

        tStdOut << rise::LogResultSuccess << std::endl;
      }

      { // test signaled state
        tStdOut << "CEvent wait limited" << std::flush;
        CMyThreadEvent th;
        SEventTest tEventTest;
        th.Start(&tEventTest);

        RISE_ASSERT(tEventTest.tEvent.Wait(300));
        RISE_ASSERT(tEventTest.bSignaled);

        th.Wait(100);

        tStdOut << rise::LogResultSuccess << std::endl;
      }


      { // test timeout
        tStdOut << "CEvent timeout" << std::flush;
        CMyThreadEvent th;
        SEventTest tEventTest;
        th.Start(&tEventTest);

        RISE_ASSERT(!tEventTest.tEvent.Wait(100));
        RISE_ASSERT(!tEventTest.bSignaled);

        th.Wait(100);

        tStdOut << rise::LogResultSuccess << std::endl;
      }
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

      rise::threading::CThread::Sleep(1000); // wait while server starts...

      cClientSocket.Create();
      cClientSocket.Connect("127.0.0.1", 8023);
      cBuffer << "This is a test string for sockets. num: " << 777 << 2.0;
      cClientSocket.Send(cBuffer, &ulSent);
//      cServerSocket.JoinThread();
//      cServerSocket.m_pClient.JoinThread();
      tStdOut << rise::LogResultSuccess << std::endl;

      cServerSocket.JoinThread();
    }


#ifndef WIN32 // temporary disabled for win
    //////////////////////////////////////////////////////////////////////////////
    //    plugin system
    //////////////////////////////////////////////////////////////////////////////
    {
      tStdOut << "plugin system" << std::flush;

      rise::CString sPluginName = "MyPlugin";

      typedef rise::plugin::CPluginManager<myplugin::CMyPlugin> CMyPluginManager;

      CMyPluginManager tPluginManager;
      myplugin::CMyPlugin* pPlugin = tPluginManager.LoadPlugin(sPluginName);

      int nResult = pPlugin->Sum(2, 3);
      rise::LogDebug() << "PluginResult: Sum(2,3) = " << nResult;

      tPluginManager.UnLoadPlugin(sPluginName);

      RISE_ASSERT(nResult == 5);
      tStdOut << rise::LogResultSuccess << std::endl;
    }
#endif

  }
  RISE_CATCH_ALL_DESCR_ACTION("\nError while testing!", tStdOut << rise::LogResultFailed << "\n\n");

  return 0;
}
