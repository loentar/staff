Use Calculator client from ExampleCalculatorClient.

## 1. Enabling non blocking call ##

To set client operation as non blocking please edit client's interface file `Calculator.h`.

Add line into class Calculator as follows:
```
virtual void Add(int nA, int nB, staff::ICallback<int>& rCallback) = 0;
```

This operation must have void return type. Data type of operation's result is in template parameter of ICallback.

## 2. Rewrite client implementation ##

You should implement callback for nonblocking operation.

Easy way is to completely regenerate client implementation by entering codegen's command from Calculator/client/src directory:
```
staff_codegen -tclientimpl Calculator.h
```

Now edit `main.cpp` to implement client.

Uncomment next lines in CalculatorAddCallback class function OnComplete:
```
staff::LogInfo() << "Add(asynch) result: " << tResult;
```

Uncomment next lines in function main:
```
CalculatorAddCallback tCalculatorAddCallback;
pCalculator->Add(nA, nB, tCalculatorAddCallback);

while (!tCalculatorAddCallback.IsCompleted())
{
  staff::Thread::Sleep(1000);
}
```

Edit call to service. Fill the arguments:
```
pCalculator->Add(1, 2, tCalculatorAddCallback);
```


## 3. Regenerate proxy ##

Regenerate client proxy by entering codegen's command from Calculator/client/src directory:
```
staff_codegen -tclient Calculator.h
```

Compile client.

## 4. Test non blocking client ##

Start modified client.
Screen output should be:
```
INFO main.cpp[22]: OnComplete: Add(asynch) result: 3
```