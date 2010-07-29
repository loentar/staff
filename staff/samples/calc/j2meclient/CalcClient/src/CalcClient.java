import javax.microedition.io.*;
import java.util.*;
import java.io.*;

import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

import org.ksoap2.*;
import org.ksoap2.serialization.*;
import org.ksoap2.transport.*;
import org.kxml2.kdom.*;

import samples.calc.CalcService;

public class CalcClient extends MIDlet implements CommandListener, Runnable
{
  // user interface
  Form mainForm = new Form("Calc");
  TextField tFieldA = new TextField("A=", "1", 5, TextField.NUMERIC);
  TextField tFieldB = new TextField("B=", "2", 5, TextField.NUMERIC);
  StringItem resultItem = new StringItem("Result", "");
  Command cmdAdd = new Command("Add", Command.SCREEN, 0);
  Command cmdSub = new Command("Sub", Command.SCREEN, 1);

  String sCmd;

  // CalcService
  CalcService tCalcService = new CalcService();


  public CalcClient()
  {
    mainForm.append(tFieldA);
    mainForm.append(tFieldB);
    mainForm.append(resultItem);
    mainForm.addCommand(cmdAdd);
    mainForm.addCommand(cmdSub);
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

      int nResult = 0;
      if (sCmd == "Add")
      {
        nResult = tCalcService.Add(nA, nB);
      }
      else
      if (sCmd == "Sub")
      {
        nResult = tCalcService.Sub(nA, nB);
      }
      else
      {
        resultItem.setText("Error: Unknown Operation");
      }

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
    if (c == cmdAdd)
    {
      sCmd = "Add";
      new Thread(this).start();
    }
    else
    if (c == cmdSub)
    {
      sCmd = "Sub";
      new Thread(this).start();
    }
  }
}

