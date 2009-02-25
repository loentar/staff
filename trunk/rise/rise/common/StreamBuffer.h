#ifndef _StreamBuffer_h_
#define _StreamBuffer_h_

#include <rise/common/types.h>
#include <rise/common/ByteOrder.h>

namespace rise
{
  //!        потоковый буфер
  class RISE_EXPORT CStreamBuffer
  {
  public:
    enum EFlags  //! флаги опций работы буфера
    {
      EF_NONE = 0,                //!< нет флагов
      EF_STR_NO_LEADING_ZERO = 1  //!< отключить добавление \0 при сериализации/десериализации строк
    };

  public:
    //!        конструктор
    CStreamBuffer();

    //!        копирующий конструктор
    /*
       \param  rBuffer - исходный буфер
    */
    CStreamBuffer(const CStreamBuffer& rBuffer);

    //!        деструктор
    ~CStreamBuffer();

    //!        оператор копирования
    /*
       \param  rBuffer - исходный буфер
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator=(const CStreamBuffer& rBuffer);

    //!        установка флагов ввода вывода
    /*
       \sa     EFlags
       \param  nFlags
    */
    void SetFlags(int nFlags);

    //!        установка флагов по умолчанию(применятся для всех ново созданных буферов)
    /*
       \sa     EFlags
       \param  nFlags
    */
    static void SetDefaultFlags(int nFlags);

    //!        получение флагов ввода вывода
    /*
       \return флаги
    */
    int GetFlags() const;

    //!        получение флагов по умолчанию ввода вывода
    /*
       \return флаги по умолчанию
    */
    static int GetDefaultFlags();

    //!        установить порядок байт
    /*
       \param  eByteOrder - порядок байт
    */
    void SetByteOrder(CByteOrder::EByteOrder eByteOrder = CByteOrder::GetDefaultByteOrder());

    //!        установить порядок байт по умолчанию(применятся для всех ново созданных буферов)
    /*
       \param  eByteOrder - порядок байт
    */
    static void SetDefaultByteOrder(CByteOrder::EByteOrder eByteOrder = CByteOrder::GetDefaultByteOrder());

    //!        получить порядок байт
    /*
       \return порядок байт
    */
    CByteOrder::EByteOrder GetByteOrder() const;

    //!        получить порядок байт по умолчанию
    /*
       \return порядок байт
    */
    static CByteOrder::EByteOrder GetDefaultByteOrder();

    //!        поместить в буфер данные (скопировать данные из другого буфера)
    /*
       \param  pData - указатель на данные
       \param  ulDataSize - размер данных(количество символов для строки)
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Put(PCBuffer pData, TSize ulDataSize);

    //!        поместить в буфер данные (скопировать данные из другого буфера)
    /*
       \param  rData - буфер данных
       \param  ulDataSize - размер данных(количество символов для строки)
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Put(const CStreamBuffer& rData, TSize ulDataSize);

    //!        поместить в буфер однобайтовую строку
    /*
       \param  sData - однобайтовая строка
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Put(CStringA& sData);

    //!        поместить в буфер двубайтовую строку
    /*
       \param  sData - двубайтовая строка
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Put(CStringW& sData);

    //!        прочесть из буфера данные
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  pData - указатель на данные
       \param  ulDataSize - размер данных
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(PBuffer pData, TSize ulDataSize);

    //!        прочесть из буфера данные
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  rData - указатель на данные
       \param  ulDataSize - размер данных
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(CStreamBuffer& rData, TSize ulDataSize);

    //!        прочесть из буфера строку указанной длинны
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  sData - строка
       \param  ulDataSize - количество символов для строки
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(CStringA& sData, TSize ulDataSize);

    //!        прочесть из буфера строку указанной длинны
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  sData - строка
       \param  ulDataSize - количество символов для строки
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(CStringW& sData, TSize ulDataSize);

    //!        прочесть из буфера строку 
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  sData - строка
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(CStringA& sData);

    //!        прочесть из буфера строку 
    /*
       исключения: CLogicSizeException - недостаточно данных для чтения
       \param  sData - строка
       \return ссылка на обьект буфера
    */
    CStreamBuffer& Get(CStringW& sData);

    //!        пропустить чтение N байт
    CStreamBuffer& Ignore(TSize ulDataSize);

    //!        Записать данные в буфер со смещением
    /*
       исключения:     CLogicSizeException - несоответствие размеров буферов
       \param  pData - указатель на буфер данных
       \param  ulDataSize - размер данных
       \param  ulOffset - смещение относительно начало буфера
       \return ссылка на обьект буфера
    */
    CStreamBuffer& WriteBuffer(PCBuffer pData, TSize ulDataSize, TSize ulOffset);

    //!        Записать данные в буфер со смещением
    /*
       исключения:     CLogicSizeException - несоответствие размеров буферов
       \param  rsbData - буфер данных
       \param  ulOffset - смещение относительно начало буфера
       \return ссылка на обьект буфера
    */
    CStreamBuffer& WriteBuffer(const CStreamBuffer& rsbData, TSize ulOffset);

    //!        сброс данных
    void Reset();

    //!        нормализация(перенос блока данных в начало буфера)
    void Normalize();

    //!        получить указатель на начало данных
    /*
       \return:         указатель на начало данных, NULL - буфер пуст
    */
    PBuffer GetData();

    //!        получить константный указатель на начало данных
    /*
       \return:         константный указатель на начало данных, NULL - буфер пуст
    */
    PCBuffer GetData() const;
   
    //!        получить указатель на начало буфера
    /*
       \return указатель на начало буфера, NULL - буфер пуст
    */
    PBuffer GetBuffer();

    //!        получить константный указатель на начало буфера
    /*
       \return константный указатель на начало буфера, NULL - буфер пуст
    */
    PCBuffer GetBuffer() const;

    //!        получение текущего размера данных в буфере
    /*
       \return текущий размер данных в буфере
    */
    TSize GetSize() const;

    //!        получить размер всего буфера
    /*
       \return размер всего буфера
    */
    TSize GetBufferSize() const;

    //!        получить смещение чтения относительно начала буфера
    /*
       \return смещение чтения
    */
    TSize GetROffset() const;

    //!        получить смещение записи относительно начала буфера
    /*
       \return смещение записи
    */
    TSize GetWOffset() const;
    
    //!        увеличить размер данных в буфере
    /*
       \param  ulSize - размер на который необходимо увеличить буфер
       \return указатель на начало выделенных данных
    */
    PBuffer Grow(const TSize ulSize);

    //!        изменение размера данных
    /*
       \param  ulSize - новый размер данных
       \return none
    */
    void Resize(const TSize ulSize);

    //!        при необходимости, выделить как минимум ulSize под буфер
    /*
       \param  ulSize - размер буфера
       \return none
    */
    void Reserve(const TSize ulSize);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(char& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(unsigned char& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(short& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(unsigned short& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(int& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(unsigned int& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(long& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(unsigned long& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(float& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(double& tData);  

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(CStringA& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(CStringW& tData);

    //!        оператор сериализации
    /*
       \param  tData - ссылка на объект сериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator>>(CStreamBuffer& tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const char tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const unsigned char tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const short tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const unsigned short tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const int tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const unsigned int tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const long tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const unsigned long tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const float tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const double tData);  

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const CStringA& tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const CStringW& tData);

    //!        оператор десериализации
    /*
       \param  tData - ссылка на объект десериализации
       \return ссылка на текущий буфер
    */
    CStreamBuffer& operator<<(const CStreamBuffer& tData);

  private:
    PBuffer m_pucBegin;                               //! указатель на начало буфера
    TSize m_ulROffset;                                //! смещение для чтения
    TSize m_ulWOffset;                                //! смещение для записи
    TSize m_ulSize;                                   //! размер данных
    TSize m_ulBufferSize;                             //! размер буфера
    int   m_nFlags;                                   //! флаги io
    CByteOrder::EByteOrder m_eByteOrder;              //! порядок байт
    
    static int m_nFlagsStatic;                        //! флаги io по умолчанию
    static CByteOrder::EByteOrder m_eByteOrderStatic; //! кодировка по умолчанию
  };

} // namespace rise

#endif // _StreamBuffer_h_
