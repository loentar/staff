#ifndef _SHAREDPTR_H_
#define _SHAREDPTR_H_

#include <rise/threading/AtomicCount.h>

namespace rise
{
  //!  "Умный" потоковобезопасный указатель
  template<class Type>
  class CSharedPtr
  {
  public:
    //! конструктор
    /*     std::bad_alloc */
    CSharedPtr();

    //! инициализирующий конструктор
    /*
      std::bad_alloc
      неявные преобразования разрешены
       \param  pContainer - указатель на обьект
    */
    CSharedPtr(Type* pObject);

    //! конструктор передачи
    CSharedPtr(const CSharedPtr<Type>& rSharedPtr);

    //! деструктор
    ~CSharedPtr();

    //!        изменение указателя на обьект
    /*
       \return указатель на обьект
    */
    CSharedPtr<Type>& operator=(CSharedPtr<Type>& rOtherPtr) throw();

    //!        изменение указателя на обьект
    /*
       \return указатель на обьект
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(CSharedPtr<OtherType>& rOtherPtr) throw();

    //!        изменение указателя на обьект
    /*
       \return указатель на обьект
    */
    CSharedPtr<Type>& operator=(Type* pObject) throw();

    //!        изменение указателя на обьект
    /*
       \return указатель на обьект
    */
    template<class OtherType>
    CSharedPtr<Type>& operator=(OtherType* pObject) throw();

    //!        получение ссылки на обьект
    /*
       исключения:
       CLogicNoInitException - обьект не проинициализирован
       \return ссылка на обьект
    */
    Type& operator*();

    //!        получение ссылки на обьект
    /*
       исключения:
       CLogicNoInitException - обьект не проинициализирован
       \return ссылка на обьект
    */
    const Type& operator*() const;
    
    //!        оператор разыменовывания
    /*
       \return указатель на обьект
    */
    Type* operator->() throw();

    //!        оператор разыменовывания
    /*
       \return указатель на обьект
    */
    const Type* operator->() const throw();

    //!        получение ссылки на контролируемый обьект
    /*
       \return ссылка на контролируемый объект
    */
    operator Type&();

    //!        получение ссылки на контролируемый обьект
    /*
       \return ссылка на контролируемый объект
    */
    operator const Type&() const;

    //!        получение указателя на контролируемый обьект
    /*
       \return указатель на контролируемый объект
    */
    operator Type*();

    //!        получение указателя на контролируемый обьект
    /*
       \return указатель на контролируемый объект
    */
    operator const Type*() const;

    //!        получение указателя на объект
    /*
       \return указатель на контролируемый объект
    */
    Type* GetPointer() throw();

    //!        получение указателя на объект
    /*
       \return указатель на контролируемый объект
    */
    const Type* GetPointer() const throw();
    
    //!        получение количества ссылок
    /*
       \return количество ссылок на объект
    */
    long GetRefCount() const throw();
    
    //!        уникален ли объект
    /*
       \return true, если объект уникален(клоичество ссылок = 1)
    */
    bool IsUnique() const;
    
    //!        проверка на отсутствие инициализации
    /*
       \return true, если обьект не инициализирован
    */
    bool IsNull() const;
    
    //!        обмен указателей на объект
    /*
       \param  rOther - ссылка на другой объект
    */
    void Swap(CSharedPtr<Type>& rOther);

  private:
    Type* m_pObject;         //! указатель на обьект
    threading::CAtomicCount* m_pCount;  //! указатель на счетчик ссылок
    
  };

  //!        обмен значениями двух типов
  /*
     \param  rLeft, rRight - ссылки на обьекты типов для обмена
  */
  template<class Type>
  void swap(Type& rLeft, Type& rRight) throw();

  //!        один обьект пренадлежит двум контейнерам?
  /*
     \param  ссылки на объекты
     \return true, если если оба контейнера содержат один обьект
  */
  template<class Type>
  bool operator==(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
  //!        разные ли обьект пренадлежит двум контейнерам?
  /*
     \param  ссылки на объекты
     \return true, если если контейнеры содержат разные обьект
  */
  template<class Type>
  bool operator!=(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();

  //!        оператор меньше
  /*
     \param  ссылки на объекты
     \return true, если если левый операнд меньше правого
  */
  template<class Type>
  bool operator<(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
  //!        оператор больше
  /*
     \param  ссылки на объекты
     \return true, если если левый операнд больше правого
  */
  template<class Type>
  bool operator>(const CSharedPtr<Type>& rLeft, const CSharedPtr<Type>& rRight) throw();
  
} // namespace rise

#include "SharedPtr.hpp"

#endif // _SHAREDPTR_H_
