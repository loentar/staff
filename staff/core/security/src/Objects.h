#ifndef _OBJECTS_H_
#define _OBJECTS_H_

#include <string>
#include <list>

namespace staff
{
  namespace security
  {
    struct SObject
    {
      int nId;
      std::string sName;
      std::string sDescription;
      int nParentId;
    };

    typedef std::list<SObject> TObjectList;

    class CObjects
    {
    public:
      static CObjects& Inst();

      static void FreeInst();

      void GetById(int nId, SObject& rstObject);

      //! get object by path name
      /*! \param  sName - object name path (for example: "components.samples.calc.Calculator")
          \param  rstObject - resulting object
          \sa GetIdByPathName
        */
      void GetByPathName(const std::string& sName, SObject& rstObject);

      //! get object id by path name
      /*! \param  sName - object name path (for example: "components.samples.calc.Calculator")
          \param  nId - resulting object id
          \sa GetByPathName
        */
      bool GetIdByPathName(const std::string& sName, int& nId);

      void Add(const std::string& sName, const std::string& sDescription, int nParentId, int& nId);

      void Remove(int nId);

      void SetDescription(int nId, const std::string& sDescription);

      void GetChilds(int nId, TObjectList& rlsChilds);

      bool GetChildId(int nId, const std::string& sChildName, int& nChildId);

      void GetParent(int nId, SObject& rstParent);

      void GetParentId(int nId, int& nParentId);

    private:
      CObjects();
      ~CObjects();

    private:
      static CObjects* m_pInst;
    };
  }
}

#endif // _OBJECTS_H_
