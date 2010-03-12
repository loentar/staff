#ifndef _DATAOBJECT_H_
#define _DATAOBJECT_H_

#include <string>
#include "staffcommonexport.h"

// AXIS typedefs
typedef struct axiom_node axiom_node_t;
typedef struct axiom_element axiom_element_t;
typedef struct axiom_attribute axiom_attribute_t;
typedef struct axutil_hash_index_t axutil_hash_index_t;
typedef struct axutil_env axutil_env_t;

//! namespace
namespace staff
{
  class CQName;
  class CAttribute;
  class CValue;

  //!  Объект данных
  class STAFF_COMMON_EXPORT CDataObject
  {
  public:
    class STAFF_COMMON_EXPORT Iterator;
    class STAFF_COMMON_EXPORT ConstIterator;
    class STAFF_COMMON_EXPORT AttributeIterator;
    class STAFF_COMMON_EXPORT ConstAttributeIterator;

  public:
    //!        конструктор по умолчанию
    /*! конструктор по умолчанию не создает элементов! */
    CDataObject(axiom_node_t* pAxiomNode = NULL);

    //!        конструктор с передачей владения
    /*! \sa Clone */
    CDataObject(const CDataObject& rDataObject);

    //!         конструктор узла с инициализацией
    /*! \param  sLocalName - имя создаваемого узла
    */
    CDataObject(const std::string& sLocalName);

    //!         конструктор узла с инициализацией
    /*! \param  sLocalName - имя создаваемого узла
        \param  sText - текст узла
    */
    explicit CDataObject(const std::string& sLocalName, const std::string& sText);

    //!         конструктор узла с инициализацией
    /*! \param  sLocalName - имя создаваемого узла
        \param  rValue - значение узла
    */
    CDataObject(const std::string& sLocalName, const CValue& rValue);

    //!         конструктор узла с инициализацией
    /*! \param  stQName
    */
    CDataObject(const CQName& stQName);

    //!        деструктор
    ~CDataObject();

    //!         присоединить узел Axiom(к обьекту данных)
    /*! для присоединения узла к дереву служит функция AttachNode 
        \sa DetachNode 
        \param  pAxiomNode - узел Axiom
        \param  bOwner - true - установить обьект данных владельцем узла. 
         В этом случае при освобождении обьекта данных связанное с ним дерево будет освобождено 
        */
    void Attach(axiom_node_t* pAxiomNode, bool bOwner = false);

    //!         отсоединить узел Axiom(от обьекта данных)
    /*! если обьект данных является владельцем дерева, произойдет освобождение дерева
        если у дерева был родитель, узел отсоединяется
        для отсоединения узла от дерева служит функция DetachNode 
        \sa DetachNode */
    void Detach();

    //!         является ли обьект данных владельцем дерева
    /*! \return true - обьект данных является владельцем дерева
        */
    bool IsOwner();

    //!         установить обьект данных владельцем узла. 
    /*! \param  bOwner - true/false - установить/сбросить обьект данных владельцем узла
        */
    void SetOwner(bool bOwner);

    //!         оператор получения узла Axiom
    /*! \return узел Axiom
    */
    operator axiom_node_t*();

    //!         проверка на инициализаицю
    /*! \return true - объект проинициализирован
    */
    bool IsInit() const;

    //////////////////////////////////////////////////////////////////////////
    // свойства узла

    //!         получить составное имя
    /*! \return составное имя
    */
    CQName GetQName();
    
    //!         установить составное имя
    /*! \param  stQName - составное имя
        */
    void SetQName(const CQName& stQName) const;

    //!         получить локальное имя
    /*! \return локальное имя
    */
    std::string GetLocalName() const;
    
    //!         установить локальное имя
    /*! \param  sLocalName - локальное имя
        */
    void SetLocalName(const std::string& sLocalName) const;

    //!         получить префикс
    /*! \return префикс
    */
    std::string GetPrefix() const;
    
    //!         установить префикс
    /*! \param  sPrefix - префикс
        */
    void SetPrefix(const std::string& sPrefix) const;

    //!         получить URI пространства имен
    /*! \return URI пространства имен
    */
    std::string GetNamespaceUri() const;
    
    //!         установить URI пространства имен
    /*! \param  sUri - URI пространства имен
        */
    void SetNamespaceUri(const std::string& sUri) const;

    //////////////////////////////////////////////////////////////////////////
    // управление узлом

    //!         создать узел и установить обьект данных владельцем узла
    void Create();

    //!         создать узел и установить обьект данных владельцем узла
    /*! \param  sLocalName - локальное имя узла
        */
    void Create(const std::string& sLocalName);

    //!         создать узел и установить обьект данных владельцем узла
    /*! \param  rstQName - составное имя узла
        */
    void Create(const CQName& rstQName);

    //!         создать узел и установить обьект данных владельцем узла
    /*! \param  sLocalName - имя создаваемого узла
        \param  sText - текст узла
    */
    void Create(const std::string& sLocalName, const std::string& sText);

    //!         создать узел и установить обьект данных владельцем узла
    /*! \param  sLocalName - имя создаваемого узла
        \param  rValue - значение узла
    */
    void Create(const std::string& sLocalName, const CValue& rValue);

    //!         удалить текущий узел и всё поддерево
    /*!         узел удаляется в независимости от флага владения */
    void Free();

    //!         отсоединить текущий узел от существующего поддерева
    /*! \return ссылка на текущий обьект
    */
    CDataObject& DetachNode();

    //!         создание копии дерева
    /*! текущий объект становится владельцем копии
        !! ВНИМАНИЕ !! неоптимальная версия: копия создаётся посредством сериализации через строку
        \param  rDataObject - исходный обьект
        \return ссылка на текущий обьект
        */
    CDataObject& Clone(const CDataObject& rDataObject);

    //!         создание копии дерева
    /*! возвращаемый объект становится владельцем копии
        !! ВНИМАНИЕ !! неоптимальная версия: копия создаётся посредством сериализации через строку
        \return объект копии
        */
    CDataObject Clone() const;

    //////////////////////////////////////////////////////////////////////////
    // управление подузлами
    
    //!         создать дочерний узел
    CDataObject CreateChild();

    //!         создать дочерний узел
    /*! \param  rstQName - составное имя узла
        */
    CDataObject CreateChild(const CQName& rstQName);

    //!         создать дочерний узел
    /*! \param  sLocalName - имя создаваемого узла
    */
    CDataObject CreateChild(const std::string& sLocalName);

    //!         создать дочерний узел
    /*! \param  sLocalName - имя создаваемого узла
        \param  rValue - значение узла
    */
    CDataObject CreateChild(const std::string& sLocalName, const CValue& rValue);

    //!         получить дочерний узел с указанным локальным именем, создать, если такой узел не существует
    /*! \param  sLocalName - локальное имя дочернего узла
        \return дочерний узел
        */
    CDataObject CreateChildOnce(const std::string& sLocalName);

    //!         добавить дочернее поддерево
    /*! сбрасывает флаг владельца у rDataObject
        \param  rDataObject - дочернее поддерево
        */
    CDataObject AppendChild(CDataObject& rDataObject);
    
    //!         добавить дочернее поддерево
    /*! \param  rDataObject - дочернее поддерево
        */
    CDataObject AppendChild(const CDataObject& rDataObject);
    
    //!         отсоединить дочернее поддерево
    /*! \param  itChild - итератор на дочерний узел для отсоединения
        */
    CDataObject DetachChild(Iterator& itChild);

    //!         отсоединить и удалить дочернее поддерево
    /*! \param  itChild - итератор на дочерний узел
        */
    void RemoveChild(Iterator& itChild);

    //!         отсоединить и удалить дочернее поддерево
    /*! \param  sName - имя поддерева
        */
    void RemoveChildByLocalName(const std::string& sName);

    //!         удалить все дочерние узлы
    void RemoveAllChildren();

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return итератор на дочерний узел
        */
    Iterator FindChildByQName(const CQName& stQName);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return итератор на дочерний узел
        */
    ConstIterator FindChildByQName(const CQName& stQName) const;

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на дочерний узел
        */
    Iterator FindChildByQName(const CQName& stQName, const Iterator& itStart);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на дочерний узел
        */
    ConstIterator FindChildByQName(const CQName& stQName, const ConstIterator& itStart) const;

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return итератор на дочерний узел
        */
    Iterator FindChildByLocalName(const std::string& sLocalName);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return итератор на дочерний узел
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName) const;

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на дочерний узел
        */
    Iterator FindChildByLocalName(const std::string& sLocalName, const Iterator& itStart);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на дочерний узел
        */
    ConstIterator FindChildByLocalName(const std::string& sLocalName, const ConstIterator& itStart) const;

    //!         получить дочерний узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    CDataObject GetChildByQName(const CQName& stQName);

    //!         получить дочерний узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    const CDataObject GetChildByQName(const CQName& stQName) const;

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    CDataObject GetChildByLocalName(const std::string& sLocalName);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    const CDataObject GetChildByLocalName(const std::string& sLocalName) const;

    //////////////////////////////////////////////////////////////////////////
    // управление данными

    //!         получение/установка значения объекта
    /*! пример: 
        int nValue = tDataObject.Value(); // чтение
        tDataObject.Value() = 2.3; // запись

        \return значение объекта
    */
    CValue Value();

    //!         получение копии значения объекта
    /*! \return копия значения объекта
        */
    CValue GetValue() const;

    //!         установка значения объекта
    /*! \param  rValue - значение объекта
    */
    void SetValue(const CValue& rValue);

    //!         получить текст объекта
    /*! \return текст объекта
    */
    std::string GetText() const;
    
    //!         установить текст
    /*! \param  sText - текст
        */
    void SetText(const std::string& sText);

    //!         получение/установка значения дочернего объекта
    /*! пример: 
        int nValue = tDataObject["Param1"]; // чтение
        tDataObject["Param1"] = 2.3; // запись

        \param  sName - имя дочернего узла
        \return значение дочернего узла
    */
    CValue operator[](const std::string& sName);

    //!         получение значения дочернего объекта
    /*! пример: 
        int nValue = tDataObject["Param1"]; // чтение
    
        \param  sName - имя дочернего узла
        \return значение дочернего узла
    */
    const CValue operator[](const std::string& sName) const;

    //////////////////////////////////////////////////////////////////////////
    // управление атрибутами

    //!         добавить атрибут
    /*! сбрасывает флаг владельца у rAttribute
        \param  rAttribute - атрибут
        */
    void AppendAttribute(CAttribute& rAttribute);
    
    //!         отсоединить и удалить дочернее поддерево
    /*! \param  itAttribute - итератор на дочерний узел
        */
    void RemoveAttribute(AttributeIterator& itAttribute);

    //!         удалить атрибуты
    void RemoveAllAttributes();

    //!         поиск атрибута по составному имени
    /*! \param  stQName - составное имя
        \return итератор на атрибут
        */
    AttributeIterator FindAttributeByQName(const CQName& stQName);

    //!         поиск атрибута по составному имени
    /*! \param  stQName - составное имя атрибута
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на атрибут
        */
    AttributeIterator FindAttributeByQName(const CQName& stQName, const AttributeIterator& itStart);

    //!         поиск дочернего узла по локальному имени
    /*! \param  stQName - составное имя узла
        \return итератор на дочерний узел
        */
    AttributeIterator FindAttributeByLocalName(const std::string& sLocalName);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \param  itStart - позиция, с которой будет производиться поиск
        \return итератор на дочерний узел
        */
    AttributeIterator FindAttributeByLocalName(const std::string& sLocalName, const AttributeIterator& itStart);

    //!         получить дочерний узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    CAttribute GetAttributeByQName(const CQName& stQName);

    //!         поиск дочернего узла по составному имени
    /*! \param  stQName - составное имя узла
        \return дочерний узел
        */
    CAttribute GetAttributeByLocalName(const std::string& sLocalName);


    //////////////////////////////////////////////////////////////////////////
    // итерация

    //!         получить итератор на первый элемент
    /*! \return итератор на первый элемент
    */
    Iterator Begin();

    //!         получить итератор на первый элемент
    /*! \return итератор на первый элемент
    */
    ConstIterator Begin() const;

    //!         получить итератор на следующий за последним элемент
    /*! \return итератор на следующий за последним элемент
    */
    Iterator End();

    //!         получить итератор на следующий за последним элемент
    /*! \return итератор на следующий за последним элемент
    */
    ConstIterator End() const;

    //!         получить итератор на первый атрибут
    /*! \return итератор на первый атрибут
    */
    AttributeIterator AttributeBegin();

    //!         получить итератор на следующий за последним атрибут
    /*! \return итератор на следующий за последним атрибут
    */
    AttributeIterator AttributeEnd();

    //////////////////////////////////////////////////////////////////////////
    // операторы поддержки

    //!         оператор передачи владения деревом
    /*! для копирования дерева используйте Clone
        \param  rDataObject - исходный обьект данных
        \return текущий обьект данных
        \sa Clone
        */
    CDataObject& operator=(const CDataObject& rDataObject);
    
    //!         указывают ли оба объекта данных на одно дерево
    /*! \param  rDataObject - второй объект данных
        \return true - оба объекта данных указывают на одно дерево
        */
    bool operator==(const CDataObject& rDataObject) const;
    
    //!         не указывают ли оба объекта данных на одно дерево
    /*! \param  rDataObject - второй объект данных
        \return true - оба объекта данных не указывают на одно дерево
        */
    bool operator!=(const CDataObject& rDataObject) const;

    //!         преобразовать текущее дерево в строку xml
    /*! \return строка xml
    */
    std::string ToString() const;

    //!         создать дерево из строки xml
    /*! \param  sXml - строка xml
        \return ссылка на текущий обьект данных
        */
    CDataObject& FromString(const std::string& sXml);

    //!         оператор разыменования
    /*! \return this
    */
    CDataObject* operator->();

    //!         оператор разыменования
    /*! \return this
    */
    const CDataObject* operator->() const;

#ifndef STAFF_NO_DEPRECATED
    //////////////////////////////////////////////////////////////////////////
    // устаревшие функции

    //!         устаревшая функция 
    /*! \sa CreateChildOnce
        */
    STAFF_DEPRECATED(CreateChildOnce) CDataObject GetOrAdd(const std::string& sName);

    //!         устаревшая функция 
    /*! \sa FindChildByLocalName
        */
    STAFF_DEPRECATED(FindChildByLocalName) Iterator Find(const std::string& sLocalName);

    //!         устаревшая функция 
    /*! \sa FindChildByLocalName
        */
    STAFF_DEPRECATED(FindChildByLocalName) ConstIterator Find(const std::string& sLocalName) const;

    //!         устаревшая функция 
    /*! \sa GetChildByLocalName
        */
    STAFF_DEPRECATED(GetChildByLocalName) const CDataObject operator()(const std::string& sLocalName) const;

    //!         устаревшая функция 
    /*! \sa GetChildByLocalName
        */
    STAFF_DEPRECATED(GetChildByLocalName) CDataObject operator()(const std::string& sLocalName);

    //!         устаревшая функция 
    /*! \sa GetNamespaceUri
        */
    STAFF_DEPRECATED(GetNamespaceUri) std::string GetNamespace() const;

    //!         устаревшая функция 
    /*! \sa SetNamespaceUri
        */
    STAFF_DEPRECATED(SetNamespaceUri) void SetNamespace(const std::string& sNamespace);

    //!         устаревшая функция 
    /*! \sa GetLocalName*/
    STAFF_DEPRECATED(GetLocalName) const std::string Name() const;

    //!         устаревшая функция 
    /*! \sa SetLocalName */
    STAFF_DEPRECATED(SetLocalName)void SetName(const std::string& sName);

    //!         устаревшая функция 
    /*! \sa CreateChild
        */
    STAFF_DEPRECATED(CreateChild) CDataObject Add(const std::string& sName);

    //!         устаревшая функция 
    /*! \sa CreateChild
        */
    STAFF_DEPRECATED(CreateChild) CDataObject Add(const std::string& sName, const CValue& rValue);

    //!         устаревшая функция 
    /*! \sa AppendChild
        */
    STAFF_DEPRECATED(AppendChild) CDataObject Add(CDataObject rDataObject);

    //!         устаревшая функция 
    /*! \sa GetValue
    */
    STAFF_DEPRECATED(GetValue) const CValue Value() const;

#endif

  private:
    axiom_node_t* m_pAxiomNode;
    axiom_element_t* m_pAxiomElement;
    static axutil_env_t* m_pEnv;
    bool m_bOwner;
    friend class Iterator;
    friend class ConstIterator;
    friend class AttributeIterator;
    friend class ConstAttributeIterator;
  };

  //! итератор узлов
  class CDataObject::Iterator
  {
  public:
    //!         конструктор
    Iterator();
    
    //!         инициализирующий конструктор 
    /*! \param  pDataObject - объект данных итератора
        \param  pAxiomNode - узел итератора
        */
    Iterator(CDataObject* pDataObject, axiom_node_t* pAxiomNode);
    
    //!         копирующий конструктор
    /*! \param  rIter - итератор
        */
    Iterator(const Iterator& rIter);
    
    //!         деструктор
    ~Iterator();

    Iterator& operator=(const Iterator& rIter);
    
    Iterator& operator--();
    Iterator& operator++();
    Iterator operator--(int);
    Iterator operator++(int);
    bool operator==(const Iterator& rIter) const;
    bool operator!=(const Iterator& rIter) const;

    CDataObject operator*();
    const CDataObject operator*() const;

    CDataObject operator->();
    const CDataObject operator->() const;

  private:
    CDataObject* m_pDataObject;
    axiom_node_t* m_pAxiomNode;
    friend class CDataObject;
  };

  //! константный итератор узлов
  class CDataObject::ConstIterator
  {
  public:
    //!         конструктор
    ConstIterator();
    
    //!         инициализирующий конструктор 
    /*! \param  pDataObject - объект данных итератора
        \param  pAxiomNode - узел итератора
        */
    ConstIterator(const CDataObject* pDataObject, axiom_node_t* pAxiomNode);
    
    //!         копирующий конструктор
    /*! \param  rIter - итератор
        */
    ConstIterator(const ConstIterator& rIter);
    
    //!         деструктор
    ~ConstIterator();

    ConstIterator& operator=(const ConstIterator& rIter);
    
    ConstIterator& operator--();
    ConstIterator& operator++();
    ConstIterator operator--(int);
    ConstIterator operator++(int);
    bool operator==(const ConstIterator& rIter) const;
    bool operator!=(const ConstIterator& rIter) const;

    const CDataObject operator*() const;

    const CDataObject operator->() const;

  private:
    const CDataObject* m_pDataObject;
    axiom_node_t* m_pAxiomNode;
    friend class CDataObject;
  };

  //! итератор атрибутов
  class CDataObject::AttributeIterator
  {
  public:
    //!         конструктор
    AttributeIterator();

    //!         инициализирующий конструктор 
    /*! \param  pDataObject - объект данных итератора
        \param  pAttributeHash - список атрибутов
        */
    AttributeIterator(CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

    //!         копирующий конструктор
    /*! \param  rIter - итератор
    */
    AttributeIterator(const AttributeIterator& rIter);

    //!         деструктор
    ~AttributeIterator();

    //!         оператор копирования
    /*! \param  rIter - исходный итератор
        \return ссылка на текущий итератор
        */
    AttributeIterator& operator=(const AttributeIterator& rIter);

    //!         оператор преинкремента
    /*! \return ссылка на текущий итератор
        */
    AttributeIterator& operator++();

    //!         оператор постинкремента
    /*! \return новый итератор
        */
    AttributeIterator operator++(int);

    //!         оператор проверки на эквивалентность
    /*! \param  rIter - итератор
        \return ссылка на текущий итератор
        */
    bool operator==(const AttributeIterator& rIter) const;

    //!         оператор проверки на неэквивалентность
    /*! \param  rIter - итератор
        \return ссылка на текущий итератор
        */
    bool operator!=(const AttributeIterator& rIter) const;

    //!         оператор разыменовывания
    /*! \return атрибут
    */
    CAttribute operator*();

    //!         оператор разыменовывания
    /*! \return атрибут
    */
    CAttribute operator->();

  private:
    CDataObject* m_pDataObject;
    axutil_hash_index_t* m_pAttributeIndex;
    friend class CDataObject;
  };

  //! константный итератор атрибутов
  class CDataObject::ConstAttributeIterator
  {
  public:
    //!         конструктор
    ConstAttributeIterator();

    //!         инициализирующий конструктор 
    /*! \param  pDataObject - объект данных итератора
        \param  pAttributeHash - список атрибутов
        */
    ConstAttributeIterator(const CDataObject* pDataObject, axutil_hash_index_t* pAttributeIndex);

    //!         копирующий конструктор
    /*! \param  rIter - итератор
    */
    ConstAttributeIterator(const ConstAttributeIterator& rIter);

    //!         деструктор
    ~ConstAttributeIterator();

    //!         оператор копирования
    /*! \param  rIter - исходный итератор
        \return ссылка на текущий итератор
        */
    ConstAttributeIterator& operator=(const ConstAttributeIterator& rIter);

    //!         оператор преинкремента
    /*! \return ссылка на текущий итератор
        */
    ConstAttributeIterator& operator++();

    //!         оператор постинкремента
    /*! \return новый итератор
        */
    ConstAttributeIterator operator++(int);

    //!         оператор проверки на эквивалентность
    /*! \param  rIter - итератор
        \return ссылка на текущий итератор
        */
    bool operator==(const ConstAttributeIterator& rIter) const;

    //!         оператор проверки на неэквивалентность
    /*! \param  rIter - итератор
        \return ссылка на текущий итератор
        */
    bool operator!=(const ConstAttributeIterator& rIter) const;

    //!         оператор разыменовывания
    /*! \return атрибут
    */
    const CAttribute operator*() const;

    //!         оператор разыменовывания
    /*! \return атрибут
    */
    const CAttribute operator->() const;

  private:
    const CDataObject* m_pDataObject;
    axutil_hash_index_t* m_pAttributeIndex;
    friend class CDataObject;
  };

  //!         сериализатор значения в объект данных
  /*! \param  rDataObject - объект данных
      \param  rValue - значение
      \return объект данных
      */
  STAFF_COMMON_EXPORT CDataObject& operator<<(CDataObject& rDataObject, const CValue& rValue);

  //!         десериализатор значения в объект данных
  /*! \param  rDataObject - объект данных
      \param  rValue - значение
      \return объект данных
      */
  STAFF_COMMON_EXPORT const CDataObject& operator>>(const CDataObject& rDataObject, CValue& rValue);
}

#endif // _DATAOBJECT_H_
