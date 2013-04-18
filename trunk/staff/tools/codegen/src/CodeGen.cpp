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

#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <errno.h>
#else
#include <sys/types.h>
#include <fnmatch.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <staff/utils/File.h>
#include <staff/utils/Log.h>
#include <staff/utils/tostring.h>
#include <staff/utils/fromstring.h>
#include <staff/utils/stringutils.h>
#include <staff/utils/Exception.h>
#include <staff/xml/Element.h>
#include <staff/codegen/tools.h>
#include "CodeGen.h"

namespace staff
{
namespace codegen
{
  class TemplateParser
  {
  public:
    TemplateParser():
      m_nLine(0), m_nIndent(0), m_bNeedIndent(false), m_bHasConfig(false)
    {
      m_tmVariables.push(StringMap());
    }

    std::string GetElementPath(const xml::Element* pElement) const
    {
      if (pElement)
      {
        std::string sPath = pElement->GetName();
        while ((pElement = pElement->GetParent()))
        {
          sPath = pElement->GetName() + "." + sPath;
        }
        return sPath;
      }
      else
      {
        return "";
      }
    }

    const xml::Element& GetElement(const std::string& sVariableName, const xml::Element& rElement) const
    {
      static xml::Element tEmptyElement;
      const xml::Element* pElement = &rElement;
      std::string::size_type nPos = sVariableName.find('.');
      std::string sVariable;
      std::string sClass;

      if (nPos != std::string::npos)
      {
        if (sVariableName.size() == 1)
        { // reference to current node
          return rElement;
        }

        if (!nPos)
        {
          bool bIsOpt = false;
          sVariable = sVariableName.substr(1);
          STAFF_ASSERT(!sVariable.empty(), "Element name expected in Name: " + sVariableName);
          nPos = sVariable.find('.');
          sClass = sVariable.substr(0, nPos); // next element name

          if (sClass[0] == '*') // sClass can't be empty
          {
            sClass.erase(0, 1);
            bIsOpt = true;
          }

          for (; pElement != NULL; pElement = pElement->GetParent())
          {
            const xml::Element* pChildElement = pElement->FindChildElementByName(sClass);
            if (pChildElement)
            {
              if (nPos == std::string::npos)
              {
                return *pChildElement;
              }

              sVariable.erase(0, nPos + 1);
              pElement = pChildElement;
              break;
            }
          }

          if (!pElement && bIsOpt)
          {
            return tEmptyElement;
          }
        }
        else
        {
          sClass = sVariableName.substr(0, nPos);
          sVariable = sVariableName.substr(nPos + 1);

          while (pElement != NULL && pElement->GetName() != sClass)
          {
            pElement = pElement->GetParent();
          }
        }

        STAFF_ASSERT(pElement != NULL, "\nCan't find node which match current class: \"" + sClass
                     + "\"\n context: " + GetElementPath(&rElement) + "\n Variable: " + sVariableName + "\n");
        STAFF_ASSERT(pElement->GetName() == sClass, "\nElement name does not match current class: \""
                     + pElement->GetName() + "\" <=> \"" + sClass + "\"\n context: " +
                     GetElementPath(&rElement) + "\n");

        while ((nPos = sVariable.find('.')) != std::string::npos)
        {
          const std::string& sSubClass = sVariable.substr(0, nPos);
          if (sSubClass[0] == '!' || sSubClass[0] == '$')
          {
            break;
          }
          try
          {
            if (sSubClass[0] == '*')
            {
              const xml::Element* pChildElement = pElement->FindChildElementByName(sSubClass.substr(1));
              pElement = pChildElement ? pChildElement : &tEmptyElement;
            }
            else
            {
              pElement = &pElement->GetChildElementByName(sSubClass);
            }
          }
          catch(...)
          {
            LogDebug() << "While parsing variable: [" << sVariableName << "]";
            throw;
          }

          sVariable.erase(0, nPos + 1);
        }
      }
      else
      {
        STAFF_ASSERT(pElement->GetName() == sVariableName, "node name does not match current class: \""
                     + pElement->GetName() + "\" <=> \"" + sVariableName + "\"");
        return rElement;
      }

      if (sVariable[0] == '$')
      {
        std::string sProperty;
        sVariable.erase(0, 1);

        nPos = sVariable.find('.');

        if (nPos != std::string::npos)
        {
          sProperty = sVariable.substr(0, nPos);
          sVariable.erase(0, nPos + 1);
        }
        else
        {
          sProperty = sVariable;
          sVariable.erase();
        }

        // number of this element by order
        if (sProperty == "Num")
        {
          static xml::Element tElementNum("Num");
          const xml::Element* pElementParent = pElement->GetParent();
          STAFF_ASSERT(pElementParent != NULL, "can't get number for node: " + pElement->GetName());

          int nNum = 0;

          for (const xml::Element* pChildElement = pElementParent->GetFirstChildElement();
               pChildElement; pChildElement = pChildElement->GetNextSiblingElement())
          {
            if (pChildElement == pElement)
            {
              break;
            }
            ++nNum;
          }

          tElementNum.SetValue(ToString(nNum));
          pElement = &tElementNum;
        }
        else
        if (sProperty == "Count")
        {
          static xml::Element tSubElementCount("Count");
          tSubElementCount.SetValue(ToString(pElement->GetChildElementCount()));
          pElement = &tSubElementCount;
        }
        else
        {
          STAFF_THROW_ASSERT("Unknown Property: [" + sVariable + "]");
        }

        if (sVariable.empty())
        {
          return *pElement;
        }
      }

      if (sVariable[0] == '!')
      { // exec function
        sVariable.erase(0, 1);
        pElement = &ExecuteFunction(sVariable, *pElement);
        while (!sVariable.empty()) // .!trimleft/:/.!dot
        {
          STAFF_ASSERT(sVariable.substr(0, 2) == ".!", "Junk [" + sVariable +  "] in variable: ["
              + sVariableName + "] at pos " + ToString(sVariableName.size() - sVariable.size()));
          sVariable.erase(0, 2);
          pElement = &ExecuteFunction(sVariable, *pElement);
        }
        return *pElement;
      }
      else
      if (sVariable[0] == '*') // optional node
      {
        const xml::Element* pChildElement = pElement->FindChildElementByName(sVariable.substr(1));
        if (pChildElement)
        {
          return *pChildElement;
        }
        else
        {
          return tEmptyElement;
        }
      }

      return pElement->GetChildElementByName(sVariable);
    }

    std::string::size_type ParseParam(std::string& sParamBegin) const
    {
      std::string::size_type nPos = sParamBegin.find_first_of("/\\");
      // slash demasking
      for (;;)
      {
        STAFF_ASSERT(nPos != std::string::npos, "Can't get param");

        char chFound = sParamBegin[nPos];
        if (chFound == '\\')
        {
          // demask
          if (nPos < sParamBegin.size())
          {
            switch (sParamBegin[nPos + 1])
            {
            case '/': sParamBegin.replace(nPos, 2, 1, '/'); break;
            case 'r': sParamBegin.replace(nPos, 2, 1, '\r'); break;
            case 'n': sParamBegin.replace(nPos, 2, 1, '\n'); break;
            case 't': sParamBegin.replace(nPos, 2, 1, '\t'); break;
            default: sParamBegin.erase(nPos, 1);
            }
          }
        }
        else
        { // '/'
          break;
        }

        nPos = sParamBegin.find_first_of("/\\", nPos + 1);
      }
      return nPos;
    }

    const xml::Element& ExecuteFunction(std::string& sFunction, const xml::Element& rElement) const
    {
      static xml::Element tResult("Result");
      std::string sResult = rElement.GetTextValue();
      tResult.Clear();

      if (sFunction.substr(0, 9) == "mangledot")
      {
        StringReplace(sResult, ".", "_", true);
        sFunction.erase(0, 9);
      }
      else
      if (sFunction.substr(0, 6) == "mangle")
      {
        if (sResult.size() >= 2 && sResult.substr(0, 2) == "::")
        {
          sResult.erase(0, 2);
        }

        if (sResult.size() >= 2 && sResult.substr(sResult.size() - 2, 2) == "::")
        {
          sResult.erase(sResult.size() - 2, 2);
        }

        StringReplace(sResult, "::", "_", true);
        sFunction.erase(0, 6);
      }
      else
      if (sFunction.substr(0, 3) == "dot")
      {
        if (sResult.size() >= 2 && sResult.substr(0, 2) == "::")
        {
          sResult.erase(0, 2);
        }

        if (sResult.size() >= 2 && sResult.substr(sResult.size() - 2, 2) == "::")
        {
          sResult.erase(sResult.size() - 2, 2);
        }

        StringReplace(sResult, "::", ".", true);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 3) == "not")
      {
        sFunction.erase(0, 3);

        sResult = sResult == "true" ? "false" : "true";
      }
      else
      if (sFunction.substr(0, 7) == "equals/")
      {
        sFunction.erase(0, 7);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        sResult = (sResult == sWhat) ? "true" : "false";
      }
      else
      if (sFunction.substr(0, 6) == "match/")
      {
        sFunction.erase(0, 6);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        sResult = (sResult.find(sWhat) != std::string::npos) ? "true" : "false";
      }
      else
      if (sFunction.substr(0, 8) == "replace/")
      {
        sFunction.erase(0, 8);

        // what replace
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        // replace with
        nPosEnd = ParseParam(sFunction);
        std::string sWith = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWith, rElement);

        StringReplace(sResult, sWhat, sWith, true);
      }
      else
      if (sFunction.substr(0, 13) == "replacenotof/")
      {
        sFunction.erase(0, 13);

        // what replace
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        // replace with
        nPosEnd = ParseParam(sFunction);
        std::string sWith = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWith, rElement);

        std::string::size_type nPosStart = 0;
        while ((nPosEnd = sResult.find_first_of(sWhat, nPosStart)) != std::string::npos)
        {
          sResult.replace(nPosStart, nPosEnd - nPosStart, sWith);
          nPosStart += sWith.size() + 1;
        }
        if (nPosStart < sResult.size())
        {
          sResult.replace(nPosStart, sResult.size() - nPosStart, sWith);
        }
      }
      else
      if (sFunction.substr(0, 5) == "trim/")
      {
        sFunction.erase(0, 5);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        StringTrim(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 9) == "trimleft/")
      {
        sFunction.erase(0, 9);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        StringTrimLeft(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 10) == "trimright/")
      {
        sFunction.erase(0, 10);

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        StringTrimRight(sResult, sWhat.c_str());
      }
      else
      if (sFunction.substr(0, 4) == "trim")
      {
        StringTrim(sResult);
        sFunction.erase(0, 4);
      }
      else
      if (sFunction.substr(0, 8) == "trimleft")
      {
        StringTrimLeft(sResult);
        sFunction.erase(0, 8);
      }
      else
      if (sFunction.substr(0, 7) == "tolower")
      {
        std::transform(sResult.begin(), sResult.end(), sResult.begin(), ::tolower);
        sFunction.erase(0, 7);
      }
      else
      if (sFunction.substr(0, 7) == "toupper")
      {
        std::transform(sResult.begin(), sResult.end(), sResult.begin(), ::toupper);
        sFunction.erase(0, 7);
      }
      else
      if (sFunction.substr(0, 11) == "tocamelcase")
      {
        sFunction.erase(0, 11);
        std::string::size_type nPos = 0;
        while ((nPos = sResult.find('_', nPos)) != std::string::npos)
        {
          if ((nPos + 1) < sResult.size())
          {
            sResult.replace(nPos, 2, 1, ::toupper(sResult[nPos + 1]));
          }
        }
      }
      else
      if (sFunction.substr(0, 9) == "trimright")
      {
        StringTrimRight(sResult);
        sFunction.erase(0, 9);
      }
      else
      if (sFunction.substr(0, 7) == "append/")
      {
        sFunction.erase(0, 7);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        sResult += sWhat;
      }
      else
      if (sFunction.substr(0, 8) == "prepend/")
      {
        sFunction.erase(0, 8);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        sResult = sWhat + sResult;
      }
      else
      if (sFunction.substr(0, 9) == "deprefix/")
      {
        sFunction.erase(0, 9);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);


        if (sResult.substr(0, sWhat.size()) == sWhat)
        {
          sResult.erase(0, sWhat.size());
        }
      }
      else
      if (sFunction.substr(0, 10) == "depostfix/")
      {
        sFunction.erase(0, 10);
        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        std::string::size_type nResSize = sResult.size();
        std::string::size_type nWhatSize = sWhat.size();

        if (nResSize > nWhatSize &&
            sResult.substr(nResSize - nWhatSize) == sWhat)
        {
          sResult.erase(nResSize - nWhatSize);
        }
      }
      else
      if (sFunction.substr(0, 6) == "token/")
      {
        sFunction.erase(0, 6);
        const std::string& sVal = rElement.GetValue();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        std::string::size_type nPos = sVal.find_first_of(sWhat);

        if (nPos != std::string::npos)
        {
          sResult = sVal.substr(0, nPos);
        }
        else
        {
          sResult.erase();
        }
      }
      else
      if (sFunction.substr(0, 10) == "lasttoken/")
      {
        sFunction.erase(0, 10);
        const std::string& sVal = rElement.GetValue();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        std::string::size_type nPos = sVal.find_last_of(sWhat);

        if (nPos != std::string::npos)
        {
          sResult = sVal.substr(nPos + 1);
        }
        else
        {
          sResult.erase();
        }
      }
      else
      if (sFunction.substr(0, 4) == "cut/")
      {
        sFunction.erase(0, 4);
        const std::string& sVal = rElement.GetValue();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        std::string::size_type nPos = sVal.find_first_of(sWhat);

        if (nPos != std::string::npos)
        {
          sResult = sVal.substr(nPos + 1);
        }
        else
        {
          sResult.erase();
        }
      }
      else
      if (sFunction.substr(0, 8) == "cutlast/")
      {
        sFunction.erase(0, 8);
        const std::string& sVal = rElement.GetValue();

        std::string::size_type nPosEnd = ParseParam(sFunction);
        std::string sWhat = sFunction.substr(0, nPosEnd);
        sFunction.erase(0, nPosEnd + 1);
        ReplaceToValue(sWhat, rElement);

        std::string::size_type nPos = sVal.find_last_of(sWhat);

        if (nPos != std::string::npos)
        {
          sResult = sVal.substr(0, nPos);
        }
        else
        {
          sResult.erase();
        }
      }
      else
      if (sFunction.substr(0, 8) == "fixid")
      {
        sFunction.erase(0, 8);
        FixId(sResult);
      }
      else
      if (sFunction.substr(0, 3) == "inc")
      {
        double dTmp = 0;
        FromString(sResult, dTmp);
        ToString(dTmp + 1, sResult);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 3) == "dec")
      {
        double dTmp = 0;
        FromString(sResult, dTmp);
        ToString(dTmp - 1, sResult);
        sFunction.erase(0, 3);
      }
      else
      if (sFunction.substr(0, 4) == "add/")
      {
        std::string::size_type nPosWhat = sFunction.find('/', 4);

        STAFF_ASSERT(nPosWhat != std::string::npos, "Can't get operand for add");

        const std::string& sWhat = sFunction.substr(4, nPosWhat - 4);
        double dOp1 = 0;
        double dOp2 = 0;
        FromString(sResult, dOp1);
        FromString(sWhat, dOp2);
        ToString(dOp1 + dOp2, sResult);
        sFunction.erase(0, nPosWhat + 1);
      }
      else
      if (sFunction.substr(0, 4) == "sub/")
      {
        std::string::size_type nPosWhat = sFunction.find('/', 4);

        STAFF_ASSERT(nPosWhat != std::string::npos, "Can't get operand for sub");
        const std::string& sWhat = sFunction.substr(4, nPosWhat - 4);
        double dOp1 = 0;
        double dOp2 = 0;
        FromString(sResult, dOp1);
        FromString(sWhat, dOp2);
        ToString(dOp1 - dOp2, sResult);
        sFunction.erase(0, nPosWhat + 1);
      }
      else
      if (sFunction.substr(0, 5) == "trunc")
      {
        double dTmp = 0;
        FromString(sResult, dTmp);
        ToString(static_cast<long>(dTmp), sResult);
        sFunction.erase(0, 5);
      }
      else
      {
        STAFF_THROW_ASSERT("function " + sFunction + " is undefined");
      }

      tResult.SetValue(sResult);
      return tResult;
    }


    const std::string& GetValue(const std::string& sVariableName, const xml::Element& rElement) const
    {
      return GetElement(sVariableName, rElement).GetValue();
    }

    std::string::size_type ReplaceToValueFindBracketMatch(std::string& sString,
                                                          std::string::size_type nPosStart,
                                                          const xml::Element& rElement) const
    {
      int nRecursion = 1;
      std::string::size_type nPosEnd = nPosStart;
      while ((nPosEnd = sString.find_first_of("()", nPosEnd)) != std::string::npos)
      {
        if (sString[nPosEnd] == ')')
        {
          if (nPosEnd > 0 && sString[nPosEnd - 1] == '\\')
          {
            ++nPosEnd;
            continue;
          }
          --nRecursion;
          if (nRecursion == 0)
          {
            break;
          }
        }
        else
        { // == '('
          // check for inline $()
          if (sString[nPosEnd - 1] == '$')
          {
            std::string::size_type nInlineEnd = ReplaceToValueFindBracketMatch(sString, nPosEnd + 1, rElement);
            STAFF_ASSERT(nInlineEnd != std::string::npos, "end of inline variable name expected: [" +
                         sString + "]");
            std::string sInline = sString.substr(nPosEnd - 1, nInlineEnd - nPosEnd + 2);
            ReplaceToValue(sInline, rElement);
            sString.replace(nPosEnd - 1, nInlineEnd - nPosEnd + 2, sInline);
            --nPosEnd; // move to prior the '$('
            continue;
          }
          else
          {
            ++nRecursion;
          }
        }
        ++nPosEnd;
      }
      return nPosEnd;
    }


    void ProcessValue(const std::string& sName, std::string& sValue, const xml::Element& rElement) const
    {
      if (sName[0] == '$')
      {
        if (sName.substr(0, 16) == "$ThisElementName")
        {
          sValue = rElement.GetName();
          if (sName.size() == 16)
          {
            return;
          }
          xml::Element tVarElement(sValue);
          tVarElement.SetValue(sValue);
          sValue = GetValue(sValue + sName.substr(16), tVarElement);
        }
        else
        if (sName.substr(0, 17) == "$ThisElementValue")
        {
          sValue = rElement.GetTextValue();
          if (sName.size() == 17)
          {
            return;
          }
          xml::Element tVarElement(sValue);
          tVarElement.SetValue(sValue);
          sValue = GetValue(sValue + sName.substr(17), tVarElement);
        }
        else
        if (sName.substr(0, 16) == "$ThisElementPath")
        {
          sValue = GetElementPath(&rElement);
          if (sName.size() == 16)
          {
            return;
          }
          xml::Element tVarElement(sValue);
          tVarElement.SetValue(sValue);
          sValue = GetValue(sValue + sName.substr(16), tVarElement);
        }
        else
        {
          std::string::size_type nPos = sName.find('.');
          if (nPos != std::string::npos) // variable + functions
          {
            const std::string& sVarName = sName.substr(1, nPos - 1);
            sValue = m_tmVariables.top()[sVarName];
            // process functions and other
            xml::Element tVarElement(sVarName);
            tVarElement.SetValue(sValue);
            sValue = GetValue(sName.substr(1), tVarElement);
          }
          else // variable only
          {
            sValue = m_tmVariables.top()[sName.substr(1)];
          }
        }
      }
      else // node value
      {
        sValue = GetValue(sName, rElement);
      }
    }

    void ReplaceToValue(std::string& sString, const xml::Element& rElement) const
    {
      std::string::size_type nPosStart = 0;
      std::string::size_type nPosEnd = 0;

      while((nPosStart = sString.find("$(", nPosEnd)) != std::string::npos)
      {
        if (nPosStart > 0 && sString[nPosStart - 1] == '\\' && nPosEnd != nPosStart)
        {
          sString.erase(nPosStart - 1, 1);
          nPosEnd = nPosStart + 1;
          continue;
        }

        nPosEnd = ReplaceToValueFindBracketMatch(sString, nPosStart + 2, rElement);

        STAFF_ASSERT(nPosEnd != std::string::npos, "end of variable name expected: [" + sString + "]");
        const std::string& sExpression = sString.substr(nPosStart + 2, nPosEnd - nPosStart - 2);
        std::string sValue;

        { // parse "node.name||$var.!func||some.other"
          std::string::size_type nNamePosBegin = 0;
          std::string::size_type nNamePosEnd = 0;

          // use first nonempty value
          for(;;)
          {
            nNamePosEnd = sExpression.find("||", nNamePosBegin);

            const std::string& sName = sExpression.substr(nNamePosBegin,
                           (nNamePosEnd != std::string::npos) ? nNamePosEnd - nNamePosBegin : nNamePosEnd);
            std::string::size_type nNameSize = sName.size();

            if (nNameSize > 0 && sName[0] == '"' && sName[nNameSize - 1] == '"')
            {
              sValue = sName.substr(1, nNameSize - 2);
              ReplaceToValue(sValue, rElement);
            }
            else
            {
              ProcessValue(sName, sValue, rElement);
            }

            if (!sValue.empty() || nNamePosEnd == std::string::npos)
            {
              break;
            }

            nNamePosBegin = nNamePosEnd + 2;
          }
        }

        sString.replace(nPosStart, nPosEnd - nPosStart + 1, sValue);
        nPosEnd = nPosStart + sValue.size();
      }
    }

    void Init(const std::string& sInDir)
    {
      m_bHasConfig = File(sInDir + STAFF_PATH_SEPARATOR + "codegen.config").IsRegularFile();
      if (!m_bHasConfig)
      {
        StringList lsFiles;

        File(sInDir).List(lsFiles, "*.*", File::AttributeRegularFile);
        for (StringList::const_iterator itFile = lsFiles.begin(); itFile != lsFiles.end(); ++itFile)
        {
          if (itFile->find('$') != std::string::npos)
          {
            m_tTemplateFileList.push_back(*itFile);
          }
          else
          {
            m_tConstFileList.push_back(*itFile);
          }
        }
      }

      m_sInDir = sInDir;
    }

    void Start(const std::string& sOutDir, const xml::Element& rRootElement, bool bUpdateOnly)
    {
      bool bNeedUpdate = false;

      m_sOutDir = sOutDir;

      if (m_bHasConfig)
      {
        const std::string& sIn = m_sInDir + "codegen.config";
        std::ostringstream fsOut;
        std::ifstream fsIn;

        fsIn.open(sIn.c_str());
        STAFF_ASSERT(fsIn.good(), "can't open input file: " + sIn);

        m_nIndent = 0;
        m_nLine = 0;
        Process(fsIn, fsOut, rRootElement);

        fsIn.close();
      }
      else
      {
        const xml::Element& rElementInterfaces = rRootElement.GetChildElementByName("Interfaces");

        for (const xml::Element* pChildElement = rElementInterfaces.GetFirstChildElement();
                  pChildElement; pChildElement = pChildElement->GetNextSiblingElement())
        {
          const xml::Element& rElementInterface = *pChildElement;

          for (std::list<std::string>::const_iterator itTemplateFile = m_tTemplateFileList.begin();
              itTemplateFile != m_tTemplateFileList.end(); ++itTemplateFile)
          {
            std::string sFile = *itTemplateFile;
            bool bProcessFile = false;
            try
            {
              ReplaceToValue(sFile, rElementInterface);
              bProcessFile = true;
            }
            catch (const Exception&)
            {
              LogDebug1() << "Skipping template file " << sFile
                  << " for interface " << rElementInterface.GetChildElementByName("NsName").GetTextValue();
              continue;
            }

            if (bProcessFile)
            {
              // erase input path
              std::string::size_type nPos = sFile.find_last_of(STAFF_PATH_SEPARATOR);
              if (nPos != std::string::npos)
              {
                sFile.erase(0, nPos + 1);
              }

              ProcessFile(m_sInDir + *itTemplateFile, sOutDir, sFile,
                          rElementInterface, bUpdateOnly, bNeedUpdate);
            }
          }
        } // for interface

        for (std::list<std::string>::const_iterator itTemplateFile = m_tConstFileList.begin();
            itTemplateFile != m_tConstFileList.end(); ++itTemplateFile)
        {
          std::string sFile = *itTemplateFile;

          // erase input path
          std::string::size_type nPos = sFile.find_last_of(STAFF_PATH_SEPARATOR);
          if (nPos != std::string::npos)
          {
            sFile.erase(0, nPos + 1);
          }

          ProcessFile(m_sInDir + *itTemplateFile, sOutDir, sFile, rRootElement, bUpdateOnly, bNeedUpdate);
        }
      } // has config
    }

    void ProcessIfeq(std::istream& fsIn, std::ostream& fsOut, const xml::Element& rElement, std::string& sLine,
                     bool bNotEq = false)
    {
      bool bCurrentBlock = true;
      bool bEq = false;
      std::stringbuf sbData;
      std::string sLines;
      int nRecursion = 1;

      ReplaceToValue(sLine, rElement);

      { //#ifeq(123,321)
        int nOffsetPos = bNotEq ? 7 : 6;
        std::string::size_type nPosStart = sLine.find(",", 6);
        std::string::size_type nPosEnd = 0;

        STAFF_ASSERT(nPosStart != std::string::npos, "#ifeq expression is invalid!: \n----\n" + sLine +
                     "\n----\n");
        nPosEnd = sLine.find(')', nPosStart);
        STAFF_ASSERT(nPosEnd != std::string::npos, "#ifeq expression is invalid!: \n----\n" + sLine +
                     "\n----\n");

        std::string sLeft = sLine.substr(nOffsetPos, nPosStart - nOffsetPos);
        std::string sRight = sLine.substr(nPosStart + 1, nPosEnd - nPosStart - 1);

        bEq = false;

        std::string::size_type nPosStartLeft = 0;
        std::string::size_type nPosStartRight = 0;
        std::string::size_type nPosEndLeft = 0;
        std::string::size_type nPosEndRight = 0;
        do
        {
          nPosEndLeft = sLeft.find("||", nPosStartLeft);
          const std::string& sLeftCmp = sLeft.substr(nPosStartLeft, nPosEndLeft - nPosStartLeft);

          nPosStartRight = 0;
          nPosEndRight = 0;
          do
          {
            nPosEndRight = sRight.find("||", nPosStartRight);
            const std::string& sRightCmp = sRight.substr(nPosStartRight, nPosEndRight - nPosStartRight);

            if (sLeftCmp == sRightCmp)
            {
              bEq = true;
              break;
            }
            nPosStartRight = nPosEndRight + 2;
          } while (nPosEndRight != std::string::npos);
          nPosStartLeft = nPosEndLeft + 2;
        } while (nPosEndLeft != std::string::npos && !bEq);

        if (bNotEq)
        {
          bEq = !bEq;
        }
      }

      while (!fsIn.eof() && fsIn.good() && nRecursion > 0)
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        } else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str() + "\n";
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 6) == "#ifeq(" || sLine.substr(0, 7) == "#ifneq(")
        {
          ++nRecursion;
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
        }
        else
        if (sLine.substr(0, 5) == "#else")
        {
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
          if (nRecursion == 1)
            bEq = !bEq;
        }
        else
        if (sLine.substr(0, 8) == "#ifeqend")
        {
          if (bCurrentBlock == bEq && nRecursion > 1)
            sLines += sLine;
          --nRecursion;
        }
        else
        if (bCurrentBlock == bEq)
        {
          sLines += sLine;
        }
      }

      STAFF_ASSERT(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines +
                   "\n------------\n");

      {
        std::istringstream ssStream(sLines);
        Process(ssStream, fsOut, rElement);
      }
    }

    void ProcessForEach(std::istream& fsIn, std::ostream& fsOut, const xml::Element& rElement,
                        std::string& sLine)
    {
      std::stringbuf sbData;
      std::string sForEachExpr;
      std::string sLines;
      int nRecursion = 1;

      std::string::size_type nPosStart = sLine.find("$(", 9);
      std::string::size_type nPosEnd = 0;

      STAFF_ASSERT(nPosStart != std::string::npos, "foreach expression is invalid!");
      nPosEnd = sLine.find(')', nPosStart);
      STAFF_ASSERT(nPosEnd != std::string::npos, "foreach expression is invalid!");
      sForEachExpr = sLine.substr(nPosStart + 2, nPosEnd - nPosStart - 2);

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        } else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (fsIn.peek() == '\n')
          {
            sLine += "\n";
          }
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 9) == "#foreach ")
          ++nRecursion;
        else
        if (sLine.substr(0, 4) == "#end")
        {
          --nRecursion;
          if (nRecursion == 0)
            break;
        }

          sLines += sLine;
      }

      STAFF_ASSERT(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines +
                   "\n------------\n");

      const xml::Element& rSubElement = GetElement(sForEachExpr, rElement);

      for (const xml::Element* pChildElement = rSubElement.GetFirstChildElement();
        pChildElement; pChildElement = pChildElement->GetNextSiblingElement())
      {
        std::istringstream ssStream(sLines);
        Process(ssStream, fsOut, *pChildElement);
      }
    }

    void ProcessContext(std::istream& fsIn, std::ostream& fsOut, const xml::Element& rElement,
                        std::string& sLine)
    {
      std::stringbuf sbData;
      std::string sContextExpr;
      std::string sLines;
      int nRecursion = 1;

      std::string::size_type nPosStart = sLine.find("$(", 9);
      std::string::size_type nPosEnd = 0;

      STAFF_ASSERT(nPosStart != std::string::npos, "context expression is invalid!");
      nPosEnd = sLine.find(')', nPosStart);
      STAFF_ASSERT(nPosEnd != std::string::npos, "context expression is invalid!");
      sContextExpr = sLine.substr(nPosStart, nPosEnd - nPosStart + 1);

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        }
        else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (fsIn.peek() == '\n')
          {
            sLine += "\n";
          }
          sbData.str("");
        }
        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 9) == "#context ")
          ++nRecursion;
        else
        if (sLine.substr(0, 11) == "#contextend")
        {
          --nRecursion;
          if (nRecursion == 0)
            break;
        }

        sLines += sLine;
      }

      STAFF_ASSERT(nRecursion == 0, "Unexpected EOF while parsing: \n---------\n" + sLines +
                   "\n------------\n");

      if (sContextExpr[2] == '$') // variable
      {
        ReplaceToValue(sContextExpr, rElement);
      }
      else
      {
        sContextExpr.erase(0, 2);
        sContextExpr.erase(sContextExpr.size() - 1);
      }

      const xml::Element& rSubElement = GetElement(sContextExpr, rElement);

      std::istringstream ssStream(sLines);
      Process(ssStream, fsOut, rSubElement);
    }

    void ProcessInclude(std::ostream& fsOut, const xml::Element& rElement, const std::string& sLine)
    {
      std::string sIncludeFileName;

      char chQuote = *sLine.begin();
      std::string::size_type nPos = 0;

      if (chQuote == '<')
      {
        nPos = sLine.find('>', 1);
        STAFF_ASSERT(nPos != std::string::npos, "cginclude expression is invalid!");
        sIncludeFileName = m_sInDir + "../" + sLine.substr(1, nPos - 1);
      }
      else
      if (chQuote == '"')
      {
        nPos = sLine.find('"', 1);
        STAFF_ASSERT(nPos != std::string::npos, "cginclude expression is invalid!");
        sIncludeFileName = m_sInDir + sLine.substr(1, nPos - 1);
      }
      else
      {
        STAFF_THROW_ASSERT("cginclude expression is invalid!");
      }

#ifdef WIN32
      StringReplace(sIncludeFileName, "/", "\\", true);
#endif

      std::ifstream fsIncFile;
      fsIncFile.open(sIncludeFileName.c_str());

      STAFF_ASSERT(fsIncFile.good(), "can't include file: " + sIncludeFileName);

      std::string sCurrInDir = m_sInDir;
      m_sInDir = sIncludeFileName.substr(0, sIncludeFileName.find_last_of("/\\") + 1);
      while (!fsIncFile.eof() && fsIncFile.good())
      {
        Process(fsIncFile, fsOut, rElement);
      }
      m_sInDir = sCurrInDir;

      fsIncFile.close();
    }

    void ProcessIndent(const std::string& sLine)
    {
      std::string sValue = sLine.substr(8);
      StringTrim(sValue);
      if (sValue == "+")
      {
        STAFF_ASSERT(m_nIndent < 1024, "Invalid indentation: " + ToString(m_nIndent + 1)
                     + " while processing line: \n" + sLine);
        ++m_nIndent;
      }
      else
      if (sValue == "-")
      {
        STAFF_ASSERT(m_nIndent > 0, "Invalid indentation: " + ToString(m_nIndent - 1)
                     + " while processing line: \n" + sLine);
        --m_nIndent;
      }
      else
      {
        int nSign = 0;
        int nIndent = m_nIndent;
        if (sValue[0] == '+')
        {
          sValue.erase(0, 1);
          nSign = 1;
        }
        else
        if (sValue[0] == '-')
        {
          sValue.erase(0, 1);
          nSign = -1;
        }

        FromString(sValue, nIndent);
        if (nSign != 0)
        {
          nIndent = m_nIndent + nSign * nIndent;
        }
        STAFF_ASSERT(nIndent < 1024 && nIndent >= 0, "Invalid indentation: " + ToString(nIndent)
                     + " while processing line: \n" + sLine);
        m_nIndent = nIndent;
      }
    }

    void Process(std::istream& fsIn, std::ostream& fsOut, const xml::Element& rElement)
    {
      std::string sLine;
      std::stringbuf sbData;

      while (!fsIn.eof() && fsIn.good())
      {
        if (fsIn.peek() == '\n')
        {
          sLine = "\n";
        }
        else
        {
          fsIn.get(sbData, '\n');
          sLine = sbData.str();
          if (sLine.size() > 0 && *sLine.rbegin() == '\\')
            sLine.erase(sLine.size() - 1);
          else
          {
            if (fsIn.peek() == '\n')
            {
              sLine += "\n";
            }
          }
          sbData.str("");
        }

        fsIn.ignore();
        fsIn.peek(); // for EOF

        if (sLine.substr(0, 5) == "#var ")
        {
          std::string::size_type nPos = sLine.find_first_of(" \t", 5);
          std::string sVariable;
          std::string sValue;

          if (nPos == std::string::npos)
          {
            sVariable = sLine.substr(5);
            StringTrimRight(sVariable);
          }
          else
          {
            sVariable = sLine.substr(5, nPos - 5);
            sValue = sLine.substr(nPos + 1, sLine.size() - nPos - 2);
            ReplaceToValue(sValue, rElement);
          }

          STAFF_ASSERT(!sVariable.empty(), "invalid var declaration: " + sLine);

          m_tmVariables.top()[sVariable] = sValue;
        }
        else
        if (sLine.substr(0, 6) == "#ifeq(")
        {
          ProcessIfeq(fsIn, fsOut, rElement, sLine);
        }
        else
        if (sLine.substr(0, 7) == "#ifneq(")
        {
          ProcessIfeq(fsIn, fsOut, rElement, sLine, true);
        }
        else
        if (sLine.substr(0, 9) == "#foreach ")
        {
          ProcessForEach(fsIn, fsOut, rElement, sLine);
        }
        else
        if (sLine.substr(0, 9) == "#context ")
        {
          ProcessContext(fsIn, fsOut, rElement, sLine);
        }
        else
        if (sLine.substr(0, 10) == "#fileopen ")
        {
          std::string sFileName = sLine.substr(10);
          ReplaceToValue(sFileName, rElement);
          StringTrim(sFileName);

          STAFF_ASSERT(!sFileName.empty(), "#fileopen: Filename is empty");

          sFileName = m_sOutDir + sFileName;
          const std::string& sFailedFileName = sFileName + ".failed";

          std::ofstream ofsFile(sFileName.c_str());
          STAFF_ASSERT(ofsFile.good(), "can't open output file: " + sFileName);

          std::cout << "Generating " << sFileName << std::endl;
          m_nIndent = 0;
          try
          {
            Process(fsIn, ofsFile, rElement);
          }
          catch(...)
          {
            ofsFile.close();
            ::unlink(sFailedFileName.c_str());
            ::rename(sFileName.c_str(), sFailedFileName.c_str());
            throw;
          }
          ofsFile.close();
          ::unlink(sFailedFileName.c_str());
        }
        else
        if (sLine.substr(0, 10) == "#fileclose")
        {
          return;
        }
        else
        if (sLine.substr(0, 7) == "#mkdir ")
        {
          std::string sDirName = sLine.substr(7);
          ReplaceToValue(sDirName, rElement);
          StringTrim(sDirName);

          File(m_sOutDir + sDirName).Mkdirs();
        }
        else
        if (sLine.substr(0, 11) == "#cginclude ")
        {
          sLine.erase(0, 11);
          StringTrim(sLine);
          ProcessInclude(fsOut, rElement, sLine);
        }
        else
        if (sLine.substr(0, 11) == "#cgwarning ")
        {
          ReplaceToValue(sLine, rElement);
          StringTrimRight(sLine, "\n\r");
          std::cerr << "Warning: " << sLine.substr(11) << std::endl;
        }
        else
        if (sLine.substr(0, 9) == "#cgerror ")
        {
          ReplaceToValue(sLine, rElement);
          STAFF_THROW_ASSERT(sLine.substr(9));
        }
        else
        if (sLine.substr(0, 8) == "#indent ")
        {
          ProcessIndent(sLine);
        }
        else
        if (sLine.substr(0, 11) == "#cgpushvars")
        {
          m_tmVariables.push(m_tmVariables.top());
        }
        else
        if (sLine.substr(0, 10) == "#cgpopvars")
        {
          m_tmVariables.pop();
        }
        else
        if (sLine.substr(0, 11) == "#cgdumpvars")
        {
          const StringMap& rmVars = m_tmVariables.top();
          sLine = "variables dump:";
          for (StringMap::const_iterator itVar = rmVars.begin(); itVar != rmVars.end(); ++itVar)
          {
            sLine += "\n" + itVar->first + "=\"" + itVar->second + "\"";
          }
          fsOut << sLine << "\n";
        }
        else
        {
          if (!sLine.empty())
          {
            std::string sIndent;
            STAFF_ASSERT(m_nIndent < 1024, "Invalid indentation: " + ToString(m_nIndent));
            for (int nIndent = 0; nIndent < m_nIndent; ++nIndent)
            {
              sIndent += "  ";
            }
            if (m_bNeedIndent && sLine[0] != '\n')
            {
              sLine = sIndent + sLine;
              m_bNeedIndent = false;
            }

            std::string::size_type nBegin = 0;
            std::string::size_type nEnd = 0;
            while ((nEnd = sLine.find('\n', nBegin)) != std::string::npos)
            {
              if (m_bNeedIndent && nEnd > nBegin) // line is not empty
              {
                sLine.insert(nBegin, sIndent);
                nEnd += sIndent.size();
              }

              nBegin = nEnd + 1;

              m_bNeedIndent = true;
            }

            ReplaceToValue(sLine, rElement);

            fsOut << sLine;
          }
        }
        ++m_nLine;
      }
    }

    void ProcessFile(const std::string& sIn, const std::string& sOutDir, const std::string& sOutFile,
                     const xml::Element& rElementInterface, bool bUpdateOnly, bool& bNeedUpdate)
    {
      const std::string& sOut = sOutDir + sOutFile;
      if (bUpdateOnly)
      {
        bool bIsStaticTemplate = rElementInterface.GetName() == "Project";
        int nInTime = File(sIn).GetTime();
        int nOutTime = File(sOut).GetTime();

        if (bIsStaticTemplate)
        {
          if (!bNeedUpdate && nOutTime && nOutTime > nInTime)
          {
            std::cout << "Skipping " << sOut << std::endl;
            return;
          }
        }
        else
        {
          if (nOutTime && nOutTime > nInTime)
          {
            int nInterfaceTime =
                File(sOutDir + rElementInterface.GetChildElementByName("FileName").GetTextValue()).GetTime();

            if (nOutTime && nOutTime > nInterfaceTime)
            {
              std::cout << "Skipping " << sOut << std::endl;
              return;
            }
            else
            {
              bNeedUpdate = true;
            }
          }
        }
      }

      {
        std::ifstream fsIn;
        std::ofstream fsOut;

        fsIn.open(sIn.c_str());
        STAFF_ASSERT(fsIn.good(), "can't open input file: " + sIn);

        fsOut.open(sOut.c_str());
        if (!fsOut.good())
        {
          fsIn.close();
          STAFF_THROW_ASSERT("can't open output file: " + sOut);
        }

        std::cout << "Generating " << sOut << std::endl;

        m_nLine = 0;
        m_nIndent = 0;
        m_bNeedIndent = true;
        Process(fsIn, fsOut, rElementInterface);

        fsIn.close();
        fsOut.close();
      }
    }

    void SetEnv(const StringMap& rmEnv)
    {
      m_tmVariables.top() = rmEnv;
    }

  private:
    std::list<std::string> m_tTemplateFileList;
    std::list<std::string> m_tConstFileList;
    mutable std::stack<StringMap> m_tmVariables;
    std::string m_sInDir;
    std::string m_sOutDir;
    int m_nLine;
    int m_nIndent;
    bool m_bNeedIndent;
    bool m_bHasConfig;
  };


  void CodeGen::Start(const std::string& sTemplateDir, const std::string& sOutDir,
                      const xml::Element& rRootElement, bool bUpdateOnly, const StringMap& rmEnv)
  {
    TemplateParser tTemplateParser;

    File(sOutDir).Mkdirs();

    tTemplateParser.Init(sTemplateDir);
    tTemplateParser.SetEnv(rmEnv);
    tTemplateParser.Start(sOutDir, rRootElement, bUpdateOnly);
  }
}
}
