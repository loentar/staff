/*
 *  Copyright 2012 Utkin Dmitry
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

#ifndef _STAFF_UTILS_FILE_H_
#define _STAFF_UTILS_FILE_H_

#include <list>
#include <string>
#include "staffutilsexport.h"

#ifdef _MSC_VER
#define STAFF_PATH_SEPARATOR "\\"
#else
#define STAFF_PATH_SEPARATOR "/"
#endif

namespace staff
{
  typedef std::list<std::string> StringList; //!< list of strings

  //! file finder
  class STAFF_UTILS_EXPORT File
  {
  public:
    //! file attributes
    enum Attribute
    {
      AttributeNone = 0,          //!< not a file nor directory
      AttributeDirectory = 1,     //!< directory
      AttributeRegularFile = 2,   //!< regular file
      AttributeOtherFile = 4,     //!< not a regular file (symlink, pipe, socket, etc...)
      AttributeAnyFile =          //!< any file (regular, symlink, pipe, socket, etc...)
        AttributeRegularFile | AttributeOtherFile,
      AttributeAll =              //!< any file or directory
        AttributeAnyFile | AttributeDirectory
    };

  public:
    //! constructor
    /*! \param  sPath - path to the file or directory
      */
    File(const std::string& sPath);

    //! find files/directories by name
    /*! \param  rList - resulting list of files/directories
        \param  sMask - file mask (shell pattern)
        \param  nAttrs - match by attributes
        */
    void List(StringList& rList, const std::string& sMask = "*", int nAttrs = AttributeAll);

    //! find files/directories by name
    /*! \param  sMask - file mask (shell pattern)
        \param  nAttrs - match by attributes
        \return resulting list of files/directories
        */
    StringList List(const std::string& sMask = "*", int nAttrs = AttributeAll);

    //! get file/directory attributes
    /*! if file does not exists returns AttributeNone
        \return attributes
      */
    int GetAttributes();

    //! test whether the file or directory exists
    /*! \return true if the file or directory exists
      */
    bool IsExists();

    //! tests whether path is a directory
    /*! \return true if path is a directory
      */
    bool IsDirectory();

    //! tests whether path is a regular file
    /*! \return true if path is a regular file
      */
    bool IsRegularFile();

    //! tests whether path is not a regular file (symlink, pipe, socket, etc...)
    /*! \return true if path is not a regular file
      */
    bool IsOtherFile();

    //! tests whether path is any file
    /*! \return true if path is any file
      */
    bool IsAnyFile();

    //! get file's last modification time
    /*! \return unix time
      */
    int GetTime();


    //! create the directory
    /*! \return true if the directory was created
      */
    bool Mkdir();

    //! create the directory, including parent directories
    /*! \return true if the directory was created
      */
    bool Mkdirs();

    //! is file name = "." or ".."
    /*! \param  szName - filename
        \return true if file name = "." or ".."
        */
    static bool IsDots(const char* szName);

    //! is file name = "." or ".."
    /*! \param  sName - filename
        \return true if file name = "." or ".."
        */
    static bool IsDots(const std::string& sName);

  private:
    std::string m_sPath; //!< path to the file or directory
  };
}

#endif // _STAFF_UTILS_FILE_H_

