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

#ifndef WIN32
#include <errno.h>
#endif
#include <iostream>
#include <fstream>
#include <list>
#include <rise/common/Log.h>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include "InterfaceInfo.h"
#include "WsdlParser.h"
#include "CodeGen.h"
#include "XmlGen.h"

void Help()
{
  std::cerr << "Code generator for Staff\n"
    "-n              - Project name\n"
    "-i<inputdir>    - Interface headers dir\n"
    "-o<outputdir>   - Output dir\n"
    "-c<changedir>   - Set input and output dirs\n"
//    "-v              - verbose\n"
    "-t<templatedir> - Generate Source With Template\n"
    "-u              - Update only(generate only missing files, update existing files if needed)\n"
    "-e              - Don't error if Interface file is not contain service\n"
    "-d              - Define environment variables: -dvar1=value1,var2=2,var3"
    "-x              - Save xml description\n\n";
}

int main(int nArgs, const char* szArgs[])
{
  if (nArgs < 2)
  {
    Help();
    return 1;
  }

  const char* szStaffHome = getenv("STAFF_HOME");

  SProject stProject;
  rise::xml::CXMLDocument tDoc;
  std::string sTemplate;
  bool bGenXml = false;
  bool bUpdateOnly = false;
  bool bSourceIsWsdl = false;
  bool bNoServiceError = true;
  TStringMap mEnv;
  int nResult = 0;


  if(szStaffHome == NULL)
  {
    std::cerr << "Environment variable STAFF_HOME is not set!!" << std::endl;
    return 1;
  }

  stProject.sName = "Project1";
  stProject.sInDir = ".";
  stProject.sOutDir = ".";

  std::list<std::string> lsFiles;

  for (int i = 1; i < nArgs; ++i)
  {
    if (szArgs[i][0] == '-')
    {
      switch(szArgs[i][1])
      {
      case 'h':
        Help();
        return 0;

      case 'i':
        stProject.sInDir = &szArgs[i][2];
        break;

      case 'o':
        stProject.sOutDir = &szArgs[i][2];
        break;

      case 'c':
        stProject.sInDir = &szArgs[i][2];
        stProject.sOutDir = &szArgs[i][2];
        break;

      case 't':
        sTemplate = &szArgs[i][2];
        break;

      case 'n':
        stProject.sName = &szArgs[i][2];
        break;

      case 'x':
        bGenXml = true;
        break;

      case 'u':
        bUpdateOnly = true;
        break;

//      case 'v':
//        bVerbosity = true;
//        break;

      case 'w':
        bSourceIsWsdl = true;
        break;

      case 'e':
        bNoServiceError = false;
        break;

      case 'd':
        {
          std::string sVars = &szArgs[i][2];
          std::string sVar;
          std::string sName;
          std::string sValue;

          std::string::size_type nBegin = 0;
          std::string::size_type nEnd = 0;
          std::string::size_type nPos = 0;
          while (nEnd != std::string::npos)
          {
            nEnd = sVars.find(',', nBegin);
            sVar = sVars.substr(nBegin, nEnd - nBegin);

            if (sVar.size() != 0)
            {
              nPos = sVar.find('=');
              if (nPos == std::string::npos)
              {
                sName = sVar;
                sValue = "true";
              }
              else
              {
                sName = sVar.substr(0, nPos);
                sValue = sVar.substr(nPos + 1);
              }

              mEnv[sName] = sValue;
            }

            nBegin = nEnd + 1;
          }
        }
        break;

      default:
        std::cerr << "unrecognized option: " << szArgs[i];
        return 1;
      }
    } else
    {
      if (szArgs[i][0] != '\0')
        lsFiles.push_back(szArgs[i]);
    }
  }

  try
  {
    if (bSourceIsWsdl)
    {
      for (std::list<std::string>::const_iterator itFileName = lsFiles.begin(); itFileName != lsFiles.end(); ++itFileName)
      {
        std::string sFileName = stProject.sInDir + "/" + *itFileName;
        std::ifstream isFile;
        SInterface stInterface;

        std::cout << "Processing wsdl: " << *itFileName << std::endl;

        CWsdlParser::Inst().Parse(*itFileName, stInterface);

        stProject.lsInterfaces.push_back(stInterface);
      }
    }
    else
    {
      for (std::list<std::string>::const_iterator it = lsFiles.begin(); it != lsFiles.end(); ++it)
      {
        std::string sFileName = stProject.sInDir + "/" + *it;
        std::ifstream isFile;
        int nServicesCount = 0;
        SInterface stInterface;

        isFile.open(sFileName.c_str());
        if(isFile.good())
        {
          ResetLine();
          stInterface.sFileName = *it;
          try
          {
            isFile >> stInterface;
            isFile.close();
          }
          catch (const std::string& sEx)
          {
            std::stringbuf sbData;
            isFile.get(sbData, '\n');
            isFile.ignore();
            isFile.get(sbData, '\n');

            isFile.close();

            throw sEx + ": before\n-----------------\n" + sbData.str() + "\n-----------------\n";
          }
          catch (const char* szEx)
          {
            std::stringbuf sbData;
            isFile.get(sbData, '\n');
            isFile.ignore();
            isFile.get(sbData, '\n');

            isFile.close();

            throw std::string(szEx) + ": before\n-----------------\n" + sbData.str() + "\n-----------------\n";
          }

          nServicesCount += stInterface.lsClass.size();
          stProject.lsInterfaces.push_back(stInterface);
        } else
        {
          throw std::string("can't open file: ") + *it + ": " + std::string(strerror(errno));
        }

        if (nServicesCount == 0 && bNoServiceError)
        {
          throw std::string("No staff service interfaces found. Staff services must inherited from staff::IService.\n"
                            "Example:\n----\n  class Calc: public staff::IService\n"
                            "  {\n  public:\n    virtual int Add(int nA, int nB) = 0;\n  };\n----\n\n");
        }
      }
    }

    tDoc.GetRoot() << stProject;

    if (bGenXml)
    {
      tDoc.GetDecl().m_sEncoding = "utf-8";
      tDoc.SaveToFile(stProject.sOutDir + "/" + stProject.sName + ".xml");
    }

    if (sTemplate != "")
    {
      std::cout << "template: " << sTemplate << std::endl;
      CCodeGen tGen;
      tGen.Start(std::string(szStaffHome) + "/bin/template/" + sTemplate, stProject.sOutDir, tDoc.GetRoot(), bUpdateOnly, mEnv);
    }

  }
  catch (const std::string& sError)
  {
    rise::LogError() << sError << "\nLineNo: " << GetLine();
    nResult = 1;
  }
  catch (const char* szError)
  {
    rise::LogError() << szError << "\nLineNo: " << GetLine();
    nResult = 1;
  }
  RISE_CATCH_ALL_DESCR_ACTION("", nResult = 1;)

  return nResult;
}
