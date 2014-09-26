#include <iostream>
#include <staff/utils/Log.h>
#include <staff/common/Exception.h>
#include <staff/common/DataObject.h>
#include <staff/common/Attribute.h>
#include <staff/common/Namespace.h>


void LeakCheck()
{ // leak checks
  staff::DataObject tdoTest;
  const staff::DataObject& tdoTestConst = tdoTest;
  tdoTest.FromString("<a attr1=\"asd\" attr2=\"asd\" attr3=\"asd\" attr4=\"asd\" xmlns:a=\"1\" xmlns:b=\"1\" xmlns:c=\"1\"><b/><c/><d/></a>");

  // test 1 attr
  for (int i = 0; i < 5; ++i) {
    staff::Attribute it = tdoTest.GetAttributeByLocalName("attr2");
    if (!it.IsNull())
      it.GetText();
  }

  // test 2 attr iter
  for (int i = 0; i < 5; ++i) {
    int p = 0;
    for (staff::DataObject::AttributeIterator it = tdoTest.AttributeBegin();
         it != tdoTest.AttributeEnd(); ++it, ++p) {
        if (p == 2)
          break;
        it->GetText();
    }

    for (staff::DataObject::AttributeIterator it = tdoTest.AttributeBegin();
         it != tdoTest.AttributeEnd(); ++it) {
        it->GetText();
    }

    p = 0;
    for (staff::DataObject::ConstAttributeIterator it = tdoTestConst.AttributeBegin();
         it != tdoTestConst.AttributeEnd(); ++it, ++p) {
        if (p == 2)
          break;
        it->GetText();
    }

    for (staff::DataObject::ConstAttributeIterator it = tdoTestConst.AttributeBegin();
         it != tdoTestConst.AttributeEnd(); ++it) {
        it->GetText();
    }
  }

  // test 3 NS
  for (int i = 0; i < 5; ++i) {
    int p = 0;
    for (staff::DataObject::NamespaceIterator it = tdoTest.NamespaceBegin();
         it != tdoTest.NamespaceEnd(); ++it, ++p) {
        if (p == 2)
          break;
        it->GetPrefix();
    }


    for (staff::DataObject::NamespaceIterator it = tdoTest.NamespaceBegin();
         it != tdoTest.NamespaceEnd(); ++it, ++p) {
        it->GetPrefix();
    }

    p = 0;
    for (staff::DataObject::ConstNamespaceIterator it = tdoTestConst.NamespaceBegin();
         it != tdoTestConst.NamespaceEnd(); ++it, ++p) {
        if (p == 2)
          break;
        it->GetPrefix();
    }


    for (staff::DataObject::ConstNamespaceIterator it = tdoTestConst.NamespaceBegin();
         it != tdoTestConst.NamespaceEnd(); ++it, ++p) {
        it->GetPrefix();
    }
  }

}

int main()
{
  LeakCheck();

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
    staff::DataObject tdoTest;

    TestEx("KNOWN BUG in axiom/libxml: DataObject.FromString fails when xml size < 45 bytes", tdoTest.FromString("<test>test test test test test test..</test>"));

    { // mutable iterator
      // test --DataObject.End() when childs exists
      TestEx("Load xml from string", tdoTest.FromString("<root><sub1>text1</sub1><sub2>text2</sub2><sub3>text3</sub3></root>"));

      staff::DataObject::Iterator itItem = tdoTest.End();

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
      const staff::DataObject& rdoTest = tdoTest;

      // test --DataObject.End() when childs exists
      TestEx("Load xml from string", tdoTest.FromString("<root><sub1>text1</sub1><sub2>text2</sub2><sub3>text3</sub3></root>"));

      staff::DataObject::ConstIterator itItem = rdoTest.End();

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
  STAFF_CATCH_ALL

  staff::LogInfo() << "\n\n\033[1mTests successed: " << nSuccessed << "\nTests failed: " << nFailed << "\033[0m";

  return 0;
}
