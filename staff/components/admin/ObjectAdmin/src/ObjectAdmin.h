#ifndef _OBJECTADMIN_H_
#define _OBJECTADMIN_H_

#include <vector>
#include <list>

namespace staff
{
  namespace admin
  {
    //! permission to object
    struct SPermission
    {
      bool bRead;      //!<  has read access
      bool bWrite;     //!<  has write access
      bool bExecute;   //!<  has execute access
    };

    //! permissions to object
    struct SPermissions
    {
      SPermission stUser;    //!<  permissions to user
      SPermission stGroup;   //!<  permissions to group
      SPermission stOthers;  //!<  permissions to others
    };

    //! object description
    struct SObject
    {
      int nObjectId;                   //!<  object id
      std::string sObjectName;         //!<  name
      int nType;                       //!<  object type \sa EObjectType
      std::string sDescription;        //!<  description
      int nUserId;                     //!<  owner user id
      int nGroupId;                    //!<  owner group id
      int nParentObjectId;             //!<  parent object id
      SPermissions stPermissions;      //!<  permissions to object
    };

    //! object type
    struct SObjectType
    {
      int nId; //!<  object type id
      std::string sName;  //!<  type name
      std::string sDescription; //!<  type description
    };

    typedef std::vector<int> TObjectIdList; //!<  list of objects id
    typedef std::list<SObject> TObjectList; //!<  list of objects
    typedef std::list<SObjectType> TObjectTypesList; //!<  list of objects types

    //!  object-admin service
    class CObjectAdmin
    {
    public:
      virtual ~CObjectAdmin() {};

      //!         get objectid list
      /*! \return objectid list
      */
      virtual TObjectIdList GetObjectIdList() = 0;

      //!         get objectid list
      /*! \return objectid list
      */
      virtual TObjectIdList GetObjectIdListWithParent(int nParentId) = 0;

      //!         get object by id
      /*! \param  nObjectId - object id
          \return object
          */
      virtual SObject GetObjectById(int nObjectId) = 0;

      //!         get object list by id list
      /*! \param  rlsObjectIdList - list of ids of objects
          \return list of objects
          */
      virtual TObjectList GetObjectList(const TObjectIdList& rlsObjectIdList) = 0;

      //!         add new object
      /*! SObject::nObjectId is ignored
          \param  pstObject - new object
          \return added object id
          */
      virtual int AddObject(const SObject& rstObject) = 0;


      //!         remove object by id
      /*! \param  nObjectId - object id
          */
      virtual void RemoveObject(int nObjectId) = 0;

      //!         replace object
      /*! \param  pstObject - new object
          \return added object id
          */
      virtual void ReplaceObject(const SObject& rstObject) = 0;


      //!         get object type list
      /*! \return type list
          */
      virtual TObjectTypesList GetObjectTypeList() = 0;

      //!         add new type
      /*! SObjectType::nId is ignored
          \param  rstObjectType - new type
          \return pointer to added type id
          */
      virtual int AddObjectType(const SObjectType& rstObjectType) = 0;

      //!         replace type
      /*! \param  pstObjectType - pointer to type
          \return true, if operation successes
          */
      virtual void ReplaceObjectType(const SObjectType& rstObjectType) = 0;

      //!         remove type by id
      /*! \param  nObjectTypeId - type id
          */
      virtual void RemoveObjectType(int nObjectTypeId) = 0;
    };
  }
}

#endif // _OBJECTADMIN_H_
