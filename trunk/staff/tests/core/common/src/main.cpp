#include <iostream>
#include <rise/common/exmacros.h>
#include <rise/common/Exception.h>
#include <rise/common/Log.h>
#include <staff/common/DataObject.h>

int main()
{
  int nSuccessed = 0;
  int nFailed = 0;

  #define TestNoEx(TEXT, EXPR) \
  {\
    if (EXPR)\
    {\
       rise::LogInfo() << TEXT": " << #EXPR << rise::LogResultSuccess;\
      ++nSuccessed;\
    }\
    else\
    {\
      rise::LogInfo() << TEXT": " << #EXPR << rise::LogResultFailed;\
      ++nFailed;\
    }\
  }

  #define Test(TEXT, EXPR) \
  {\
    try\
    {\
      TestNoEx(TEXT, EXPR);\
    } catch(...)\
    {\
      TestNoEx(TEXT ": FAILED BY EXCEPTION", #EXPR);\
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

  try
  {
    staff::CDataObject tdoTest;

    TestEx("KNOWN BUG in axiom/libxml: DataObject.FromString fails when xml size < 45 bytes", tdoTest.FromString("<test>test test test test test test..</test>"));

    { // mutable iterator
      // test --DataObject.End() when childs exists
      TestEx("Load xml from string", tdoTest.FromString("<root><sub1>text1</sub1><sub2>text2</sub2><sub3>text3</sub3></root>"));

      staff::CDataObject::Iterator itItem = tdoTest.End();

      Test("DataObject.End() == DataObject.End() if childs exists", itItem == tdoTest.End());
      Test("DataObject.End() != DataObject.End() if childs exists", itItem != tdoTest.Begin());

      --itItem;

      Test("--DataObject.End() != DataObject.End() if childs exists", itItem != tdoTest.End());
      Test("Get last element via --DataObject.End()", itItem->GetLocalName() == "sub3");
      Test("Get last element text via --DataObject.End()", itItem->GetText() == "text3");


      // test --DataObject.End() when no childs exists
      TestEx("Load xml from string", tdoTest.FromString("<test>test test test test test test test</test>")); // due to bug in axiom/libxml...

      itItem = tdoTest.End();

      Test("DataObject.End() == DataObject.End() if no childs exists", itItem == tdoTest.End());
      Test("DataObject.End() == DataObject.Begin() if no childs exists", itItem == tdoTest.Begin());

      TestShouldEx("--DataObject.End() should throw an exception if no childs exists", --itItem);
    }

    { // const iterator
      const staff::CDataObject& rdoTest = tdoTest;

      // test --DataObject.End() when childs exists
      TestEx("Load xml from string", tdoTest.FromString("<root><sub1>text1</sub1><sub2>text2</sub2><sub3>text3</sub3></root>"));

      staff::CDataObject::ConstIterator itItem = rdoTest.End();

      Test("DataObject.End() == DataObject.End() if childs exists", itItem == rdoTest.End());
      Test("DataObject.End() != DataObject.End() if childs exists", itItem != rdoTest.Begin());

      --itItem;

      Test("--DataObject.End() != DataObject.End() if childs exists", itItem != rdoTest.End());
      Test("Get last element via --DataObject.End()", itItem->GetLocalName() == "sub3");
      Test("Get last element text via --DataObject.End()", itItem->GetText() == "text3");


      // test --DataObject.End() when no childs exists
      TestEx("Load xml from string", tdoTest.FromString("<test>test test test test test test test</test>")); // due to bug in axiom/libxml...

      itItem = rdoTest.End();

      Test("DataObject.End() == DataObject.End() if no childs exists", itItem == rdoTest.End());
      Test("DataObject.End() == DataObject.Begin() if no childs exists", itItem == rdoTest.Begin());

      TestShouldEx("--DataObject.End() should throw an exception if no childs exists", --itItem);
    }

  }
  RISE_CATCH_ALL

  rise::LogInfo() << "\n\n\033[1mTests successed: " << nSuccessed << "\nTests failed: " << nFailed << "\033[0m";

  return 0;
}
