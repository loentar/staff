#ifndef _CHECKER_H_
#define _CHECKER_H_

#include <list>
#include <staff/common/IService.h>
#include <staff/common/Optional.h>
#include <staff/client/ICallback.h>

namespace samples
{
namespace optional
{

  //! attach info
  struct AttachInfo
  {
    std::string sTitle;
    std::string sFileName;
    int nFileSize;
  };

  //! task with optional fields
  struct Task
  {
    staff::Optional<int> nId;
    std::string sDescr;
    staff::Optional<int> tnOwnerId;
    staff::Optional<AttachInfo> tstAttachInfo;
  };

  typedef std::list<Task> TasksList; //!< tasks list

  //! Tasks service
  class Tasks: public staff::IService
  {
  public:
    //! add new task
    /*! \param rstTask - task
        \return resulting id
      */
    virtual int Add(const Task& rstTask) = 0;

    virtual void UpdateOwner(int nTaskId, const staff::Optional<int>& rtnOwnerId) = 0;

    virtual void UpdateAttachInfo(int nTaskId, const staff::Optional<AttachInfo>& rtnAttachInfo) = 0;

    virtual staff::Optional<AttachInfo> GetAttachInfo(int nTaskId) = 0;

    virtual staff::Optional< std::list<std::string> > EchoOpt(const staff::Optional< std::list<std::string> >& opt) = 0;

    //! get all tasks
    /*! \return tasks list
      */
    virtual TasksList GetAllTasks() const = 0;



    //asynch
    virtual void Add(const Task& rstTask, staff::ICallback<int>& rCallback) = 0;

    virtual void UpdateOwner(int nTaskId, const staff::Optional<int>& rtnOwnerId, staff::ICallback<void>& rCallback) = 0;

    virtual void UpdateAttachInfo(int nTaskId, const staff::Optional<AttachInfo>& rtnAttachInfo, staff::ICallback<void>& rCallback) = 0;

    virtual void GetAttachInfo(int nTaskId, staff::ICallback< staff::Optional<AttachInfo> >& rCallback) = 0;

    virtual void EchoOpt(const staff::Optional< std::list<std::string> >& opt, staff::ICallback< staff::Optional< std::list<std::string> > >& rCallback) = 0;

    virtual void GetAllTasks(staff::ICallback<TasksList>& rCallback) const = 0;

  };
}
}

#endif // _CHECKER_H_

