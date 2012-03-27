#ifndef _Tasks_h_
#define _Tasks_h_

#include <list>
#include <staff/common/IService.h>
#include <staff/common/Optional.h>

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
    // *isAttribute: true
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

    virtual std::list< staff::Optional<std::string> > EchoOpt2(const std::list< staff::Optional<std::string> >& opt) = 0;

    //! get all tasks
    /*! \return tasks list
      */
    virtual TasksList GetAllTasks() const = 0;
  };
}
}

#endif // _Tasks_h_

