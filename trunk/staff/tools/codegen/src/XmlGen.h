#ifndef _XMLGEN_H_
#define _XMLGEN_H_

struct SProject;
namespace rise 
{
  namespace xml
  {
    class CXMLNode;
  }
}

rise::xml::CXMLNode& operator<<(rise::xml::CXMLNode& rRootNode, const SProject& rProject);

#endif // _XMLGEN_H_
