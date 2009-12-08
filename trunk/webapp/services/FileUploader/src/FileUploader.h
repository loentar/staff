/*
 *  Copyright 2009 Utkin Dmitry
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
 
/* 
 *  This file is part of the WSF Staff project.
 *  Please, visit http://code.google.com/p/staff for more information.
 */

#ifndef _FILEUPLOADER_H_
#define _FILEUPLOADER_H_

#include <string>
#include <list>

namespace webapp
{
  namespace admin
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
}

#endif // _FILEUPLOADER_H_
