#ifndef _RUNTIME_H_
#define _RUNTIME_H_

#include "staffcommonexport.h"
#include <string>

// Axis2/C typedefs
typedef struct axutil_env axutil_env_t;

namespace staff
{
  //!  Runtime information
  class STAFF_COMMON_EXPORT CRuntime
  {
  public:
    //!         get instance
    /*! \return ref to instance
    */
    static CRuntime& Inst();

    //!         get Axis2/C env for given component
    /*! \param  sEnvComponent - environment component(service, client, etc)
        \return Axis2/C env
    */
    axutil_env_t* GetAxis2Env(const std::string& sEnvComponent = "staff");

    //!         free Axis2/C env for given component
    /*! \param  sEnvComponent - environment component(service, client, etc)
    */
    void FreeAxis2Env(const std::string& sEnvComponent = "staff");

    //!         get Axis2/C home dir
    /*! \return Axis2/C home dir
    */
    std::string GetAxis2Home() const;

    //!         get staff home dir
    /*! \return staff home dir
    */
    std::string GetStaffHome() const;

    //!         get home dir for staff components
    /*! \return home dir for staff components
    */
    std::string GetComponentsHome() const;

    //!         get home dir for given component
    /*! \param  sComponent - component
        \return home dir for given component
        */
    std::string GetComponentHome(const std::string& sComponent) const;

    //!         get environment variable value
    /*! throws exception, when variable is not found
        \param  sVariable - environment variable
        \return environment variable value
        */
    std::string GetEnv(const std::string& sVariable) const;

  private:
    //!        protected constructor
    CRuntime();

    //!        destructor
    virtual ~CRuntime();

  private:
    class CRuntimeImpl;
    CRuntimeImpl* m_pImpl;
    static CRuntime* m_pInst;
  };
}

#endif // _RUNTIME_H_
