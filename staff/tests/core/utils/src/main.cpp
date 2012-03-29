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

#include <iostream>
#include <list>
#include <staff/utils/Log.h>
#include <staff/utils/Exception.h>
#include <staff/utils/StackTracer.h>
#include <staff/utils/File.h>

void Header(const std::string& sText)
{
  std::cout
    << "\n\n---------------------------------------------------------------\n"
    << sText
    << "\n---------------------------------------------------------------\n";

  std::string sTracedStack;
  staff::StackTracer::GetStackTraceStr(sTracedStack);
  staff::LogInfo() << "Traced stack:\n" << sTracedStack;
}


int main(int, char**)
{
  int nSuccessed = 0;
  int nFailed = 0;

  #define Test(TEXT, EXPR) \
    if (EXPR)\
    {\
       staff::LogInfo() << TEXT": " << #EXPR << staff::LogResultSuccess;\
      ++nSuccessed;\
    }\
    else\
    {\
      staff::LogInfo() << TEXT": " << #EXPR << staff::LogResultFailed;\
      ++nFailed;\
    }

  try
  {
    Header("test");
  }
  STAFF_CATCH_ALL;

  staff::StringList lsFiles;
  try
  {
    STAFF_ASSERT_PARAM(staff::File(".").GetAttributes() == staff::File::AttributeDirectory);
    STAFF_ASSERT_PARAM(staff::File("src").GetAttributes() == staff::File::AttributeDirectory);
    STAFF_ASSERT_PARAM((staff::File("Makefile").GetAttributes() & staff::File::AttributeAnyFile) == staff::File::AttributeRegularFile);
    STAFF_ASSERT_PARAM((staff::File("Makefile").GetAttributes() & staff::File::AttributeRegularFile) == staff::File::AttributeRegularFile);
    STAFF_ASSERT_PARAM(staff::File("non exsiting file or dir").GetAttributes() == staff::File::AttributeNone);

    staff::File("src").List(lsFiles, "*", staff::File::AttributeRegularFile);
    STAFF_ASSERT_PARAM(!lsFiles.empty());

    staff::File("src").List(lsFiles, "_*", staff::File::AttributeDirectory);
    STAFF_ASSERT_PARAM(lsFiles.empty());

    staff::File(".").List(lsFiles, "*", staff::File::AttributeDirectory);
    STAFF_ASSERT_PARAM(!lsFiles.empty());
  }
  STAFF_CATCH_ALL;

  return 0;
}
