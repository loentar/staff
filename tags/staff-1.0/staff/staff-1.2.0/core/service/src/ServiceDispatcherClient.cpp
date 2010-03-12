#include <algorithm>
#include <rise/socket/ClientSocket.h>
#include <rise/common/Log.h>
#include <rise/common/StreamBuffer.h>
#include <rise/threading/Thread.h>
#include <rise/signal/SignalHandler.h>
#include <rise/xml/XMLNode.h>
#include <staff/common/Config.h>
#include <staff/common/Value.h>
#include <staff/common/DataObject.h>
#include <staff/common/DataObjectHelper.h>
#include <staff/common/Operation.h>
#include "ServiceDispatcherClient.h"
#include "ServiceWrapper.h"

namespace staff
{
  class CServiceDispatcherClient::CServiceDispatcherClientImpl: public CServiceDispatcherClient, 
                                                                public rise::threading::CThread, 
                                                                private rise::CSignalHandler
  {
  public:
    CServiceWrapper*              m_pServiceWrapper;
    rise::sockets::CClientSocket  m_tSocket;
    bool                          m_bGeneralFault;

  public:
    CServiceDispatcherClientImpl():
      m_pServiceWrapper(NULL),
      m_bGeneralFault(false)
    {
    }

    virtual void SignalHandler(int nSignal)
    {
      try
      {
        rise::LogDebug1() << "Signal Received: " << GetSignalStr(nSignal) << "  : CurrentthreadID = " << GetCurrentId() << "; CThread id = " << GetId();
        if(nSignal == ESIGINT)
        {
          if(IsWorking() && GetId() == CThread::GetCurrentId())
          {
            rise::LogDebug1() << "Stopping thread";
            CThread::Stop();
          }
          else
          {
            m_tSocket.Close();
            rise::LogDebug() << "waiting for thread exit";
            while(IsWorking())
              Sleep(10);

            m_bGeneralFault = true;
          }
        }
      }
      RISE_CATCH_ALL
    }
    
    virtual void Run(void* = NULL)
    {
      rise::CStreamBuffer tBufferIn;
      rise::CStreamBuffer tBufferOut;
      const rise::xml::CXMLNode& rXmlConfig = CConfig::Inst().GetModuleConfig("ServiceDispatcher");
      rise::ushort ushPort = rXmlConfig["DispatcherPort"];
      rise::ulong  ushReconnectDelay = rXmlConfig["ReconnectDelay"];

      RegisterSignal(ESIGINT);

      tBufferIn.Reserve(4096);
      tBufferOut.Reserve(40960);

      m_tSocket.Create();

      RISE_ASSERT(m_pServiceWrapper);

      try
      {
        while (!IsStopping() && !m_bGeneralFault)
        {
          rise::LogInfo() << "connecting to dispatcher...\n";
          while(!m_tSocket.Connect("127.0.0.1", ushPort) && !IsStopping())
          {
            if(m_bGeneralFault)
            {
              m_tSocket.Close();
              return;
            }
            CThread::Sleep(ushReconnectDelay);
          }
  
          rise::LogNotice() << "SessionId:[" << m_pServiceWrapper->GetSessionId() << "] connected to dispatcher. waiting for requests\n";
  
          while (!IsStopping())
          {
            if(m_bGeneralFault)
            {
              m_tSocket.Close();
              return;
            }
            
            if(!ReceiveMessage(tBufferIn))
            {
              m_tSocket.Close();
              rise::LogWarning() << "connection closed. trying to reconnect...\n";
              Sleep(ushReconnectDelay);
              m_tSocket.Create();
              break;
            }
            Invoke(tBufferIn, tBufferOut);
            SendMessage(tBufferOut);
          }
        }
      }
      catch(const rise::CFileIOException& rEx) // waitforinfinite+<Ctrl+C>
      {
        rise::LogDebug() << rEx.GetString();
      }
      RISE_CATCH_ALL
      
      m_tSocket.Close();
    }

    bool ReceiveMessage(rise::CStreamBuffer& tBuffer)
    {
      rise::ulong ulCurrentSize = 0;
      rise::ulong ulReceived = tBuffer.GetSize();

      tBuffer.Normalize();
      while (!IsStopping())
      {
        if(memchr(tBuffer.GetData() + ulCurrentSize, '\0', ulReceived) != NULL)
          return true;

        ulCurrentSize += ulReceived;

        m_tSocket.WaitForData();
        if(!m_tSocket.Recv(tBuffer, true, &ulReceived))
          break;

#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "received " << ulReceived << " bytes/(" << (ulCurrentSize + ulReceived) << ")\n";
#endif

      }
      return false;
    }

    void SendMessage(rise::CStreamBuffer& tBuffer)
    {    
      rise::ulong ulSent = 0;
      rise::ulong ulBuffPos = 0;
      rise::ulong ulDataSize = tBuffer.GetSize();

      while (ulDataSize > 0)
      {
#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "sending " << ulDataSize << " bytes...\n";
#endif
        m_tSocket.Send(tBuffer.GetData() + ulBuffPos, ulDataSize, &ulSent);
#if defined DEBUG || defined _DEBUG
        rise::LogDebug2() << "sent " << ulSent << " bytes...\n";
#endif
        ulBuffPos += ulSent;
        ulDataSize -= ulSent;
      }
    }

    void Invoke(rise::CStreamBuffer& rBufferIn, rise::CStreamBuffer& rBufferOut)
    {
      COperation tOperation;

#if defined DEBUG || defined _DEBUG
      rise::LogDebug3() << "Received Request: \n" << reinterpret_cast<const char*>(rBufferIn.GetData());
#endif
      rBufferIn >> tOperation.Request();

      if (tOperation.Request().Name() == "Fault")
      {
        rise::LogError() << "General Fault received: " << tOperation.Request().Value().AsString();
        tOperation.ResultValue() = "ok";
        m_bGeneralFault = true;
      } else
      if (m_pServiceWrapper != NULL)
      {
        try
        {
#if defined DEBUG || defined _DEBUG
          rise::LogDebug1() << "Invoking operation: \"" << tOperation.GetName() << "\"";
#endif
          m_pServiceWrapper->Invoke(tOperation);
        }
        catch (const rise::CException& rEx)
        {
          tOperation.SetFault("Failed to invoke operation", rEx.GetDescr());
        }
        catch(...)
        {
          tOperation.SetFault("Failed to invoke operation", "Unknown exception raised while executing request!");
        }
#if defined DEBUG || defined _DEBUG
        if (tOperation.IsFault())
          rise::LogDebug1() << "Fault: \"" << tOperation.GetFaultString() << "\"";
        else
          rise::LogDebug1() << "ResultValue: \"" << tOperation.ResultValue().AsString() << "\"";
#endif
      }
      else
      {
        tOperation.SetFault("Failed to invoke operation", "Request-handler is not set!");
      }

#if defined DEBUG || defined _DEBUG
          rise::LogDebug1() << "Preparing Result...";
#endif
      tOperation.PrepareResult();

      rBufferOut.Reset();
      rBufferOut << tOperation.Result();

#if defined DEBUG || defined _DEBUG
      rise::LogDebug3() << "Sending result: \n" << reinterpret_cast<const char*>(rBufferOut.GetData());
#endif
    }

    void StartThread()
    {
      CThread::Start();
    }

    void StopThread()
    {
      CThread::Stop();
    }
  };


  CServiceDispatcherClient::CServiceDispatcherClient()
  {
  }

  CServiceDispatcherClient& CServiceDispatcherClient::ServiceDispatcherClient()
  {
    if (m_pImpl == NULL)
      m_pImpl = new CServiceDispatcherClientImpl;

    return *m_pImpl;
  }

  void CServiceDispatcherClient::SetWrapper( CServiceWrapper& rServiceWrapper )
  {
    m_pImpl->m_pServiceWrapper = &rServiceWrapper;
  }

  void CServiceDispatcherClient::Start( bool bSingleThread /*= true*/ )
  {
    if (bSingleThread)
      m_pImpl->Run();
    else
      m_pImpl->StartThread();
  }

  void CServiceDispatcherClient::Stop()
  {
    m_pImpl->StopThread();
  }


  CServiceDispatcherClient::CServiceDispatcherClientImpl* CServiceDispatcherClient::m_pImpl = NULL;
}
