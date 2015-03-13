<h1>Table of contents</h1>


## Dependencies ##

Must have the following dependencies installed and configured:
  * Axis2/C;
  * WSF Staff;
  * Qt Creator;

## Generating client project ##

  1. Create a directory where StockQuote client will be placed;
  1. Enter that directory and start codegenerator to produce client project from wsdl:
```
# to create blocking (synchronous) version
staff_codegen -pwsdl -tclient_all http://www.webservicex.net/stockquote.asmx?WSDL
# to create non-blocking (asynchronous) version
# staff_codegen -pwsdl -tclient_all -dnonblocking http://www.webservicex.net/stockquote.asmx?WSDL
```


## Implementing client ##

Start Qt-Creator, click "Open Project..." button in "Welcome" mode or select "File" => "Open File or Project..." from main menu.

Navigate the `stockquote.pro` then click "Open".

In "Project Setup" window that opens click "Finish".

In Projects pane navigate main.cpp in "Sources/src" folder and open it.

Implement the client under "Invoke Your service here:" comment:

For example, if you're using _blocking_ version:
```
#include <memory>
#include <QtCore/QCoreApplication>
#include <QDebug>
#include <staff/utils/Log.h>
#include <staff/common/logoperators.h>
#include <staff/common/Exception.h>
#include <staff/client/ServiceFactory.h>
#include "stockquote.h"


int main(int nArgs, char* paszArgs[])
{
  QCoreApplication qCoreApp(nArgs, paszArgs);

  try
  {
    std::auto_ptr< ::StockQuote > pStockQuote(::staff::ServiceFactory::Inst().GetService< ::StockQuote >());

    STAFF_ASSERT(pStockQuote.get(), "Cannot get client for service StockQuote!");

    // Invoke Your service here:

    staff::Optional<std::string> symbol("IBM");
    qDebug() << "Getting Quote for symbol: " << symbol->c_str();
    const staff::Optional<std::string>& GetQuoteResponse = pStockQuote->GetQuote(symbol);
    qDebug() << "GetQuote result: " << GetQuoteResponse;
  }
  STAFF_CATCH_ALL

  qDebug() << "\nexecuting application...";
  return qCoreApp.exec();
}
```

And if you're using _non-blocking_ version:
```
#include <memory>
#include <QtCore/QCoreApplication>
#include <QDebug>
#include <staff/utils/Log.h>
#include <staff/utils/Thread.h>
#include <staff/common/DataObject.h>
#include <staff/client/ICallback.h>
#include <staff/common/logoperators.h>
#include <staff/common/Exception.h>
#include <staff/client/ServiceFactory.h>
#include "stockquote.h"

// callbacks

// callback for StockQuote::GetQuote
class StockQuoteGetQuoteCallback: public staff::ICallback< const staff::Optional<std::string>& >
{
public:
  virtual void OnComplete(const staff::Optional<std::string>& tResult)
  {
    // process result here
    qDebug() << "GetQuote(asynch) result: " << (tResult.IsNull() ? "(null)" : tResult->c_str());
  }

  void OnFault(const staff::DataObject& rFault)
  {
    // process error here
    qCritical() << rFault.ToString().c_str();
  }
};



int main(int nArgs, char* paszArgs[])
{
  QCoreApplication qCoreApp(nArgs, paszArgs);

  try
  {
    std::auto_ptr< ::StockQuote > pStockQuote(::staff::ServiceFactory::Inst().GetService< ::StockQuote >());

    STAFF_ASSERT(pStockQuote.get(), "Cannot get client for service StockQuote!");

    // Invoke Your service here:

    staff::Optional<std::string> symbol("IBM");
    qDebug() << "Getting Quote for symbol: " << symbol->c_str();
    StockQuoteGetQuoteCallback tStockQuoteGetQuoteCallback;
    pStockQuote->GetQuote(symbol, tStockQuoteGetQuoteCallback);

    // Wait for asynch call is completed
    // Please note, that you cannot call any operation of this client
    // until this asynchronous request is completed.
    // To call operation while request is processing, please use another copy of client.
    while (!tStockQuoteGetQuoteCallback.IsCompleted())
    {
      staff::Thread::Sleep(1000);
    }

  }
  STAFF_CATCH_ALL

  qDebug() << "\nexecuting application...";
  return qCoreApp.exec();
}
```

## Compiling and starting ##

Compile application in Qt Creator.

When you start application you should see result like:
```
Getting Quote for symbol:  IBM 
GetQuote result: 
 <StockQuotes><Stock><Symbol>IBM</Symbol><Last>163.40</Last><Date>2/16/2011</Date><Time>4:00pm</Time><Change>0.00</Change><Open>163.33</Open><High>N/A</High><Low>N/A</Low><Volume>0</Volume><MktCap>201.0B</MktCap><PreviousClose>163.40</PreviousClose><PercentageChange>0.00%</PercentageChange><AnnRange>116.00 - 166.25</AnnRange><Earns>11.52</Earns><P-E>14.18</P-E><Name>International Bus</Name></Stock></StockQuotes> 

executing application... 
```

## Download source code ##

Download source code [here](http://code.google.com/p/staff/downloads/detail?name=qt-soap-stockquote-a1.7z)
