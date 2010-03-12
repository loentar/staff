#ifndef _EXCODES_H_
#define _EXCODES_H_

namespace rise
{
  enum EXCLASS  //! классы исключений
  {
    EXCFILE,     //!< файловые исключения
    EXCLOGIC,    //!< логические исключения
    EXCINTERNAL, //!< внутренние исключения
    EXCXML,      //!< XML исключения
    EXCLAST
  };

  enum EXCODE  //! коды исключений
  {
    EXCGENERIC,        //!< стандартное

    // файловые исключения
    EXCREATE,          //!< создание
    EXDELETE,          //!< удаление
    EXOPEN,            //!< открытие
    EXCLOSE,           //!< закрытие
    EXREAD,            //!< чтение
    EXWRITE,           //!< запись
    EXIO,              //!< установка режима ввода/вывода

    // логические исключения
    EXNOINIT = 32,     //!< не произведена инициализация
    EXALREADYEXISTS,   //!< попытка повторной инициализации/создания
    EXCONTINUE,        //!< продолжить операцию
    EXSIZE,            //!< неверный размер
    EXNOITEM,          //!< заданный элемент не найден
    EXTIMEOUT,         //!< истекло время ожидания
    EXNOPERMISSION,    //!< нет прав
    EXFORMAT,          //!< неверный формат
    EXPARAM,           //!< неверный параметр

    // внутренние исключения
    EXINTERNAL = 64,   //!< внутренняя ошибка
    EXNOMEM,           //!< ошибка при выделении памяти
    EXASSERT,          //!< проверка утверждения
    EXNOTIMPLEMENTED,  //!< не реализовано
    EXTEMPDECISION     //!< временное решение
  };

} // namespace rise

#endif // #ifndef _EXCODES_H_
