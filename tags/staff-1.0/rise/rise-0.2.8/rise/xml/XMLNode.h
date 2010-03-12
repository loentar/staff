#ifndef _XMLNODE_H_
#define _XMLNODE_H_

#include <string>
#include <list>
#include <utility>
#include <rise/os/osdllexport.h>
#include <rise/common/ExceptionTemplate.h>
#include <rise/string/String.h>
#include <rise/xml/XMLValue.h>
#include <rise/xml/XMLAttribute.h>
#include <rise/xml/XMLNamespace.h>

namespace rise
{
  namespace xml
  {
    //!        узел XML
    class RISE_EXPORT CXMLNode
    {
    public:
      enum ENodeType //! тип узла
      { 
        ENTGENERIC,  //!< обычный узел
        ENTCDATA,    //!< секция CDATA
        ENTCOMMENT   //!< комментарий
      };

      typedef std::list<CXMLNode> TXMLNodeList;                   //!<  тип - список узлов
      typedef std::list<SXMLAttribute> TXMLAttrList;              //!<  тип - список атрибутов
      typedef std::list<SXMLNamespace> TXMLNsList;                //!<  тип - список пространств имен
      typedef TXMLNodeList::iterator TXMLNodeIterator;            //!<  тип - итератор по подузлам
      typedef TXMLAttrList::iterator TXMLAttrIterator;            //!<  тип - итератор по атрибутам
      typedef TXMLNsList::iterator TXMLNsIterator;                //!<  тип - итератор по пространствам имен
      typedef TXMLNodeList::const_iterator TXMLNodeConstIterator; //!<  тип - константный итератор по подузлам
      typedef TXMLAttrList::const_iterator TXMLAttrConstIterator; //!<  тип - константный итератор по атрибутам
      typedef TXMLNsList::const_iterator TXMLNsConstIterator;     //!<  тип - константный итератор по пространствам имен

    public:
      //!         конструктор по умолчанию
      /*! \param  pParentNode - родительский узел
          \return 
          */
      CXMLNode(CXMLNode* pParentNode = NULL);

      //!         инициализирующий конструктор
      /*! \param  sNodeText - имя создаваемого узла, если eNodeType = ENTGENERIC, иначе контент узла, имя может содержать знак пространства имен
          \param  pParentNode - родительский узел
          \param  eNodeType - eNodeType - тип узла
          \return 
          */
      CXMLNode(const CString& sNodeText, CXMLNode* pParentNode = NULL, ENodeType eNodeType = ENTGENERIC); 

      //!         копирующий конструктор
      /*! \param  rNode - ссылка на существующий узел
          */
      CXMLNode(const CXMLNode& rNode);

      //!         деструктор
      virtual ~CXMLNode();

      //!         оператор копирования
      /*! \param  rNode - исходный узел
          \return копия узла
          */
      CXMLNode& operator=(const CXMLNode& rNode);

      //!         получить родительский узел
      /*! \return указатель на родительский узел или NULL
          */
      CXMLNode* GetParent();

      //!         получить родительский узел
      /*! \return const-указатель на родительский узел или NULL
          */
      const CXMLNode* GetParent() const;

      //!         установить для узла пространство имен и имя
      /*! \param  sNsName - namespace:name
          */
      void SetNodeNsName(const CString& sNsName);

      //!         получить для узла пространство имен и имя
      /*! \return  sNsName - namespace:name
      */
      CString GetNodeNsName() const;

      //!        функция доступа к атрибутам по имени
      /*
         CLogicNoItemException
         \param   sName - имя атрибута
         \return значение атрибута
      */
      const CXMLValue& Attribute(const CString& sName) const;

      //!        функция доступа к атрибутам по имени
      /*
         CLogicNoItemException
         \param   sName - имя атрибута
         \return значение атрибута
      */
      CXMLValue& Attribute(const CString& sName);

      //!        существует ли атрибут с заданным именем
      /*
         \param   sName - имя атрибута
         \param  RETURN:         true - атрибут существует
      */
      bool IsAttributeExists(const CString& sName) const;

      //!        функция доступа к подузлам по имени
      /*
          CLogicNoItemException, если атрибут не найден
         \param   sName - имя подузла
         \return подузел
      */
      const CXMLNode& Subnode(const CString& sName) const;

      //!        функция доступа к подузлам по имени
      /*
          CLogicNoItemException, если атрибут не найден
         \param   sName - имя подузла
         \return подузел
      */
      CXMLNode& Subnode(const CString& sName);

      //!        существует ли подузел с заданным именем
      /*
         \param   sName - имя подузла
         \param  RETURN:         true - подузел с заданным именем существует
      */
      bool IsSubnodeExists(const CString& sName) const;

      //!        получение итератора на первый подузел
      /*
         \return итератор на первый подузел
      */
      TXMLNodeConstIterator NodeBegin() const;

      //!        получение итератора на первый подузел
      /*
         \return итератор на первый подузел
      */
      TXMLNodeIterator NodeBegin();

      //!        получение итератора на подузел за последним подузлом
      /*
         \return итератор на подузел за последним подузлом
      */
      TXMLNodeConstIterator NodeEnd() const;

      //!        получение итератора на подузел за последним подузлом
      /*
         \return итератор на подузел за последним подузлом
      */
      TXMLNodeIterator NodeEnd();

      //!        получение итератора на первый атрибут
      /*
         \return итератор на первый атрибут
      */
      TXMLAttrConstIterator AttrBegin() const;

      //!        получение итератора на первый атрибут
      /*
         \return итератор на первый атрибут
      */
      TXMLAttrIterator AttrBegin();

      //!        получение итератора на атрибут за последним атрибутом
      /*
         \return итератор на атрибут за последним атрибутом
      */
      TXMLAttrConstIterator AttrEnd() const;

      //!        получение итератора на атрибут за последним атрибутом
      /*
         \return итератор на атрибут за последним атрибутом
      */
      TXMLAttrIterator AttrEnd();

      //!        получение имени узла
      /*
         \return ссылка на имя узла
      */
      const CString& NodeName() const;

      //!        получение/установка имени узла
      /*
         \return ссылка на имя узла
      */
      CString& NodeName();

      //!        получение контента узла
      /*
         \return ссылка на контент текущего узла
      */
      const CXMLValue& NodeContent() const;

      //!        получение/установка контента узла
      /*
         \return ссылка на контент текущего узла
      */
      CXMLValue& NodeContent();

      //!        доступ к значению подузла
      /*
          rise::CLogicNoItemException
          не-const функция добавляет подузел, если такового не существует
         \param  sName - имя подузла
         \return ссылка на контент подузла
      */
      const CXMLValue& operator[](const CString& sName) const;

      //!        доступ к значению подузла
      /*
          rise::CLogicNoItemException
          не-const функция добавляет подузел, если такового не существует
         \param  sName - имя подузла
         \return ссылка на контент подузла
      */
      CXMLValue& operator[](const CString& sName);

      //!        добавление подузла с заданным именем, если узла с таким именем еще не существует
      /*
         \param   sNodeName - имя создаваемого узла, 
         \return ссылка на добавленный или найденный подузел
      */
      CXMLNode& GetOrAddSubNode(const CString& sNodeName);

      //!        получение/установка типа узла
      /*
         \return ссылка на тип узла
      */
      const ENodeType& NodeType() const;

      //!        получение/установка типа узла
      /*
         \return ссылка на тип узла
      */
      ENodeType& NodeType();

      //!        получение количества подузлов
      /*
         \return количество подузлов
      */
      int SubNodesCount() const;

      //!        добавление атрибута
      /*
          CLogicAlreadyExistsException
         \param   sAttrName - имя атрибута
         \param   sAttrValue - значение атрибута
         \return ссылка на атрибут
      */
      SXMLAttribute& AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue);

      //!        добавление атрибута
      /*
          CLogicAlreadyExistsException
         \param   sAttrName - имя атрибута
         \param   sAttrValue - значение атрибута
         \return ссылка на атрибут
      */
      SXMLAttribute& AddAttribute(const SXMLAttribute& stAttribute);

      //!        добавление атрибута
      /*
         CLogicAlreadyExistsException
         \param   stAttribute - атрибут
         \param   itAttr - позиция куда будет вставлен указанный атрибут
         \return итератор
      */
      TXMLAttrIterator AddAttribute(const CString& sAttrName, const CXMLValue& sAttrValue, TXMLAttrIterator itAttr);

      //!        добавление атрибута
      /*
         CLogicAlreadyExistsException
         \param   stAttribute - атрибут
         \param   itAttr - позиция куда будет вставлен указанный атрибут
         \return итератор
      */
      TXMLAttrIterator AddAttribute(const SXMLAttribute& stAttribute, TXMLAttrIterator itAttr);

      //!        удаление атрибута
      /*
         \param   itAttr - позиция удаляемого атрибута
         \return none
      */
      //    EXCEPTIONS:     CLogicNoItemException
      void DelAttribute(TXMLAttrIterator itAttr);

      //!        добавление подузла
      /*
         \param   sNodeText - имя создаваемого узла, если eNodeType = ENTGENERIC, иначе контент узла
         \param   eNodeType - тип узла
         \return ссылка на добавленный подузел
      */
      CXMLNode& AddSubNode(const CString& sNodeText, ENodeType eNodeType = ENTGENERIC);

      //!        добавление подузла
      /*
         \param   sNodeText - имя создаваемого узла, если eNodeType = ENTGENERIC, иначе контент узла
         \param   eNodeType - тип узла
         \return ссылка на добавленный подузел
      */
      CXMLNode& AddSubNode(const CString& sNodeText, TXMLNodeIterator itNode, ENodeType eNodeType = ENTGENERIC);

      //!        удаление подузла
      /*
         \param   itAttr - позиция удаляемого атрибута
         \return none
      */
      //    EXCEPTIONS:     CLogicNoItemException
      void DelSubNode(TXMLNodeIterator itNode);

      //!        удаление всех подузлов и атрибутов
      void Clear();

      //!        функция поиска атрибута по имени
      /*
         \param   sName - имя атрибута
         \return итератор на найденный атрибут с заданным именем. если атрибут не найден, то итератор == AttrEnd()
      */
      TXMLAttrConstIterator FindAttribute(const CString& sName) const;

      //!        функция поиска атрибута по имени
      /*
         \param   sName - имя атрибута
         \return итератор на найденный атрибут с заданным именем. если атрибут не найден, то итератор == AttrEnd()
      */
      TXMLAttrIterator FindAttribute(const CString& sName);

      //!        поиск артибутов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \return none
      */
      template<typename UNARY_PREDICATE>
      TXMLAttrConstIterator FindAttributeIf(UNARY_PREDICATE pFindFunction) const;

      //!        поиск артибутов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \return none
      */
      template<typename UNARY_PREDICATE>
      TXMLAttrIterator FindAttributeIf(UNARY_PREDICATE pFindFunction);

      //!        функция поиска подузлов по имени
      /*
                если атрибут не найден, то итератор == AttrEnd()
         \param   sName - имя подузла
         \return итератор на первый назденный подузел с заданным именем
      */
      TXMLNodeConstIterator FindSubnode(const CString& sName) const;

      //!        функция поиска подузлов по имени
      /*
                 если атрибут не найден, то итератор == AttrEnd()
         \param   sName - имя подузла
         \return итератор на первый назденный подузел с заданным именем
      */
      TXMLNodeIterator FindSubnode(const CString& sName);

      //!        функция поиска подузлов по имени
      /*
                 если атрибут не найден, то итератор == AttrEnd()
         \param   sName - имя подузла
         \param   itNode - итератор, с которого следует произвести поиск
         \return итератор на первый назденный подузел с заданным именем
      */
      TXMLNodeConstIterator FindSubnode(const CString& sName, TXMLNodeConstIterator itNode) const;

      //!        функция поиска подузлов по имени
      /*
                 если атрибут не найден, то итератор == AttrEnd()
         \param   sName - имя подузла
         \param   itNode - итератор, с которого следует произвести поиск
         \return итератор на первый назденный подузел с заданным именем
      */
      TXMLNodeIterator FindSubnode(const CString& sName, TXMLNodeIterator itNode);

      //!        поиск подузла с заданным значением атрибута
      /*
         \param   sNodeName - имя подузла
         \param   stAttribute - атрибут
         \return итератор на найденный подузел
      */
      TXMLNodeConstIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute) const;

      //!        поиск подузла с заданным значением атрибута
      /*
         \param   sNodeName - имя подузла
         \param   stAttribute - атрибут
         \return итератор на найденный подузел
      */
      TXMLNodeIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute);

      //!        поиск подузла с заданным значением атрибута
      /*
         \param   sNodeName - имя подузла
         \param   stAttribute - атрибут
         \param   itNode - итератор на дочерний подузел, с которого следует продолжить поиск
         \return итератор на найденный подузел
      */
      TXMLNodeConstIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute,
        TXMLNodeConstIterator itNode) const;

      //!        поиск подузла с заданным значением атрибута
      /*
         \param   sNodeName - имя подузла
         \param   stAttribute - атрибут
         \param   itNode - итератор на дочерний подузел, с которого следует продолжить поиск
         \return итератор на найденный подузел
      */
      TXMLNodeIterator FindNodeMatch(
        const CString& sNodeName,
        const SXMLAttribute& stAttribute,
        TXMLNodeIterator itNode);

      //!        поиск подузлов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \param  itNode - итератор на узел, начиная с которого будет производиться поиск
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeConstIterator FindNodeIf(UNARY_PREDICATE pFindFunction) const;

      //!        поиск подузлов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \param  itNode - итератор на узел, начиная с которого будет производиться поиск
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeIterator FindNodeIf(UNARY_PREDICATE pFindFunction);

      //!        поиск подузлов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \param  itNode - итератор на узел, начиная с которого будет производиться поиск
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeConstIterator FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeConstIterator itNode) const;

      //!        поиск подузлов по предикату
      /*
         \param  pFindFunction - унарный предикат поиска
         \param  itNode - итератор на узел, начиная с которого будет производиться поиск
      */
      template<typename UNARY_PREDICATE>
      TXMLNodeIterator FindNodeIf(UNARY_PREDICATE pFindFunction, TXMLNodeIterator itNode);

      //!        сортировка атрибутов по заданному условию
      /*
         \param  pCompareFunction - бинарный предикат сортировки
         \return none
      */
      template<typename BINARY_PREDICATE>
      void SortAttrs(BINARY_PREDICATE pCompareFunction);

      //!        сортировка подузлов по заданному условию
      /*
         \param  pCompareFunction - бинарный предикат сортировки
         \return none
      */
      template<typename BINARY_PREDICATE>
      void SortNodes(BINARY_PREDICATE pCompareFunction);

      //!         получить пространство имен узла
      /*! \return const пространство имен узла
          */
      const CString& Namespace() const;

      //!         получить пространство имен узла
      /*! \return пространство имен узла
          */
      CString& Namespace();

      //!         получить список пространств имен
      /*! \return список пространств имен
          */
      const TXMLNsList& GetNsList() const;

      //!         получить список пространств имен
      /*! \return список пространств имен
          */
      TXMLNsList& GetNsList();

      //!         найти узел с описанием пространства имен среди родительских узлов
      /*! \param  sNamespace - пространство имен
          \return узел узел с описанием пространства имен или NULL, если узел не найден
          */
      CXMLNode* FindNamespaceDecl(const CString& sNamespace);

      //!         найти узел с описанием пространства имен среди родительских узлов
      /*! \param  sNamespace - пространство имен
          \return const узел узел с описанием пространства имен или NULL, если узел не найден
          */
      const CXMLNode* FindNamespaceDecl(const CString& sNamespace) const;

    private:
      CXMLNode*        m_pNodeParent;     //!<  родительский узел
      TXMLAttrList     m_tAttr;           //!<  список атрибутов
      TXMLNodeList     m_tSubNodes;       //!<  список подузлов
      CString          m_sName;           //!<  имя узла
      CXMLValue        m_sContent;        //!<  контент узла
      ENodeType        m_eNodeType;       //!<  тип узла
      CString          m_sNamespace;      //!<  пространство имен узла
      TXMLNsList       m_tNsList;         //!<  список пространств имен определяемых в текущем узле

    }; // class CXMLNode //////////////////////////////////////////////////

    //!        оператор сериализации
    /*
       CXMLFormatException
       \param   rStream - поток сериализации
       \param   rXMLNode - ссылка на корневой узел для сериализации
       \return результирующий поток сериализации
    */
    RISE_EXPORT CXMLOStream& operator<<(CXMLOStream& rStream, const CXMLNode& rXMLNode);

    //!        оператор десериализации
    /*
      CXMLFormatException
       \param   rStream - поток десериализации
       \param   rXMLNode - ссылка на корневой узел для десериализации
       \return результирующий поток десериализации
    */
    RISE_EXPORT CXMLIStream& operator>>(CXMLIStream& rStream, CXMLNode& rXMLNode);

    //!        оператор сериализации
    /*
      CXMLFormatException
       \param   rStream - поток сериализации
       \param   rXMLNode - ссылка на корневой узел для сериализации
       \return результирующий поток сериализации
    */
    RISE_EXPORT COStream& operator<<(COStream& rStream, const CXMLNode& rXMLNode);

    //!        оператор десериализации
    /*
      CXMLFormatException
       \param   rStream - поток десериализации
       \param   rXMLNode - ссылка на корневой узел для десериализации
       \return результирующий поток десериализации
    */
    RISE_EXPORT CIStream& operator>>(CIStream& rStream, CXMLNode& rXMLNode);

  } // namespace xml
} // namespace rise

#include "XMLNode.hpp"

#endif // #ifndef _XMLNODE_H_
