/*
 *  Copyright 2010 Utkin Dmitry
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

#ifndef _DATASOURCEFACTORY_H_
#define _DATASOURCEFACTORY_H_

#include <string>
#include <list>
#include "staffdascommonexport.h"

namespace staff
{
namespace das
{
  class DataSource;

  typedef std::list<std::string> StringList;

  class STAFF_DAS_COMMON_EXPORT DataSourceFactory
  {
  public:
    static DataSourceFactory& Inst();
    static void FreeInst();

    const DataSource& GetDataSource(const std::string& sName) const;
    void GetDataSources(StringList& rlsSources) const;

  private:
    DataSourceFactory();
    ~DataSourceFactory();

  private:
    static DataSourceFactory* m_pInst;
    class DataSourceFactoryImpl;
    DataSourceFactoryImpl* m_pImpl;
  };
}
}

#endif // _DATASOURCEFACTORY_H_
