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
#include <staff/utils/Mutex.h>
#include <staff/utils/Thread.h>
#include <staff/utils/Exception.h>
#include <staff/utils/StackTracer.h>
#include <staff/utils/CrashHandler.h>
#include <staff/utils/File.h>

int nSuccessed = 0;
int nFailed = 0;

#define TestNoEx(TEXT, EXPR) \
{\
if (EXPR)\
{\
   staff::LogInfo() << TEXT": " << #EXPR << staff::LogResultSuccess;\
  ++nSuccessed;\
}\
else\
{\
  staff::LogInfo() << TEXT": " << #EXPR << staff::LogResultFailed;\
  ++nFailed;\
}\
}

#define TestEx(TEXT, TEST_CODE) \
{\
bool bTestSuccess = false;\
try\
{\
  TEST_CODE;\
  bTestSuccess = true;\
} catch(...) {}\
TestNoEx(TEXT, bTestSuccess);\
}

#define TestShouldEx(TEXT, TEST_CODE) \
{\
bool bTestSuccess = false;\
try\
{\
  TEST_CODE;\
} catch(...)\
{\
  bTestSuccess = true;\
}\
TestNoEx(TEXT, bTestSuccess);\
}

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



class MockThread: public staff::Thread
{
public:
	staff::Mutex & m_mutex1;	
	staff::Mutex & m_mutex2;
	int & m_testVar;

	MockThread(staff::Mutex &mutex1,staff::Mutex &mutex2, int &testVar):m_mutex1(mutex1),m_mutex2(mutex2),m_testVar(testVar)
	{
	}
	
	virtual ~MockThread()
	{
	}
	
	virtual void Run()
	{
		m_mutex2.Lock();
		m_testVar++;
		m_mutex1.Unlock();
		
		//Block until the second thread is released
		m_mutex2.Lock();
		Join();
	}
	

};

void testMutex()
{
	staff::Mutex mutex1;
	staff::Mutex mutex2;
	
	mutex1.Lock();
	TestNoEx("Test Mutex TryLock after locking first mutex",!mutex1.TryLock());
	mutex1.Unlock();
	TestNoEx("Test Mutex TryLock after unlocking first mutex",mutex1.TryLock());
	
	mutex2.Lock();
	TestNoEx("Test Mutex TryLock after locking second mutex",!mutex2.TryLock());
	mutex2.Unlock();
	mutex1.Unlock();
}

void testThreads()
{
	int testVar = 1; //Initial value of the variable
	staff::Mutex mutex1;
	staff::Mutex mutex2;
	MockThread thread1(mutex1,mutex2,testVar);
	MockThread thread2(mutex1,mutex2,testVar);

	mutex1.Lock();
	
	TestNoEx("Test Thread1::IsRunning() when thread not running",!thread1.IsRunning());
	TestNoEx("Test Thread1::Start()",thread1.Start());
	//Block until the thread unlocks the first mutex
	mutex1.Lock(); 
	
	TestNoEx("Test Thread1::IsStopped() when thread running",!thread1.IsStopping());
	TestNoEx("Test Thread1::IsRunning() when thread running",thread1.IsRunning());
	TestNoEx("Test that the Thread1 ran",(testVar == 2));
	mutex2.Unlock();

	
	TestNoEx("Test wait for Thread1",thread1.Wait());
	mutex2.Unlock();
	mutex1.Unlock();
	
	mutex1.Lock();
	TestNoEx("Test Thread2::Start()",thread2.Start());
	mutex1.Lock();
	TestNoEx("Test Thread2::Stop()",thread2.Terminate());
	
	mutex2.Unlock();
	mutex1.Unlock();
}

int main(int, char**)
{
  try
  {
    Header("test");
    testMutex();
    testThreads();
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
