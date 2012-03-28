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
#include <staff/xml/Node.h>
#include <staff/xml/Element.h>
#include <staff/xml/Document.h>
#include <staff/xml/XmlWriter.h>
#include <staff/xml/XmlReader.h>

void Header(const std::string& sText)
{
  std::cout
    << "\n\n---------------------------------------------------------------\n"
    << sText
    << "\n---------------------------------------------------------------\n";
}


int main()
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
    // writer
    {
      staff::xml::Document tDoc;
      staff::xml::Element& rRootElement = tDoc.GetRootElement();
      rRootElement.SetName("xml-test");
      rRootElement.SetValue("xml-test");
      rRootElement.DeclareDefaultNamespace("urn:staff-xml-writer-test");
      rRootElement.DeclareNamespace("urn:some-other-ns", "ns1");

      rRootElement.CreateChildComment("_ here is some comment: < \"testing\" escape & 'writing' > _");
      rRootElement.CreateChildElement("child.with.text", "text").CreateChildElement("subchild").AppendChild(new staff::xml::Comment("some comment"));
      rRootElement.CreateChildText("_ here is some text _");
      rRootElement.CreateChildCdata("_ < \"testing\" escape & 'writing' > _");
      rRootElement.AppendChild(new staff::xml::Text("_ 'TEST' _"));
      rRootElement.CreateChildElement("empty_child");
      rRootElement.CreateChildElement("empty_child_with_ns").SetNamespace("urn:empty-child-ns", "ens");
      rRootElement.CreateChildText("_ < \"testing\" escape & 'writing' > _");

      staff::xml::XmlFileWriter tXmlWriter("test_output.xml");
      tXmlWriter.WriteDocument(tDoc);

      staff::xml::XmlFileWriter tXmlWriterNf("test_output_nf.xml", false);
      tXmlWriterNf.WriteDocument(tDoc);


      // iterate childs
      for (staff::xml::Node* pNode = rRootElement.GetFirstChild(); pNode; pNode = pNode->GetNextSibling())
      {
        staff::LogInfo() << "Node. Type = " << pNode->GetType() << ", value=[" << pNode->GetValue() << "]";
      }
    }

    {
      staff::xml::Document tDoc;

      staff::xml::XmlFileReader tXmlReader("test_output_nf.xml", false);

      tXmlReader.ReadDocument(tDoc);

      staff::xml::XmlFileWriter tXmlWriterNf("test_output_nf_readed.xml", false);
      tXmlWriterNf.WriteDocument(tDoc);
    }
  }
  STAFF_CATCH_ALL;

  return 0;
}
