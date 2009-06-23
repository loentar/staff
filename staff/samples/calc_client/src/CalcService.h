#ifndef _CalcService_H_
#define _CalcService_H_

namespace samples
{
  namespace calc
  {
    // description: sample component service CalcService
    // uri: http://localhost:9090/axis2/services/samples.calc.CalcService
    class CCalcService
    {
    public:
      virtual ~CCalcService() {}

      /*! add nA and nB.
          example of using non-default result node
      */
      virtual int /*addResult*/ Add(int nA, int nB) const = 0;

      /*! substract nA and nB.
          example of invoking another component service
      */
      virtual int Sub(int nA, int nB) const = 0;

      /*! store integer value.
          example of using service configuration
      */
      virtual void SetMem(int nMem) = 0;

      /*! retrieve integer value.
          example of using service configuration
      */
      virtual int GetMem() const = 0;
    };
  }
}

#endif
