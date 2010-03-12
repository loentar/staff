#ifndef _MyPlugin_h_
#define _MyPlugin_h_

#include <rise/string/String.h>

namespace myplugin  // namespace for plug-in
{
  //! interface class for plugin
  class CMyPlugin
  {
  public:
    //!  virtual destructor
    virtual ~CMyPlugin() {};

    //!   returns sum of "a" and "b" for test
    /*!   EXCEPTIONS:     CLogicFormatException, CLogicSizeException
          \param a - operand a
          \param b - operand b
          \return sum of "a" and "b"
    */
    virtual int Sum(int a, int b) = 0;

    //!  another example: put text value to log
    /*!  \param nType - type of message(log level)
         \param sLogString - text value
    */
    virtual void TestPutToLog(int nType, const rise::CString& sLogString) = 0;
  };
} // namespace myplugin

#endif // _MyPlugin_h_
