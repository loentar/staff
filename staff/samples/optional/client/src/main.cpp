// This file generated by staff_codegen
// For more information please visit: http://code.google.com/p/staff/
// Client skeleton

#include <memory>
#include <rise/common/Log.h>
#include <rise/threading/Thread.h>
#include <staff/common/DataObject.h>
#include <staff/client/ICallback.h>
#include <staff/common/Exception.h>
#include <staff/client/ServiceFactory.h>
#include "Tasks.h"

template <typename Type>
std::ostream& operator<<(std::ostream& rStream, const staff::Optional<Type>& rOpt)
{
  if (rOpt.IsNull())
  {
    rStream << "(not set)";
  }
  else
  {
    rStream << *rOpt;
  }
  return rStream;
}

std::ostream& operator<<(std::ostream& rStream, const ::samples::optional::AttachInfo& rAttach)
{
  return rStream << "sTitle = " << rAttach.sTitle << "; sFileName = " << rAttach.sFileName << "; nFileSize = " << rAttach.nFileSize;
}

std::ostream& operator<<(std::ostream& rStream, const ::samples::optional::Task& rTask)
{
  return rStream << "id = " << rTask.nId << "; descr = \"" << rTask.sDescr
    << "\"; tnOwnerId = " << rTask.tnOwnerId << "; tstAttachInfo = {" << rTask.tstAttachInfo << "}";
}

template <typename Type>
std::ostream& operator<<(std::ostream& rStream, const std::list<Type>& rList)
{
  for (typename std::list<Type>::const_iterator it = rList.begin(); it != rList.end(); ++it)
  {
    rStream << *it << std::endl;
  }

  return rStream << std::endl;
}


// callbacks
// callback for Tasks::Add
class TasksAddCallback: public staff::ICallback< int >
{
public:
  virtual void OnComplete(int tResult)
  {
//     process result here
     rise::LogInfo() << "Add(asynch) result: " << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    rise::LogError() << rFault.ToString();
  }
};

// callback for Tasks::UpdateOwner
class TasksUpdateOwnerCallback: public staff::ICallback< void >
{
public:
  virtual void OnComplete()
  {
    // process result here
    rise::LogInfo() << "UpdateOwner(asynch) result: ";// << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    rise::LogError() << rFault.ToString();
  }
};

// callback for Tasks::GetAttachInfo
class TasksGetAttachInfoCallback: public staff::ICallback< staff::Optional< ::samples::optional::AttachInfo > >
{
public:
  virtual void OnComplete(staff::Optional< ::samples::optional::AttachInfo > tResult)
  {
    // process result here
    rise::LogInfo() << "GetAttachInfo(asynch) result: " << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    rise::LogError() << rFault.ToString();
  }
};

// callback for Tasks::EchoOpt
class TasksEchoOptCallback: public staff::ICallback< staff::Optional< std::list< std::string >  > >
{
public:
  virtual void OnComplete(staff::Optional< std::list< std::string >  > tResult)
  {
    // process result here
    rise::LogInfo() << "EchoOpt(asynch) result: " << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    rise::LogError() << rFault.ToString();
  }
};

// callback for Tasks::GetAllTasks
class TasksGetAllTasksCallback: public staff::ICallback< ::samples::optional::TasksList >
{
public:
  virtual void OnComplete(::samples::optional::TasksList tResult)
  {
    // process result here
    rise::LogInfo() << "GetAllTasks(asynch) result: " << tResult;
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    rise::LogError() << rFault.ToString();
  }
};



int main(int /*nArgs*/, const char* /*paszArgs*/[])
{
  try
  {
    std::auto_ptr< ::samples::optional::Tasks > pTasks(::staff::ServiceFactory::Inst().GetService< ::samples::optional::Tasks >());

    RISE_ASSERTS(pTasks.get(), "Cannot get client for service samples.optional.Tasks!");

    // Invoke Your service here:

    ::samples::optional::Task rstTask;
    rstTask.sDescr = "only descr";
    int tAddResult = pTasks->Add(rstTask);
    rise::LogInfo() << "Add result: " << tAddResult;

    rstTask.tnOwnerId = 123;
    rstTask.sDescr = "with owner id";
    tAddResult = pTasks->Add(rstTask);
    rise::LogInfo() << "Add result: " << tAddResult;

    rise::LogInfo() << "UpdateOwner called";

    ::samples::optional::AttachInfo stAttachInfo;
    stAttachInfo.sTitle = "some file";
    stAttachInfo.sFileName = "file1.txt";
    stAttachInfo.nFileSize = 150;

    ::samples::optional::TasksList tGetAllTasksResult = pTasks->GetAllTasks();
    rise::LogInfo() << "GetAllTasks result: \n" << tGetAllTasksResult;

    pTasks->UpdateAttachInfo(tAddResult, stAttachInfo);
    rise::LogInfo() << "UpdateAttachInfo called";

    // staff::Optional< ::samples::optional::AttachInfo > tGetAttachInfoResult = pTasks->GetAttachInfo(nTaskId);
    // rise::LogInfo() << "GetAttachInfo result: " << tGetAttachInfoResult;

    staff::Optional< std::list< std::string >  > opt;
    opt->push_back("test");
    staff::Optional< std::list< std::string >  > tEchoOptResult = pTasks->EchoOpt(opt);
    rise::LogInfo() << "EchoOpt result: " << tEchoOptResult;

    staff::Optional< std::list< std::string >  > tEchoOptResult1 = pTasks->EchoOpt(staff::Optional< std::list< std::string >  >());
    rise::LogInfo() << "EchoOpt(none) result: " << tEchoOptResult;

    tGetAllTasksResult = pTasks->GetAllTasks();
    rise::LogInfo() << "GetAllTasks result: \n" << tGetAllTasksResult;


    // // Wait for asynch call is completed
    // // Please note, that you cannot call any operation of this client
    // // until this asynchronous request is completed.
    // // To call operation while request is processing, please use another copy of client.
    // while (!tTasksAddCallback.IsCompleted())
    // {
    //   rise::threading::CThread::Sleep(1000);
    // }


    // TasksUpdateOwnerCallback tTasksUpdateOwnerCallback;
    // pTasks->UpdateOwner(nTaskId, rtnOwnerId, tTasksUpdateOwnerCallback);

    // // Wait for asynch call is completed
    // // Please note, that you cannot call any operation of this client
    // // until this asynchronous request is completed.
    // // To call operation while request is processing, please use another copy of client.
    // while (!tTasksUpdateOwnerCallback.IsCompleted())
    // {
    //   rise::threading::CThread::Sleep(1000);
    // }


    // pTasks->UpdateAttachInfo(nTaskId, rtnAttachInfo);
    // rise::LogInfo() << "UpdateAttachInfo called";

    // TasksGetAttachInfoCallback tTasksGetAttachInfoCallback;
    // pTasks->GetAttachInfo(nTaskId, tTasksGetAttachInfoCallback);

    // // Wait for asynch call is completed
    // // Please note, that you cannot call any operation of this client
    // // until this asynchronous request is completed.
    // // To call operation while request is processing, please use another copy of client.
    // while (!tTasksGetAttachInfoCallback.IsCompleted())
    // {
    //   rise::threading::CThread::Sleep(1000);
    // }


       TasksEchoOptCallback tTasksEchoOptCallback;
       pTasks->EchoOpt(opt, tTasksEchoOptCallback);

       // Wait for asynch call is completed
       // Please note, that you cannot call any operation of this client
       // until this asynchronous request is completed.
       // To call operation while request is processing, please use another copy of client.
       while (!tTasksEchoOptCallback.IsCompleted())
       {
         rise::threading::CThread::Sleep(1000);
       }


       TasksGetAllTasksCallback tTasksGetAllTasksCallback;
       pTasks->GetAllTasks(tTasksGetAllTasksCallback);

       // Wait for asynch call is completed
       // Please note, that you cannot call any operation of this client
       // until this asynchronous request is completed.
       // To call operation while request is processing, please use another copy of client.
       while (!tTasksGetAllTasksCallback.IsCompleted())
       {
         rise::threading::CThread::Sleep(1000);
       }

  }
  RISE_CATCH_ALL

  return 0;
}

