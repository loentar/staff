#ifndef _CLIENTGEN_H_
#define _CLIENTGEN_H_

struct SProject;

namespace rise 
{
  namespace xml
  {
    class CXMLNode;
  }
}

//!  ClientGen
class CCodeGen
{
public:
  void Start(const std::string& sTemplateDir, const std::string& sOutDir, const rise::xml::CXMLNode& rRootNode, bool bUpdateOnly);
};

#endif // _CLIENTGEN_H_
