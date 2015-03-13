<h1>Table of contents</h1>


## Dependencies ##
Must have the following dependencies:
  * Java SE (JDK);
  * Eclipse with WTP;
  * Apache Axis2
  * Tomcat v.7 Server
  * Service samples.Calculator Wsdl file ;

## Creating a J2EE client Calculator ##
### Creating a Project ###
  1. Start Eclipse and select "File" => "New" => "Dynamic Web Project";
  1. Enter the name of the project in the "Project Name": Calculator;
  1. Select Dynamic web module version 2.5 (I don't know if 3.0 has compatibility with Axis2)
  1. Click "Next"
  1. Click "Finish"; will open your project in J2EE perspective;

### Configuring Apache Axis2 in Eclipse ###
  1. Download Apache Axis2 from http://axis.apache.org/axis2/java/core/download.cgi (the .war file)
  1. Save .war file donwloaded in a directory you want.
  1. On Eclipse go Window -> Preferences -> Web Services -> Axis2 Preferences.
  1. Set Axis2 runtime location (directory where you left the Axis2 war file). If it succesful will show "Axis2 runtime loaded succesfully".
  1. Click "Apply" and "OK"
  1. Select your project. "Right button" -> "Properties" -> "Project Facets". Check "Axis2 Web Services"
  1. Click "Apply" and "OK"

### Generating client proxy ###
  1. Select your project and then with right button mouse select "New" -> "Other... " -> "Web services" -> Web service client" This shows the webservice dialog. Select the service information box: http://localhost:9090/axis2/services/staff.wsdl.Wsdl/get/samples.calc/samples.calc.CalcService.wsdl (Remember you must have "axis\_http\_server" on.)
  1. Leave "Client type" as Java Proxy.
  1. Change the scroll pad to "Test Client". Default is "Deploy Client"
  1. Set your server runtime to point Tomcat server.
  1. Web service runtime to Apache Axis 2 (Default is Apache Axis)
  1. Click "Monitor the Web Service" if you want to see SoapResquest and SoapResponse (very useful).
  1. Click "Next". An Axis2 client webservice configuration is shown. (Change parameters you want)
  1. Click "Next" and "Finish". At this point all stubs and jsp test examples are created and "Web services Explorer" opens to you...


## Testing ##

If you want later display the Web services Explorer, from the main menu bar, select Run -> Launch the Web Services Explorer . This will take a few moments the first time while Eclipse's internal Tomcat server starts up and loads the Web Services Explorer application.
Then navigate to the method you want to launch (Add, Subs...)

### Implementing a Java Class for testing ###

> If you want only create a java client without creating a J2EE application (Remember at least to create stubb classes).
> You can test with a simple java class:

```

package org.sample.com;

import java.rmi.RemoteException;

import org.apache.axis2.AxisFault;
import org.sample.com.CalcServiceStub.Add;
import org.sample.com.CalcServiceStub.AddResponse;

public class CalcClient {
	
	public static void main(String[] args) {
		try {
			CalcServiceStub stub = new CalcServiceStub();
			Add addition = new Add();
			addition.localNA = 2;
			addition.localNB = 3;
			AddResponse resp = new AddResponse();
			resp = stub.add(addition);
			System.out.println("result :" + resp.getAddResult());
			
		} catch(AxisFault e) {
			System.out.println("AxisFault exception " + e.getMessage());
			
		} catch(RemoteException e) {
			System.out.println("Remote exception " + e.getMessage());
			
		}
	}
	

}
```