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
#include "CodeGen.h"
#include "XmlGen.h"

void Help()
{
  std::cerr << "Code generator for Staff\n"
    "-n              - Project name\n"
    "-i<inputdir>    - Interface headers dir\n"
    "-o<outputdir>   - Output dir\n"
    "-c<changedir>   - set input and output dirs\n"
//    "-v              - verbose\n"
    "-t<templatedir> - Generate Source With Template\n"
    "-u              - Update only(generate only missing files, update existing files if needed)\n"
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
    for (std::list<std::string>::const_iterator it = lsFiles.begin(); it != lsFiles.end(); ++it)
    {
      std::string sFileName = stProject.sInDir + "/" + *it;
      std::ifstream isFile;
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
          
          throw sEx + ": before line\n" + sbData.str() + "\n";
        }
        catch (const char* szEx)
        {
          std::stringbuf sbData;
          isFile.get(sbData, '\n');
          isFile.ignore();
          isFile.get(sbData, '\n');

          isFile.close();

          throw std::string(szEx) + ": before line\n" + sbData.str() + "\n";
        }

        //std::cout << stInterface;
        stProject.lsInterfaces.push_back(stInterface);
      } else
      {
        throw std::string("can't open file: ") + *it + ": " + std::string(strerror(errno));
      }
    }

    tDoc.GetRoot() << stProject;

    if (bGenXml)
    {
      tDoc.GetDecl().m_sEncoding = "koi8-r";
      tDoc.SaveToFile(stProject.sOutDir + "/" + stProject.sName + ".xml");
    }

    if (sTemplate != "")
    {
      std::cout << "template: " << sTemplate << std::endl;
      CCodeGen tGen;
      tGen.Start(std::string(szStaffHome) + "/bin/template/" + sTemplate, stProject.sOutDir, tDoc.GetRoot(), bUpdateOnly);
    }

  }
  catch (const std::string& sError)
  {
    std::cerr << "ERROR: " << sError << "\nLineNo: " << GetLine() << std::endl << std::endl;
    return 1;
  }
  catch (const char* szError)
  {
    std::cerr << "ERROR: " << szError << "\nLineNo: " << GetLine() << std::endl << std::endl;
    return 1;
  }
  catch(const rise::CException& rEx)
  {
    std::cerr << "ERROR: " << rEx.GetDescr() << std::endl << std::endl;
    return 1;
  }

  return 0;
}
