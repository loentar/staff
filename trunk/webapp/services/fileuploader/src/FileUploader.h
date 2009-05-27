#ifndef _FILEUPLOADER_H_
#define _FILEUPLOADER_H_

#include <string>
#include <list>

namespace webapp
{
  typedef std::list<std::string> TStringList; //!< string list

  //!  File Uploader
  class CFileUploader
  {
  public:
    //!        destructor
    virtual ~CFileUploader() {};

    //!         move uploaded file
    /*! \param  sFileName - uploaded filename
        \param  sPathTo - target path
        */
    virtual void Move(const std::string& sFileName, const std::string& sPathTo) = 0;

    //!         unpack uploaded file
    /*! after unpacking file will be deleted
        \param  sFileName - uploaded filename
        \param  sPathTo - target path
        */
    virtual void Unpack(const std::string& sFileName, const std::string& sPathTo) = 0;

    //!         get unpacking status
    /*! \return -1 - running, 0 - finished with success, > 0 -  fail code
    */
    virtual int GetUnpackingStatus() = 0;
    
    //!         get unpacked files
    /*! \param  sMask - file mask
        \return list of unpacked files filtered by mask
    */
    virtual TStringList GetUnpackedFiles(const std::string& sMask) = 0;
    
    //!         delete uploaded file
    /*! \param  sFileName - uploaded filename
        */
    virtual void Delete(const std::string& sFileName) = 0;
  };
}

#endif // _FILEUPLOADER_H_
