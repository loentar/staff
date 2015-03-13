# Introduction #

This article explains how to run Staff under IIS 6.0. The procedure has been tested on Windows Server 2003 and the instructions assume the build tool is Microsoft Visual Studio.

# Procedure #

  1. Download and install Staff and Axis2/C. Let’s assume it is installed in c:\ws<br><br>
<ol><li>Download the unofficial 1.6.0 version of Axis2/C. Let’s assume it is installed in c:\axis2-unofficial.<br><br>
</li><li>Install a version of Microsoft Visual Studio that is compatible with the Staff distribution that was downloaded. For the December 2013 binary distribution of Staff, this would be Microsoft Visual Studio 2010.<br><br>
</li><li>Start Visual Studio and go to the Tools menu to start a Visual Studio Command Prompt. In there, issue the command <i>cd c:\axis2-unofficial\build\win32</i>, followed by the command <i>nmake install</i>. This should build Axis2/C.<br><br>
</li><li>Enter the command, <i>nmake mod_axis2_iis</i>. This should build the Axis2/C IIS module and it will be located at c:\axis2-unofficial\build\deploy\lib\mod_axis2_IIS.dll. Copy this file to c:\ws\axis2c\lib.<br><br>
</li><li>Start Internet Information Services (IIS) Manager from Start/Administrative Tools.  Go to “<code>&lt;machine&gt;</code> (local computer)/Web Service Extensions” and ensure that “All Unknown ISAPI Extensions” has a status of “Allowed”.<br><br>
</li><li>Create either a new web site, a virtual directory or use the “Default Web Site” for the Staff installation. Set the home folder to be c:\inetpub\wwwroot (in fact any folder other than the root Staff folder will do).<br><br>
</li><li>Select “Properties” on the web site (or virtual directory), then select the “Home Directory” tab and press the “Configuration” button. In the dialog that pops up, select the “Mappings” tab and press the “Insert” button for the section entitled “Wildcard application maps”. Add the IIS module at c:\ws\axis2c\lib\mod_axis2_IIS.dll and ensure the setting for “Verify that file exists” is unchecked.<br><br>
</li><li>Set write permissions for IIS on c:\ws\staff\staff.db. To do this, select Properties from Windows Explorer on that file, select the “Security” tab and press the “Add” button for the “Groups or user names” section. The easiest was to find the correct user is to press the “Advanced” button on the dialog that pops up and then the “Find Now” button. Select the user which begins with IUSR, usually it is IUSR<code>_&lt;machine&gt;</code> and press the OK button. Highlight the user and ensure that write permission is granted, then press the OK button to set.<br><br>
</li><li>Set up the registry for the IIS module. Run the registry editor and create the following key:<br><br>    <code>HKEY_LOCAL_MACHINE/SOFTWARE/Apache Axis2C/IIS ISAPI Redirector</code><br><br>Beneath this key create the following REG_SZ (string) key values:<br>
<blockquote><code>axis2c_home = c:\ws\axis2c </code><br>
<code>log_file = c:\ws\axis2c\logs\axis2.log </code><br>
<code>log_level = one of crit, error, warn, info, debug or trace </code>
</blockquote></li><li>Start the System applet from Control Panel. Select the “Advanced” tab and press the “Environment Variables” button. Set up the following system environment variables and press the OK button on both dialogs:<br>
<blockquote><code>STAFF_HOME = c:\ws\staff </code><br>
<code>PATH = %PATH%;c:\ws\axis2c\lib;c:\ws\staff\lib;c:\ws\staff\bin </code>
</blockquote></li><li>Reboot the system and then in a browser navigate to <a href='http://localhost/axis2/services'>http://localhost/axis2/services</a>, assuming the web site mentioned in step 7 is listening on port 80. The Axis2 and Staff services should be listed.