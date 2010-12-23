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
#include <rise/plugin/PluginManager.h>
#include <rise/tools/FileFind.h>
#include <rise/xml/XMLDocument.h>
#include <rise/xml/XMLNode.h>
#include "Interface.h"
#include "CodegenParser.h"
#include "CodeGen.h"
#include "XmlGen.h"

void Help()
{
  std::cerr << "Code generator for Staff\n"
    "staff_codegen [-n<Project_Name>][-i<inputdir>][-i<outputdir>][-c<chagedir>][-t<template>][-u][-e][-p<plugin_name>][-x][Source_files]\n"
    "  -n              - Project name\n"
    "  -i<inputdir>    - Interface headers dir\n"
    "  -o<outputdir>   - Output dir\n"
    "  -c<changedir>   - Set input and output dirs\n"
    "  -t<template>    - Generate source with template name\n"
    "  -u              - Update only(generate only missing files, update existing files if needed)\n"
    "  -e              - Don't error if Interface file is not contain service\n"
    "  -d              - Define environment variables: -dvar1=value1,var2=2,var3\n"
    "  -p<plugin_name> - Use plugin with given name(default: cpp). Example: -pwsdl\n"
    "  -x              - Write xml description\n\n";
}

int main(int nArgs, const char* szArgs[])
{
  if (nArgs < 2)
  {
    Help();
    return 1;
  }

  const char* szStaffHome = getenv("STAFF_HOME");

  staff::SParseSettings stParseSettings;
  staff::SProject stProject;

  std::string sTemplate;
  std::string sPluginName = "cpp";
  bool bGenXml = false;
  bool bUpdateOnly = false;
  staff::TStringMap mEnv;
  int nResult = 0;

  if(szStaffHome == NULL)
  {
    std::cerr << "Environment variable STAFF_HOME is not set!!" << std::endl;
    return 1;
  }

  stProject.sName = "Project1";
  stParseSettings.sInDir = ".";
  stParseSettings.sOutDir = ".";

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
        stParseSettings.sInDir = &szArgs[i][2];
        break;

      case 'o':
        stParseSettings.sOutDir = &szArgs[i][2];
        break;

      case 'c':
        stParseSettings.sInDir = &szArgs[i][2];
        stParseSettings.sOutDir = &szArgs[i][2];
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

      case 'w':
        std::cerr << "WARNING: \"-w\" is obsolete flag, please use \"-pwsdl\"" << std::endl;
        sPluginName = "wsdl";
        break;

      case 'e':
        stParseSettings.bNoServiceError = false;
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

      case 'p':
        sPluginName = &szArgs[i][2];
        break;

      default:
        std::cerr << "unrecognized option: " << szArgs[i] << std::endl << std::endl;
        Help();
        return 1;
      }
    }
    else
    {
      if (szArgs[i][0] != '\0')
      {
        stParseSettings.lsFiles.push_back(szArgs[i]);
      }
    }
  }


  try
  {
    // loading plugin
    std::string sPluginsDir = std::string(szStaffHome) + "/lib/codegen/parsers/";
    rise::plugin::CPluginManager<staff::ICodegenParser> tPlugins;

    const std::string& sFileName = sPluginsDir +
    RISE_LIBRARY_PREFIX "staffcgparser-" + sPluginName + RISE_LIBRARY_EXT;

    staff::ICodegenParser* pCodegenParser = tPlugins.LoadPlugin(sFileName, true);

    if (!pCodegenParser)
    {
      CSP_THROW("Can't get plugin symbol " + sFileName, "", 0);
    }

    stParseSettings.mEnv = mEnv;

    // Source files parsing
    pCodegenParser->Process(stParseSettings, stProject);

    // Generation
    rise::xml::CXMLDocument tDoc;
    tDoc.GetRoot() << stProject;

    if (bGenXml)
    {
      std::string sXmlFileName = stParseSettings.sOutDir + "/" + stProject.sName + ".xml";
      std::cout << "Generating " << sXmlFileName << std::endl;
      tDoc.GetDecl().m_sEncoding = "utf-8";
      tDoc.SaveToFile(sXmlFileName);
    }

    if (sTemplate != "")
    {
      rise::LogDebug() << "template: " << sTemplate;
      staff::CCodeGen tGen;
      tGen.Start(std::string(szStaffHome) + "/bin/template/" + sTemplate, stParseSettings.sOutDir, tDoc.GetRoot(), bUpdateOnly, mEnv);
    }
  }
  catch (const staff::CParseException& rException)
  {
    rise::LogError() << rException;
    nResult = 1;
  }
  catch (std::string sEx)
  {
    rise::LogError() << sEx;
    nResult = 1;
  }
  catch (const char* szEx)
  {
    rise::LogError() << szEx;
    nResult = 1;
  }
  RISE_CATCH_ALL_DESCR_ACTION("", nResult = 1;)

  return nResult;
}
