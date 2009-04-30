#ifndef _WIDGETMANAGER_H_
#define _WIDGETMANAGER_H_

#include <list>
#include <map>
#include <rise/string/String.h>
#include <staff/common/Value.h>

namespace widget
{
  //! widget class
  struct SWidgetClass
  {
    rise::CString sClass;   //!< class name
    rise::CString sDescr;   //!< description
  };

  //! list of widget classes
  typedef std::list<SWidgetClass> TWidgetClassList;

  //! property of widget object
  struct SProperty
  {
    rise::CString sName;   //!< name of property
    staff::CValue tValue;  //!< value of property
  };

  //! list of properties
  typedef std::list<SProperty> TPropertyList;

  //! widget description
  struct SWidget
  {
    rise::CString sClass;        //!< widget class name
    rise::CString sName;         //!< widget instance name
    TPropertyList lsProperties;  //!< properties list
  };

  //! widget map
  typedef std::map<int, SWidget> TWidgetMap;

  //!  Widget Manager
  class CWidgetManager
  {
  public:
    //!        destructor
    virtual ~CWidgetManager() {};

    //!         open widget db
    /*! \param  sProfile - db name
        */
    virtual void Open(const rise::CString& sProfile) = 0;

    //!         close db, commiting changes
    virtual void Close() = 0;

    //!         commit changes
    virtual void Commit() = 0;

    //!         get widget class list
    /*! \return widget class list
    */
    virtual TWidgetClassList GetWidgetClassList() = 0;

    //!         get active widet list
    /*! \return active widget list
    */
    virtual TWidgetMap GetWidgetList() const = 0;

    //!         add widget to active list
    /*! \param  rWidget - widget description
        \return id of widget instance
        */
    virtual int AddWidget(const SWidget& rWidget) = 0;

    //!         delete
    /*! \param  rWidget - id of widget instance
        */
    virtual void DeleteWidget(int nWidgetID) = 0;

    //!         alter widget
    /*! \param  nWidgetID - id of widget instance
        \param  rWidget - widget description
        */
    virtual void AlterWidget(int nWidgetID, const SWidget& rWidget) = 0;
  };
}

#endif // _WIDGETMANAGER_H_
