#include <iostream>
#include <rise/common/Log.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include "XMLNode.h"
#include "XMLException.h"

namespace rise 
{
  namespace xml
  {
    /////////////////////////////////////////////////////////////////////////////
    //    CLASS:        CXMLNode
    //    DESCRIPTION:  узел XML
    /////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLNode
    //    DESCRIPTION:    конструктор по умолчанию
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::CXMLNode(CXMLNode* pParentNode /*= NULL*/):
      m_pNodeParent(pParentNode), m_eNodeType(ENTGENERIC)
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLNode(...)
    //    DESCRIPTION:    конструктор с параметрами
    //    PARAMETRS:      (in) sNodeText - имя создаваемого узла, 
    //                         если eNodeType = ENTGENERIC, иначе контент узла
    //                    (in) eNodeType - тип узла
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::CXMLNode(const CString& sNodeText, CXMLNode* pParentNode /*= NULL*/, ENodeType eNodeType /*= ENTGENERIC*/): 
      m_pNodeParent(pParentNode), m_eNodeType(eNodeType)
    {
      if ( eNodeType == ENTGENERIC )
        SetNodeNsName(sNodeText);
      else
        m_sContent = sNodeText;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    CONSTRUCTOR:    CXMLNode(...)
    //    DESCRIPTION:    копирующий конструктор
    //    PARAMETRS:      (in) rNode - ссылка на существующий узел
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::CXMLNode(const CXMLNode& rNode)
    {
      *this = rNode;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    DESTRUCTOR:     ~CXMLNode
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::~CXMLNode() 
    {
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       =
    //    DESCRIPTION:    оператор копирования
    //    PARAMETRS:      (in) rNode - исходный узел
    //    RETURN:         копия узла
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode& CXMLNode::operator=(const CXMLNode& rNode)
    {
      m_sName = rNode.m_sName;
      m_sNamespace = rNode.m_sNamespace;
      m_sContent = rNode.m_sContent;
      m_tAttr = rNode.m_tAttr;
      m_tSubNodes = rNode.m_tSubNodes;
      m_eNodeType = rNode.m_eNodeType;
      m_pNodeParent = rNode.m_pNodeParent;

      return *this;
    }

    //!         получить родительский узел
    /*! \return указатель на родительский узел или NULL
        */
    CXMLNode* CXMLNode::GetParent()
    {
      return m_pNodeParent;
    }

    //!         получить родительский узел
    /*! \return const-указатель на родительский узел или NULL
        */
    const CXMLNode* CXMLNode::GetParent() const
    {
      return m_pNodeParent;
    }

    void CXMLNode::SetNodeNsName( const CString& sNsName )
    {
      TSize nPos = sNsName.find(':');
      if (nPos != CString::npos)
      {
        StrMid(sNsName, m_sName, nPos + 1);
        StrLeft(sNsName, m_sNamespace, nPos);
      }
      else
        m_sName = sNsName;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Attribute()
    //    DESCRIPTION:    функция доступа к атрибутам по имени
    //    PARAMETRS:      (in) sName - имя атрибута
    //    RETURN:         значение атрибута
    //    EXCEPTIONS:     CLogicNoItemException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CXMLValue& CXMLNode::Attribute( const CString& sName ) const
    {
      for ( TXMLAttrConstIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return itAttr->sAttrValue;

      RISE_THROW(CLogicNoItemException);
    }

    CXMLValue& CXMLNode::Attribute(const CString& sName)
    {
      for ( TXMLAttrIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return itAttr->sAttrValue;

      RISE_THROW(CLogicNoItemException);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       FindAttribute()
    //    DESCRIPTION:    функция поиска атрибута по имени
    //    PARAMETRS:      (in) sName - имя атрибута
    //    RETURN:         итератор на найденный атрибут с заданным именем
    //                    если атрибут не найден, то итератор == AttrEnd()
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLAttrConstIterator CXMLNode::FindAttribute(const CString& sName) const
    {
      TXMLAttrConstIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && itAttr->sAttrName != sName; ++itAttr);
      return itAttr;
    }

    CXMLNode::TXMLAttrIterator CXMLNode::FindAttribute(const CString& sName)
    {
      TXMLAttrIterator itAttr = AttrBegin();
      for ( ; itAttr != AttrEnd() && itAttr->sAttrName != sName; ++itAttr);
      return itAttr;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsAttributeExists()
    //    DESCRIPTION:    существует ли атрибут с заданным именем
    //    PARAMETRS:      (in) sName - имя атрибута
    //    RETURN:         true - атрибут существует
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CXMLNode::IsAttributeExists(const CString& sName) const
    {
      for ( TXMLAttrConstIterator itAttr = AttrBegin(); itAttr != AttrEnd(); ++itAttr)
        if (itAttr->sAttrName == sName)
          return true;

      return false;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       FindSubnode(...)
    //    DESCRIPTION:    функция поиска подузлов по имени
    //    PARAMETRS:      (in) sName - имя подузла
    //                    (in) itNode - итератор, с которого следует произвести поиск
    //    RETURN:         итератор на первый назденный подузел с заданным именем
    //                    если атрибут не найден, то итератор == AttrEnd()
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLNodeConstIterator CXMLNode::FindSubnode(const CString& sName) const
    {
      return FindSubnode(sName, NodeBegin());
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindSubnode(const CString& sName)
    {
      return FindSubnode(sName, NodeBegin());
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindSubnode(const CString& sName, TXMLNodeConstIterator itNode) const
    {
      for ( ; itNode != NodeEnd() && itNode->NodeName() != sName; ++itNode);

      return itNode;
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindSubnode(const CString& sName, TXMLNodeIterator itNode)
    {
      for ( ; itNode != NodeEnd() && itNode->NodeName() != sName; ++itNode);

      return itNode;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Subnode(...)
    //    DESCRIPTION:    функция доступа к подузлам по имени
    //    PARAMETRS:      (in) sName - имя подузла
    //    RETURN:         подузел
    //    EXCEPTIONS:     CLogicNoItemException, если атрибут не найден
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CXMLNode& CXMLNode::Subnode(const CString& sName) const
    {
      TXMLNodeConstIterator itNode = FindSubnode(sName);
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "Node: [" + sName + "] not found");
      return *itNode;
    }

    CXMLNode& CXMLNode::Subnode(const CString& sName)
    {
      TXMLNodeIterator itNode = FindSubnode(sName);
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "Node: [" + sName + "] not found");
      return *itNode;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       IsSubnodeExists()
    //    DESCRIPTION:    существует ли подузел с заданным именем
    //    PARAMETRS:      (in) sName - имя подузла
    //    RETURN:         true - подузел с заданным именем существует
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    bool CXMLNode::IsSubnodeExists(const CString& sName) const
    {
      for ( TXMLNodeConstIterator itNode = NodeBegin(); itNode != NodeEnd(); ++itNode)
        if (itNode->NodeName() == sName)
          return true;

      return false;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       NodeBegin()
    //    DESCRIPTION:    получение итератора на первый подузел
    //    PARAMETRS:      none
    //    RETURN:         итератор на первый подузел
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLNodeConstIterator CXMLNode::NodeBegin() const
    {
      return m_tSubNodes.begin();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::NodeBegin()
    {
      return m_tSubNodes.begin();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       NodeEnd()
    //    DESCRIPTION:    получение итератора на последний подузел
    //    PARAMETRS:      none
    //    RETURN:         итератор на последний подузел
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLNodeConstIterator CXMLNode::NodeEnd() const
    {
      return m_tSubNodes.end();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::NodeEnd()
    {
      return m_tSubNodes.end();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AttrBegin()
    //    DESCRIPTION:    получение итератора на первый атрибут
    //    PARAMETRS:      none
    //    RETURN:         итератор на первый атрибут
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLAttrConstIterator CXMLNode::AttrBegin() const
    {  
      return m_tAttr.begin();
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AttrBegin()
    {  
      return m_tAttr.begin();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AttrEnd()
    //    DESCRIPTION:    получение итератора на атрибут за последним атрибут
    //    PARAMETRS:      none
    //    RETURN:         итератор на атрибут за последним атрибутом
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLAttrConstIterator CXMLNode::AttrEnd() const
    {
      return m_tAttr.end();
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AttrEnd()
    {
      return m_tAttr.end();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       NodeName() 
    //    DESCRIPTION:    получение/установка имени узла
    //    PARAMETRS:      none
    //    RETURN:         ссылка на имя узла
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CString& CXMLNode::NodeName() const
    {
      return m_sName;
    }

    CString& CXMLNode::NodeName()
    {
      return m_sName;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       NodeContent() 
    //    DESCRIPTION:    получение/установка контента узла
    //    PARAMETRS:      none
    //    RETURN:         ссылка на контент текущего узла
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CXMLValue& CXMLNode::NodeContent() const
    {
      return m_sContent;
    }

    CXMLValue& CXMLNode::NodeContent()
    {
      return m_sContent;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       operator[]
    //    DESCRIPTION:    оператор доступа к значению подузла
    //    PARAMETRS:      sName - имя подузла
    //    RETURN:         ссылка на контент подузла
    //    EXCEPTIONS:     rise::CLogicNoItemException
    //    COMMENT:        не-const функция добавляет подузел, если такового не существует
    //////////////////////////////////////////////////////////////////////////////      
    const CXMLValue& CXMLNode::operator[](const CString& sName) const
    {
      return Subnode(sName).NodeContent();
    }

    CXMLValue& CXMLNode::operator[](const CString& sName)
    {
      return GetOrAddSubNode(sName).NodeContent();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       GetOrAddSubNode
    //    DESCRIPTION:    добавление подузла с заданным именем, если узла с таким именем еще не существует
    //    PARAMETRS:      (in) sNodeName - имя создаваемого узла, 
    //    RETURN:         ссылка на добавленный или найденный подузел
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode& CXMLNode::GetOrAddSubNode(const CString& sNodeName)
    {
      TXMLNodeIterator itNode = FindSubnode(sNodeName);
      if ( itNode != NodeEnd()) 
        return *itNode;
      else
        return AddSubNode(sNodeName);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       NodeType
    //    DESCRIPTION:    получение/установка типа узла
    //    PARAMETRS:      none
    //    RETURN:         ссылка на тип узла
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    const CXMLNode::ENodeType& CXMLNode::NodeType() const
    {
      return m_eNodeType;
    }

    CXMLNode::ENodeType& CXMLNode::NodeType()
    {
      return m_eNodeType;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       SubNodesCount()
    //    DESCRIPTION:    получение количества подузлов
    //    PARAMETRS:      none
    //    RETURN:         количество подузлов
    //    EXCEPTIONS:     none
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    int CXMLNode::SubNodesCount() const
    {
      return (int)m_tSubNodes.size();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       FindMatch()
    //    DESCRIPTION:    поиск подузла с заданным значением атрибута
    //    PARAMETRS:      (in) sNodeName - имя подузла
    //                    (in) sAttrName - имя атрибута
    //                    (in) sAttrValue - значение атрибута
    //    RETURN:         итератор на найденный подузел
    //    EXCEPTIONS:     CLogicNoItemException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute) const
    {
      return FindNodeMatch(sNodeName, stAttribute, FindSubnode(sNodeName));
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute)
    {
      return FindNodeMatch(sNodeName, stAttribute, FindSubnode(sNodeName));
    }

    CXMLNode::TXMLNodeConstIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute,
      TXMLNodeConstIterator itNode) const
    {
      // ищем ноду с подходящем именем
      for(; itNode != NodeEnd(); itNode = FindSubnode(sNodeName, itNode))
      {
        // ищем аттрибут с подходящем именем
        TXMLAttrConstIterator itAttr = itNode->FindAttribute(stAttribute.sAttrName);
        // сравниваем значение
        if ((itAttr != itNode->AttrEnd()) && (*itAttr == stAttribute))
          return itNode;
        ++itNode;
      }

      return NodeEnd();
    }

    CXMLNode::TXMLNodeIterator CXMLNode::FindNodeMatch(
      const CString& sNodeName,
      const SXMLAttribute& stAttribute,
      TXMLNodeIterator itNode)
    {
      // ищем ноду с подходящем именем
      for(; itNode != NodeEnd(); itNode = FindSubnode(sNodeName, itNode))
      {
        // ищем аттрибут с подходящем именем
        TXMLAttrIterator itAttr = itNode->FindAttribute(stAttribute.sAttrName);
        // сравниваем значение
        if ((itAttr != itNode->AttrEnd()) && (*itAttr == stAttribute))
          return itNode;
        ++itNode;
      }

      return NodeEnd();
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AddAttribute(...)
    //    DESCRIPTION:    добавление атрибута
    //    PARAMETRS:      (in) sAttrName - имя атрибута
    //                    (in) sAttrValue - значение атрибута
    //    RETURN:         ссылка на атрибут
    //    EXCEPTIONS:     CLogicAlreadyExistsException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    SXMLAttribute& CXMLNode::AddAttribute( const CString& sAttrName, const CXMLValue& sAttrValue )
    {
      return AddAttribute(SXMLAttribute(sAttrName, sAttrValue));
    }

    SXMLAttribute& CXMLNode::AddAttribute(const SXMLAttribute& stAttribute)
    {
      RISE_ASSERTE(!IsAttributeExists(stAttribute.sAttrName), CLogicAlreadyExistsException);

      return *m_tAttr.insert(AttrEnd(), stAttribute);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AddAttribute(...)
    //    DESCRIPTION:    добавление атрибута
    //    PARAMETRS:      (in) stAttribute - атрибут
    //                    (in) itAttr - позиция куда будет вставлен указанный атрибут
    //    RETURN:         итератор
    //    EXCEPTIONS:     CLogicAlreadyExistsException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode::TXMLAttrIterator CXMLNode::AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue, TXMLAttrIterator itAttr)
    {
      return AddAttribute(SXMLAttribute(sAttrName, sAttrValue), itAttr);
    }

    CXMLNode::TXMLAttrIterator CXMLNode::AddAttribute(const SXMLAttribute& stAttribute, TXMLAttrIterator itAttr)
    {
      RISE_ASSERTE(!IsAttributeExists(stAttribute.sAttrName), CLogicAlreadyExistsException);

      return m_tAttr.insert(itAttr, stAttribute);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       DelAttribute(...)
    //    DESCRIPTION:    удаление атрибута
    //    PARAMETRS:      (in) itAttr - позиция удаляемого атрибута
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoItemException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLNode::DelAttribute(CXMLNode::TXMLAttrIterator itAttr)
    {
      RISE_ASSERTES(itAttr != AttrEnd(), CLogicNoItemException, "invalid iterator value");
      m_tAttr.erase(itAttr);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       AddSubNode
    //    DESCRIPTION:    добавление подузла
    //    PARAMETRS:      sNodeName - имя подузла
    //    RETURN:         none
    //    EXCEPTIONS:     none
    //    COMMENT:        если тип узла не ENTGENERIC, sNodeName будет служить контентом
    //////////////////////////////////////////////////////////////////////////////
    CXMLNode& CXMLNode::AddSubNode(const CString& sNodeText, CXMLNode::ENodeType eNodeType/* = ENTGENERIC*/)
    {
      return *m_tSubNodes.insert(NodeEnd(), CXMLNode(sNodeText, this, eNodeType));
    }

    CXMLNode& CXMLNode::AddSubNode(const CString& sNodeText, TXMLNodeIterator itNode, ENodeType eNodeType/* = ENTGENERIC*/)
    {
      return *m_tSubNodes.insert(itNode, CXMLNode(sNodeText, this, eNodeType));
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       DelSubNode(...)
    //    DESCRIPTION:    удаление подузла
    //    PARAMETRS:      (in) itAttr - позиция удаляемого атрибута
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoItemException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLNode::DelSubNode(CXMLNode::TXMLNodeIterator itNode)
    {
      RISE_ASSERTES(itNode != NodeEnd(), CLogicNoItemException, "invalid iterator value");
      m_tSubNodes.erase(itNode);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       Clear
    //    DESCRIPTION:    удаление всех подузлов и атрибутов
    //    PARAMETRS:      none
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicNoItemException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    void CXMLNode::Clear()
    {
      m_tSubNodes.clear();
      m_tAttr.clear();
    }

    const CXMLNode::TXMLNsList& CXMLNode::GetNsList() const
    {
      return m_tNsList;
    }

    CXMLNode::TXMLNsList& CXMLNode::GetNsList()
    {
      return m_tNsList;
    }

    CXMLNode* CXMLNode::FindNamespaceDecl( const CString& sNamespace )
    {
      for(CXMLNode* pNode = this; pNode != NULL; pNode = pNode->GetParent())
      {
        const TXMLNsList& rNsList = pNode->GetNsList();
        for (TXMLNsList::const_iterator itNs = rNsList.begin(); itNs != rNsList.end(); ++itNs)
          if (itNs->sNs == sNamespace)
            return pNode;
      }
      return NULL;
    }

    const CXMLNode* CXMLNode::FindNamespaceDecl( const CString& sNamespace ) const
    {
      for(const CXMLNode* pNode = this; pNode != NULL; pNode = pNode->GetParent())
      {
        const TXMLNsList& rNsList = pNode->GetNsList();
        for (TXMLNsList::const_iterator itNs = rNsList.begin(); itNs != rNsList.end(); ++itNs)
          if (itNs->sNs == sNamespace)
            return pNode;
      }
      return NULL;
    }

    const CString& CXMLNode::Namespace() const
    {
      return m_sNamespace;
    }

    CString& CXMLNode::Namespace()
    {
      return m_sNamespace;
    }

    CString CXMLNode::GetNodeNsName() const
    {
      return m_sNamespace == "" ? m_sName : m_sNamespace + ":" + m_sName;
    }

    // class CXMLNode /////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       WriteNode
    //    DESCRIPTION:    запись узла
    //    PARAMETRS:      (in) rXMLNode - ссылка на узел XML-дерева
    //    RETURN:         none
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream& WriteNode(CXMLOStream& rStream, const CXMLNode& rXMLNode, int nLevel = 0 )
    {
      for ( int i = 0; i < nLevel; ++i )
        rStream << "  ";

      rStream << "<";
      switch ( rXMLNode.NodeType() )
      {
        case CXMLNode::ENTGENERIC:
        {
          rStream << rXMLNode.GetNodeNsName();

          // write NS
          for (CXMLNode::TXMLNsConstIterator itNs = rXMLNode.GetNsList().begin(); 
            itNs != rXMLNode.GetNsList().end(); ++itNs)
            rStream << *itNs;

          // write attrs
          for (CXMLNode::TXMLAttrConstIterator itAttr = rXMLNode.AttrBegin(); itAttr != rXMLNode.AttrEnd(); ++itAttr)
            rStream << *itAttr;

          // 
          if ( (rXMLNode.SubNodesCount() == 0) && (rXMLNode.NodeContent() == "") )
            rStream << "/>\n";
          else
          {
            rStream << ">";
            rStream << rXMLNode.NodeContent();
            if ( rXMLNode.SubNodesCount() != 0 )
            {
              rStream << "\n";
              // subnodes

              for ( CXMLNode::TXMLNodeConstIterator itNode = rXMLNode.NodeBegin(); itNode != rXMLNode.NodeEnd(); ++itNode )
                WriteNode(rStream, *itNode, nLevel + 1);

              for ( int i = 0; i < nLevel; ++i )
                rStream << "  ";
            }
            rStream << "</" << rXMLNode.GetNodeNsName() << ">\n";
          }
        } break;
        case CXMLNode::ENTCDATA:
        {
          rStream << "![CDATA[" << rXMLNode.NodeContent().AsString() << "]]>\n";
        } break;
        case CXMLNode::ENTCOMMENT:
        {
          rStream << "!--" << rXMLNode.NodeContent() << "-->\n"; // CHECK SPACE!
        } break;
        default:
          THROWXML(CXMLFormatException, "Invalid Node Type: " + ToStr(rXMLNode.NodeType()) + 
                                         " NodeNsName=" + rXMLNode.GetNodeNsName(), rXMLNode.NodeType(), NULL, 0);
      }
      
      return rStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //                    (in) rXMLNode - ссылка на корневой узел для сериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLNode& rXMLNode)
    {
      return WriteNode(rStream, rXMLNode);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    FUNCTION:       ReadSubNodes
    //    DESCRIPTION:    рекурсивное чтение узла
    //    PARAMETRS:      (in)  rStream - поток
    //                    (out) rXMLNode - ссылка на узел XML-дерева к которому 
    //                            будут добавляться подузлы
    //    RETURN:         поток
    //    EXCEPTIONS:     CLogicFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream& ReadSubNodes(CXMLIStream& rStream, CXMLNode& rXMLNode)
    {
      SXMLAttribute stAttribute;
      CString sTmp;

      for(;;)
      {
        rStream.SkipWhitespace();
        // тег
        ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found", 
          rStream.GetFileName(), rStream.GetLine());

        if (rStream.Test("/")) // закрывающий тег
          return rStream;

        if (rStream.Test("!"))  // special
        {
          if (rStream.Test("[CDATA["))
          {
            rStream.ReadStringUntil(sTmp, "]]>");
            rXMLNode.AddSubNode(sTmp, CXMLNode::ENTCDATA);
          } else
          if (rStream.Test("--"))
          {
            rStream.ReadStringUntil(sTmp, "-->");
            rXMLNode.AddSubNode(sTmp, CXMLNode::ENTCOMMENT);
          } else
            THROWXML(CXMLFormatException, "Invalid Special id ", , rStream.GetFileName(), rStream.GetLine());
        } else
        {
          bool bIsEmptyTag = false;
          rStream.ReadId(sTmp);
          CXMLNode& rXMLNodeNew = rXMLNode.AddSubNode(sTmp);
          for(;;)  // reading attrs
          {
            rStream.SkipWhitespace();
            if(rStream.Test(">"))
              break;
            else
            if(rStream.Test("/>")) // пустой тег
            {
              bIsEmptyTag = true;
              break; // end reading node
            }
            
            rStream >> stAttribute;
            if (StrLeft(stAttribute.sAttrName, 6) == "xmlns:")
              rXMLNodeNew.GetNsList().push_back(SXMLNamespace(StrMid(const_cast<const SXMLAttribute&>(stAttribute).sAttrName, 6), 
                        const_cast<const SXMLAttribute&>(stAttribute).sAttrValue));
            else
              rXMLNodeNew.AddAttribute(const_cast<const SXMLAttribute&>(stAttribute));

          }  // while reading attrs

          // проверить ns
          if (rXMLNodeNew.Namespace() != "")
            if(rXMLNodeNew.FindNamespaceDecl(rXMLNodeNew.Namespace()) == NULL)
              LogDebug2() << "Could\'nt find xmlns declaration: \"" + rXMLNodeNew.Namespace() + "\"";

          if (!bIsEmptyTag)
          {
            rStream.AddContent(rXMLNodeNew.NodeContent()); // чтение контента узла
            if (!rStream.Test("</"))  // если не закрывающий тег, то подузел
              ReadSubNodes(rStream, rXMLNodeNew); // read subnodes

            rStream.ReadId(sTmp);
            ASSERTXMLS(sTmp == rXMLNodeNew.GetNodeNsName(), CXMLFormatException, 
              "Unmatched opening and closed tags: found </"
              + sTmp + "> expected: </" + rXMLNodeNew.GetNodeNsName() + ">", 
              rStream.GetFileName(), rStream.GetLine());
            rStream.SkipWhitespace();
            rStream.ReadStringUntil(sTmp, ">"); 
            ASSERTXMLS(sTmp == "", CXMLFormatException, "Unexpected text in closing tag:" + sTmp,
              rStream.GetFileName(), rStream.GetLine());
          }
        }
        rStream.AddContent(rXMLNode.NodeContent()); // чтение контента узла
      }
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //                    (in) rXMLNode - ссылка на корневой узел для десериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CXMLFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    CXMLIStream& operator>>(CXMLIStream& rStream, CXMLNode& rXMLNode)
    {
      CString sTmp;
      SXMLAttribute stAttribute;
     
      rStream.SkipWhitespace();
      ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found", 
        rStream.GetFileName(), rStream.GetLine());
      while(rStream.Test("!--"))  // !!!! комментарий перед корневым узлом
      {
        rStream.ReadStringUntil(sTmp, "-->");
        rStream.SkipWhitespace();
        ASSERTXMLS(rStream.Test("<"), CXMLFormatException, "open tag not found", 
          rStream.GetFileName(), rStream.GetLine());
      }

      rStream.ReadId(sTmp);
      rXMLNode.SetNodeNsName(sTmp);
      for(;;)
      {
        rStream.SkipWhitespace();
        if(rStream.Test(">"))
        {
          TChar chTmp = ' ';
          rStream.ReadStringUntil(rXMLNode.NodeContent(), "<");
          if(rXMLNode.NodeContent().AsString().size() > 0)
            chTmp = rXMLNode.NodeContent().AsString()[(int)(rXMLNode.NodeContent().AsString().size() - 1)];
          StrTrim(rXMLNode.NodeContent().AsString());
          rStream.UnGetChar(chTmp);
          ReadSubNodes(rStream, rXMLNode); //read subnodes

          rStream.ReadStringUntil(sTmp, ">");
          ASSERTXMLS(sTmp == rXMLNode.GetNodeNsName(), CXMLFormatException, 
            "Unmatched opening and closing tags: found </"
            + sTmp + "> expected: </" + rXMLNode.GetNodeNsName() + ">", 
            rStream.GetFileName(), rStream.GetLine() );
          break;
        } else
        if(rStream.Test("/>"))  // пустой элемент?
          break;// end reading root node
        
        rStream >> stAttribute;
        if (StrLeft(stAttribute.sAttrName, 6) == "xmlns:")
          rXMLNode.GetNsList().push_back(SXMLNamespace(StrMid(const_cast<const SXMLAttribute&>(stAttribute).sAttrName, 6), 
          const_cast<const SXMLAttribute&>(stAttribute).sAttrValue));
        else
          rXMLNode.AddAttribute(const_cast<const SXMLAttribute&>(stAttribute));
      }

      rStream.SkipWhitespace();
      rStream.ReadStringUntil(sTmp, "");
      ASSERTXMLS(sTmp == "", CXMLFormatException, "text after parsing", 
        rStream.GetFileName(), rStream.GetLine());
      
      return rStream;
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       <<
    //    DESCRIPTION:    оператор сериализации
    //    PARAMETRS:      (in/out) rStream - поток сериализации
    //                    (in) rXMLNode - ссылка на корневой узел для сериализации
    //    RETURN:         результирующий поток сериализации
    //    EXCEPTIONS:     CXMLFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLNode& rXMLNode)
    {
      CXMLOStream tTmpStream(rStream);
      return operator<<(tTmpStream, rXMLNode);
    }

    //////////////////////////////////////////////////////////////////////////////
    //    OPERATOR:       >>
    //    DESCRIPTION:    оператор десериализации
    //    PARAMETRS:      (in/out) rStream - поток десериализации
    //                    (in) rXMLNode - ссылка на корневой узел для десериализации
    //    RETURN:         результирующий поток десериализации
    //    EXCEPTIONS:     CXMLFormatException
    //    COMMENT:        none
    //////////////////////////////////////////////////////////////////////////////
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLNode& rXMLNode)
    {
      CXMLIStream tTmpStream(rStream);
      return operator>>(tTmpStream, rXMLNode);
    }

  } // namespace xml
}  // namespace rise
  
