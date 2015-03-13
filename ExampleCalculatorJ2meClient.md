<h1>Table of Contents</h1>


## Installing J2ME WTK ##
To install J2ME WTK follow instructions located in:
http://www.oracle.com/technetwork/java/download-135801.html

## Creating Calculator J2ME client ##
### Creating project ###
Start J2ME WTK (KToolbar) and click "New Project".
```
Project Name: Calculator
MIDlet Class Name: CalculatorMIDlet
```
Click "Create Project", Click "OK".

In the text output pane of the KToolbar, you'll see several helpful messages telling you where to store the project's source files.

### Installing ksoap ###
Download file `ksoap2-j2me-core-2.1.2.jar` from http://sourceforge.net/projects/ksoap2/ and put it into project's lib dir `Calculator/lib'

### Implementing MIDlet ###
Place MIDlet's source code into project's source dir as CalculatorMIDlet.java:
```
import javax.microedition.io.*;
import java.util.*;
import java.io.*;

import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import org.ksoap2.*;
import org.ksoap2.serialization.*;
import org.ksoap2.transport.*;
import org.kxml2.kdom.*;

import samples.Calculator;

public class CalculatorMIDlet extends MIDlet implements CommandListener, Runnable
{
    Form mainForm = new Form("Calculator");
    TextField tFieldA = new TextField("A=", "1", 5, TextField.NUMERIC);
    TextField tFieldB = new TextField("B=", "2", 5, TextField.NUMERIC);
    StringItem resultItem = new StringItem("Result", "");
    Command getCommand = new Command("Add", Command.SCREEN, 1);

    Calculator tCalcService = new Calculator();

    public CalculatorMIDlet()
    {
        mainForm.append(tFieldA);
        mainForm.append(tFieldB);
        mainForm.append(resultItem);
        mainForm.addCommand(getCommand);
        mainForm.setCommandListener(this);
    }

    public void startApp()
    {
        Display.getDisplay(this).setCurrent(mainForm);
    }

    public void pauseApp()
    {
    }

    public void destroyApp(boolean unconditional)
    {
    }

    public void run()
    {
      try
      {
        int nA = Integer.parseInt(tFieldA.getString());
        int nB = Integer.parseInt(tFieldB.getString());

        int nResult = tCalcService.Add(nA, nB);

        resultItem.setText(Integer.toString(nResult));
      }
      catch (Exception e)
      {
        e.printStackTrace();
        resultItem.setLabel("Error:");
        resultItem.setText(e.toString());
      }
    }

    public void commandAction(Command c, Displayable d)
    {
      new Thread(this).start();
    }
}
```

### Generating client proxy ###
Copy Calculator's interface (ExampleCalculatorService) into project's source dir.

Start codegen:
```
staff_codegen -tj2meclient Calculator.h
```

Client proxy `samples/Calculator.java` must be generated.

### Building and starting ###
Click "Build" in KToolbar and then "Run".

## Testing ##
When emulator window opens, press F2 to Launch Calculator MIDlet.

Press F2 to invoke "Add" operation in samples.Calculator service.

Press F2 to confirm network access(only in first time).

You must see result in "Result" field as follows.

![![](http://staff.googlecode.com/svn/wiki/img/j2me-calculator.png)](http://staff.googlecode.com/svn/wiki/img/j2me-calculator.png)
