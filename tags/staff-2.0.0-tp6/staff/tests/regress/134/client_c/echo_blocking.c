#include <axis2_util.h>
#include <axiom_soap.h>
#include <axis2_client.h>
#include <axutil_thread.h>

void* Run(axutil_thread_t* t, void* v)
{
  int i;
  const axis2_char_t *address = NULL;
  axis2_endpoint_ref_t *endpoint_ref = NULL;
  axis2_options_t *options = NULL;
  const axis2_char_t *client_home = NULL;
  axis2_svc_client_t *svc_client = NULL;
  axiom_node_t *payload = NULL;
  axiom_node_t *ret_node = NULL;

  client_home = AXIS2_GETENV("AXIS2C_HOME");


  for (i = 0; i < 50; ++i)
  {
    axutil_env_t *env = NULL;
    env = axutil_env_create_all("staff.log", AXIS2_LOG_LEVEL_CRITICAL);

    /* Set end point reference of echo service */
    address = "http://localhost:9090/axis2/services/echo";

    /* Create EPR with given address */
    endpoint_ref = axis2_endpoint_ref_create(env, address);

    /* Setup options */
    options = axis2_options_create(env);
    axis2_options_set_to(options, env, endpoint_ref);

    /* Set up deploy folder. It is from the deploy folder, the configuration is picked up
     * using the axis2.xml file.
     * In this sample client_home points to the Axis2/C default deploy folder. The client_home can 
     * be different from this folder on your system. For example, you may have a different folder 
     * (say, my_client_folder) with its own axis2.xml file. my_client_folder/modules will have the 
     * modules that the client uses
     */
    /* Create service client */
    svc_client = axis2_svc_client_create(env, client_home);
    if (!svc_client)
    {
        printf("Error creating service client, Please check AXIS2C_HOME again\n");
        AXIS2_LOG_ERROR(env->log, AXIS2_LOG_SI,
                        "Stub invoke FAILED: Error code:" " %d :: %s",
                        env->error->error_number,
                        AXIS2_ERROR_GET_MESSAGE(env->error));
        return NULL;
    }

    /* Set service client options */
    axis2_svc_client_set_options(svc_client, env, options);


    if (svc_client)
    {
        axis2_svc_client_free(svc_client, env);
        svc_client = NULL;
    }

    if (env)
    {
      axutil_env_free((axutil_env_t *) env);
      env = NULL;
    }
  }


  return NULL;
}


int main(int nArgs, const char* paszArgs[])
{
  static const int nThreadCount = 50;
  axutil_thread_t* pThread[nThreadCount];
  int bIsRunning = 0;
  int nTh = 0;

  axutil_env_t *env = NULL;
  env = axutil_env_create_all("staff.log", AXIS2_LOG_LEVEL_CRITICAL);

  for (nTh = 0; nTh < nThreadCount; ++nTh)
  {
    pThread[nTh] = axutil_thread_create(env->allocator, NULL, &Run, env);
    if (!pThread[nTh])
    {
      fprintf(stderr, "failed to create axutil thread");
      return 1;
    }
  }

  bIsRunning = 0;
  for (nTh = 0; nTh < nThreadCount; ++nTh)
  {
    axutil_thread_join(pThread[nTh]);
  }

  if (env)
  {
    axutil_env_free((axutil_env_t *) env);
    env = NULL;
  }

  printf("done\n");

  return 0;
}
