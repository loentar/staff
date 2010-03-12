#ifndef _INTERFACEINFO_H_
#define _INTERFACEINFO_H_

#include <iosfwd>
#include <string>
#include <list>

//! тип данных
struct SDataType 
{
  enum EDataType  //! тип данных
  {
    EUnknown,     //!<  неизвестный
    EGeneric,     //!<  встроеннный(поддерживаемый staff)
    EDataObject,  //!<  обьект данных
    EStruct,      //!<  структура
    ETypedef,     //!<  обьявление типа
    ETemplate     //!<  шаблон
  };

  bool                    bIsConst;    //!<  объявлен как const
  bool                    bIsRef;      //!<  объявлен как ссылка
  EDataType               eType;       //!<  тип данных
  std::string             sName;       //!<  имя типа
  std::string             sNamespace;  //!<  namespace
  std::list<SDataType>    lsParams;    //!<  шаблонные параметры
};

//!  параметр
struct SParam 
{
  SDataType    stDataType;  //!<  тип данных параметра
  std::string  sName;       //!<  название переменной
};

//! функция
struct SMember
{
  SDataType          stReturn;    //!<  возвращаемое значение
  std::string        sName;       //!<  имя функции
  std::list<SParam>  lsParamList; //!<  список параметров
  bool               bIsConst;    //!<  объявлен как const
};

//! класс
struct SClass
{
  std::string         sName;          //!<  имя класса
  std::string         sNamespace;     //!<  namespace
  std::list<SMember>  lsMember;       //!<  список функций
};

//! структура
struct SStruct
{
  std::string         sName;          //!<  имя структуры
  std::string         sNamespace;     //!<  namespace
  std::list<SParam>   lsMember;       //!<  список свойств
};

//! тип данных
struct STypedef
{
  std::string         sName;          //!<  имя типа
  std::string         sNamespace;     //!<  namespace
  SDataType           stDataType;     //!<  тип данных
};

//! интерфейс
struct SInterface
{
  std::string           sName;          //!<  имя интерфейса, базирован на имени заголовочного файла
  std::string           sFileName;      //!<  имя заголовочного файла, в котором обьявлен интерфейс
  std::list<STypedef>   lsTypedef;      //!<  список типов данных
  std::list<SStruct>    lsStruct;       //!<  список структур
  std::list<SClass>     lsClass;        //!<  список классов
};

//! проект
struct SProject
{
  std::string            sName;         //!<  имя проекта
  std::string            sInDir;        //!<  входной каталог
  std::string            sOutDir;       //!<  выходной каталог
  std::list<SInterface>  lsInterfaces;  //!<  список интерфейсов
};

std::istream& operator>>(std::istream& rStream, SDataType& rDataType);
std::istream& operator>>(std::istream& rStream, SParam& rParameter);
std::istream& operator>>(std::istream& rStream, SMember& rMember);
std::istream& operator>>(std::istream& rStream, SClass& rClass);
std::istream& operator>>(std::istream& rStream, SStruct& rStruct);
std::istream& operator>>(std::istream& rStream, STypedef& rTypedef);
std::istream& operator>>(std::istream& rStream, SInterface& rInterface);

#ifdef DEBUG
std::ostream& operator<<(std::ostream& rStream, const SDataType& rDataType);
std::ostream& operator<<(std::ostream& rStream, const SParam& rParameter);
std::ostream& operator<<(std::ostream& rStream, const SMember& rMember);
std::ostream& operator<<(std::ostream& rStream, const SClass& rClass);
std::ostream& operator<<(std::ostream& rStream, const SInterface& rInterface);
#endif

std::ostream& operator<<(std::ostream& rStream, const SDataType::EDataType eDataType);

void ResetLine();
int& GetLine();

#endif // _INTERFACEINFO_H_
