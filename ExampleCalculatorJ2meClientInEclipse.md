<h1>Table of contents</h1>


## Dependencies ##
Must have the following dependencies:
  * Java SE (JDK);
  * Sun Java Wireless Toolkit (WTK);
  * Eclipse with WTP, MTJ;
  * Service samples.Calculator from ExampleCalculatorService;

## Creating a J2ME client Calculator ##
### Creating a Project ###
  1. Start Eclipse and select "File" => "New" => "MIDlet Project";
  1. Enter the name of the project in the "Project Name": Calculator;
  1. If the configuration list is empty (the group "Configurations"), then click "Add ...", select the SDK, and Device, click" Finish ";
  1. Click "Finish"; will open your project;

### Generating client proxy ###
Copy Calculator's interface from ExampleCalculatorService into project's source dir.

Start codegen:
```
staff_codegen -tj2meclient Calculator.h
```

Client proxy `samples/Calculator.java` must be generated.

### Installing ksoap ###
Download file `ksoap2-j2me-core-2.1.2.jar` from http://sourceforge.net/projects/ksoap2/ and put it into project's lib dir `Calculator/lib'

### Project Configuration ###
  1. In the "Package exporer" select project "Calculator" and refresh the project by pressing F5;
  1. In the "Package exporer" right-click on the project, "Build Path" => "Configure Build Path ...";
  1. Tab "Libraries", click "Add JARs ...", and select Calculator/lib/ksoap2-j2me-core-2.1.2.jar, Click "OK";
  1. Tab "Order and Export" check the box near the item "ksoap2-j2me-core-2.1.2.jar - Calculator/lib", Click "OK";

### Implementing MIDlet ###
  1. Create the main MIDlet class: right-click the project name, "New" => "Java ME MIDlet";
  1. In the "Name:" enter the name of the main class "CalculatorMIDlet", click "Finish";
  1. Eclipse generates MIDlet's source code. Paste the source code replacing generated:
```
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import samples.Calculator;

public class CalculatorMIDlet extends MIDlet implements CommandListener
{
  Form mainForm = new Form("Calculator");
  TextField tFieldA = new TextField("A=", "1", 5, TextField.NUMERIC);
  TextField tFieldB = new TextField("B=", "2", 5, TextField.NUMERIC);
  StringItem resultItem = new StringItem("Result", "");
  Command addCommand = new Command("Add", Command.SCREEN, 1);

  Calculator tCalcService = new Calculator();

  public CalculatorMIDlet()
  {
    mainForm.append(tFieldA);
    mainForm.append(tFieldB);
    mainForm.append(resultItem);
    mainForm.addCommand(addCommand);
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

  public void commandAction(Command c, Displayable d)
  {
    if (c == addCommand)
    {
      new Thread()
      {
        public void run()
        {
          try
          {
            int nA = Integer.parseInt(tFieldA.getString());
            int nB = Integer.parseInt(tFieldB.getString());

            int nResult = tCalcService.Add(nA, nB);

            resultItem.setText(String.valueOf(nResult));
          }
          catch (Exception e)
          {
            e.printStackTrace();
            resultItem.setLabel("Error:");
            resultItem.setText(e.toString());
          }
        }
      }.start();
    }
  }
}
```


### Building and starting ###
Eclipse automatically compiles the project.

To start the project, press Ctrl+F11 or choose "Run" => "Run" from the main menu.

If You need to create jar file, please right click on the project and select "Mobile Tools for Java" => "Create Package".

## Testing ##
When emulator window opens, press F2 to invoke "Add" operation in samples.Calculator service.

Press F2 to confirm network access(only in first time).

You must see result in "Result" field as follows.

![![](http://staff.googlecode.com/svn/wiki/img/j2me-calculator.png)](http://staff.googlecode.com/svn/wiki/img/j2me-calculator.png)

## Download ##

Download source code [here](http://code.google.com/p/staff/downloads/detail?name=samples.Calculator-j2me-src.7z)

Download jar [here](http://code.google.com/p/staff/downloads/detail?name=samples.Calculator-j2me-jar.7z)
