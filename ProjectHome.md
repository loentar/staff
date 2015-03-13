Staff is a complete solution for building and deploying Web services in C++.

<font color='darkred'><b>Web service's interface example:</b></font><font size='1'>
<pre><code>namespace samples {<br>
  //! user<br>
  struct User {<br>
    int id; //!&lt; user id<br>
    std::string name; //!&lt; user name<br>
    std::string description; //!&lt; user description<br>
  };<br>
<br>
  //! user manager service<br>
  class UserManager: public staff::IService {<br>
  public:<br>
    virtual int Add(const std::string&amp; name, const std::string&amp; description) = 0;<br>
    virtual void Remove(int id) = 0;<br>
    virtual void Update(const User&amp; user) = 0;<br>
    virtual User Get(int id) const = 0;<br>
    virtual std::list&lt;User&gt; GetAllUsers() const = 0;<br>
  };<br>
} // namespace samples<br>
</code></pre>
</font>

See also: **3 steps to [create web service](ExampleCalculatorService.md)**, **3 steps to [create web service client](ExampleCalculatorService.md)**, [Short examples](BriefExamples.md), [complete examples list](Examples.md), [Download examples](http://code.google.com/p/staff/downloads/list?q=samples)

## Latest News ##
  * Oct 31, 2014: VM WSF-Staff-SDK is available from [google drive](https://drive.google.com/file/d/0BwvdX1pBENi1SFkwR3hHU3l6YWM/view). Login: "user", Password: "user". VMWare Player 6.x is required to start this virtual machine.
  * Oct 31, 2014: WSF-Staff build [r876](https://code.google.com/p/staff/source/detail?r=876) with Axis2/C-unofficial 0913cc364461 is available from [google drive](https://drive.google.com/folderview?id=0BwvdX1pBENi1dWk1VUwySTdjY0E#list).
  * May 23, 2014: Added short guide on how to install WSF Staff under linux without having root permissions: InstallationManualLinuxUser .
  * **Dec 3, 2013: WSF-Staff Beta 1 is released.** Tarballs [r816](https://code.google.com/p/staff/source/detail?r=816) are available from [Download section](http://code.google.com/p/staff/downloads/list?q=r816). [Source code](https://code.google.com/p/staff/downloads/detail?name=wsf-staff-src-2.0.0-b1-r816.7z), Binaries for [MSVC 2010](https://code.google.com/p/staff/downloads/detail?name=wsf-staff-bin-2.0.0-b1-r816-vs10.7z),  [MinGW](https://code.google.com/p/staff/downloads/detail?name=wsf-staff-bin-2.0.0-b1-r816-mingw.7z),  [Linux](https://code.google.com/p/staff/downloads/detail?name=wsf-staff-bin-2.0.0-b1-r816-linux.7z),  [Android](https://code.google.com/p/staff/downloads/detail?name=wsf-staff-bin-2.0.0-b1-r816-android.7z).
  * 11.05.2013: Tarballs [r770](https://code.google.com/p/staff/source/detail?r=770) are available in [Download section](http://code.google.com/p/staff/downloads/list?q=r770). _This and further builds is now based on [axis2c-unofficial](https://code.google.com/p/axis2c-unofficial/)._
  * 04.04.2013: Updated [REST Web-client example](https://code.google.com/p/staff/source/browse/#svn%2Ftrunk%2Fstaff%2Fsamples%2Frest%2Fwebclient) to support JSON with [axis2c-unofficial](https://code.google.com/p/axis2c-unofficial/wiki/InstallationManualLinux).
  * 30.03.2013: Started project [Unofficial backports for Axis2/C-1.6.0](https://code.google.com/p/axis2c-unofficial/);
  * 28.03.2013: Added SunOS and Solaris Support;
  * 22.03.2013: Added wiki-page CodegenDefinitions;
<a href='Hidden comment: 
* 22.03.2013: Tarballs r748 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r748 Download section].
* 14.03.2013: Added [ExampleHelloWorldServiceAndroid example] of creating and deploying the web service for android. Updated [Screenshots Screenshots] wiki page.
* 12.03.2013: WSF Staff Android package is [https://code.google.com/p/staff/downloads/detail?name=WSFStaffAndroid-armeabi-r732.apk here]. To install scan QR-Code.
* 07.03.2013: Added Android support. See [InstallationManualAndroidBinary installation manual].
* 13.11.2012: Added wiki-page [KnownBugsAxis2c Axis2/C Known bugs].
* 07.11.2012: Tarballs r719 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r719 Download section]: new features and many bugfixes. Download [http://code.google.com/p/staff/downloads/list?can=2&q=r719+Debug Debug] or [http://code.google.com/p/staff/downloads/list?can=2&q=r719+Release Release] version.
* 07.11.2012: *Development status of project is now Alpha 2.*
* 14.08.2012: Added wiki-page WsfCodegensOverview: overview of C++/C SOA Frameworks codegenerator capabilities.
* 14.07.2012: Tarballs r686 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r686 Download section]: some new features and bugfixes. Download [http://code.google.com/p/staff/downloads/list?can=2&q=r686+Debug Debug] or [http://code.google.com/p/staff/downloads/list?can=2&q=r686+Release Release] version.
* 09.06.2012: Added support for SOAP Arrays.
* 05.06.2012: DAS: added support for prepared statements with parameters binding. To update your datasource please read the UpdateDatasourcesFromR674toR675.
* 23.05.2012: Tarballs r670 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r670 Download section]: some bugfixes.
* 03.04.2012: Tarballs r649 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r649 Download section], it contain important bugfixes and some DAS improvements.
* 30.03.2012: updated [http://code.google.com/p/staff/wiki/InstallationManualUbuntuPpa installation manual] for Ubuntu PPA.
* 29.03.2012: *Alpha 1* Tarballs r634 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r634 Download section]. Latest "stable" tp6 is [http://code.google.com/p/staff/downloads/list?can=2&q=r620 Here].
* 29.03.2012: Ubuntu PPA changed. Now there is two PPA: [https://launchpad.net/~loentar/+archive/wsf-staff-stable stable] and [https://launchpad.net/~loentar/+archive/wsf-staff-trunk trunk].
* 28.03.2012: *Project status is now Alpha 1.* If you looking for latest tp6 it"s branch is [http://staff.googlecode.com/svn/branches/staff-2.0.0-tp6 here].
* 12.03.2012: Tarballs svn revision 620 are available in [http://code.google.com/p/staff/downloads/list?can=2&q=r620 Download section]. (There are win32, win64 binaries for VS2005, VS2010 and gcc4.4.0 - QT SDK 1.1.4).
* 27.02.2012: Tarballs svn revision 606 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp6-r606.7z Sources], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp6-r606.7z MSVC win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win64-vs2005-staff-2.0.0-tp6-r606.7z MSVC win64 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp6-r606.7z GCC-4.5.2 win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.4.0-staff-2.0.0-tp6-r606.7z GCC-4.4.0 QtSDK 1.1.4 win32 bin];
* 21.02.2012: DAS services (datasources) can now be invoked using REST. [DasRestfulDatasources More info...]
* 17.02.2012: DAS: Added "dasdumpdb" codegen plugin, "datasources" and "datatypes" codegen templates to create datasources and datatypes files from database model. [DasDumpDb More info...];
* 15.02.2012: DAS: Added Shell provider to call scripts/executables from datasources. [DasProviders More info...];
* 02.02.2012: Staff can now be compiled against trunk version of Axis2/C-1.7.0 while using [http://code.google.com/p/staff/downloads/detail?name=axis2c-1.7.0-om.patch OM Patch];
* 25.01.2012: DAS: added support for [DasProviders Services provider] and [DasMultipleProviders multiple providers in datasources];
* 10.01.2012: Added installation manual [InstallationManualUbuntuPpa how to install WSF Staff from Ubuntu PPA].
* 11.12.2011: Ubuntu packages are available from PPA: https://launchpad.net/~loentar/+archive/ppa ;
* 21.11.2011: Added wiki pages [http://code.google.com/p/staff/wiki/InstallationManualWindowsMinGW InstallationManualWindowsMinGW], ExampleCalculatorCustomClient;
* 17.11.2011: Tarball svn revision 558 compatible with Qt SDK 1.1.4 is available from [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.4.0-qtsdk-1.1.4-staff-2.0.0-tp6-r558.7z download section].
* 27.10.2011: Tarballs svn revision 553 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp6-r553.7z Sources], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp6-r553.7z MSVC win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win64-vs2005-staff-2.0.0-tp6-r553.7z MSVC win64 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp6-r553.7z GCC win32 bin];
* 27.10.2011: r528-r552: many codegen improvements related to wsdl parsing such as: generate correct namespaces in soap requests/responses while using imported xsd types, java-style namespace extraction, elementFormDefault and attributeFormDefault support, etc...
* 1.09.2011: updated wiki documentation, see the updated [Examples Examples].
* 1.09.2011: Tarballs svn revision 523 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp6-r523.7z Sources], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp6-r523.7z MSVC win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win64-vs2005-staff-2.0.0-tp6-r523.7z MSVC win64 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp6-r523.7z GCC win32 bin];
* 18.08.2011: Tarballs svn revision 518 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp6-r518.7z Sources], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp6-r518.7z MSVC win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win64-vs2005-staff-2.0.0-tp6-r518.7z MSVC win64 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp6-r518.7z GCC win32 bin];
* 16.08.2011: r512: Creating client and service is now much more easier when using client_all and component_all codegen templates. See an updated example ExampleCreatingCppClientFromWsdl.
* 12.08.2011: r511: Codegen"s serialization templates for C++ (client and service) was totally rewritten. It must be much more stable. Serialization was additionally optimized also.
* 31.07.2011: r499: Added abstract types support. See the [https://code.google.com/p/staff/source/browse/trunk/staff/samples/abstract/client/src/Transport.h client interface example].
* 25.07.2011: r487: Added support for wsdl choice type. Many improvements relating to wsdl generating/parsing.
* 22.07.2011: r482: Added support for optional types. See [https://code.google.com/p/staff/source/browse/trunk/staff/samples/optional/client/src/Tasks.h client interface example].
* 10.07.2011: r474: Current unreleased version of Axis2/C-1.7.0 from [http://svn.apache.org/repos/asf/axis/axis2/c/core/trunk trunk] is now supported. To work with Axis2/C-1.7.0 need to apply the [http://code.google.com/p/staff/downloads/detail?name=axis2c-1.7.0-init_with_conf.patch patch].
* 07.07.2011: r469: Improved REST client support: generating correct URL for C++ RESTful clients; added [http://code.google.com/p/staff/downloads/detail?name=http_transport_utils.c.REST_DELETE.patch patch for Axis2/C-1.6.0] that fixes REST DELETE problem.
* 28.05.2011: Added support for 64-bit Windows. Tarball available from [http://code.google.com/p/staff/downloads/list?q=ws-win64 download section];
* 16.05.2011: Tarballs svn revision 460 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp5-r461.7z Sources], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp5-r460.7z MSVC win32 bin], [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp5-r460.7z GCC win32 bin];
* 14.05.2011: r459: fixed memleaks, optimized !DataObject, optimized serialization. *Staff is now runs up to 20-30% faster* (as shown by tests).
* 09.04.2011: r447: codegen - wsdl plugin: download wsdl and xsd schemas via http using axis2/c http client;
* 31.03.2011: r436: core: implemented hexBinary and base64Binary classes to pass binary data over SOAP [http://code.google.com/p/staff/source/browse/#svn%2Ftrunk%2Fstaff%2Fsamples%2Fbinarydata%2Fcomponent%2Fsrc see sample...]
* 18.03.2011: r426: codegen: Added bridge to protobuf: Ability to call ptotobuf services via Staff (SOAP). Please see the [ExampleProtobufBridge protobuf bridge service example];
* 03.03.2011: codegen: Added support for enums (wsdl, cpp, protobuf);
* 19.02.2011: Added new download location(ftp) for [VirtualMachineStaffSDK Virtual Machine Staff SDK];
* 17.02.2011: Tarballs svn revision 401 are available in [http://code.google.com/p/staff/downloads/list Download section]: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp4-r401.7z src] [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp4-r401.7z win32 bin];
* 16.02.2011: r399: Added support for building in MinGW environment. Please see [http://code.google.com/p/staff/downloads/detail?name=ws-win32-gcc4.5.2-staff-2.0.0-tp4-r399.7z Download section] [InstallationManualWindowsBinary Installation manual];
* 11.02.2011: r393: Codegen improvements: Google Protocol Buffers proto files can be generated from service"s interface;
* 2.02.2011: r391: Codegen improvements: 1. added protobuf plugin to generate from ".proto" files (google protocol buffers); 2. added support for structure nesting.
* 24.12.2010: Tarballs svn rev 386 is available in Download section: [http://code.google.com/p/staff/downloads/detail?name=staff-2.0.0-tp4-r386.7z src] [http://code.google.com/p/staff/downloads/detail?name=ws-win32-vs2005-staff-2.0.0-tp4-r386.7z win32 bin];
* 23.12.2010: Added support for Mac OS X;
* 7.12.2010: Updated [VirtualMachineStaffSDK Virtual Machine Staff SDK] (1,8Gb);
* 5.12.2010: Added FreeBSD support;
* 27.11.2010: Added Virtual Machine that includes preinstalled Staff SDK;
* 24.11.2010: Added traffic compression support in J2ME and AJAX;
'></a>

## Details ##

Staff is Web Service Framework for C++ that is based on Apache Axis2/C. WSF Staff wraps Apache Axis2/C code into C++, and also provides many additional features:
  * stateful [Web Services implementation](ExampleCalculatorService.md) in C++ with instance support;
  * [synchronous](ExampleCalculatorClient.md) and [asynchronous](ExampleCalculatorNonBlocking.md) client implementation in C++, [AJAX](ExampleCalculatorAjaxClient.md) and [J2ME](ExampleCalculatorJ2meClient.md);
  * support for RESTful [web services](ExampleCalculatorRestfulService.md) and [clients](ExampleCalculatorRestfulClient.md);
  * [WSDL](ExampleCreatingCppClientFromWsdl.md) support with xsd import/export;
  * access to relational databases via [DAS](ExampleDasDataSource.md) (data access service);
  * Google [Protocol Buffers](ExampleProtobufBridge.md) support.
[More...](StaffDetails.md)

[TODO Plans...](http://code.google.com/p/staff/wiki/ToDo)

## Development status ##

Current development status of this project is Beta 1.

Short project Roadmap of **2.0** Release:

tp1..tp6(1.99.0) -> a(1.99.1) -> **b(1.99.2)** -> rc(1.99.3) -> Release (2.0.0)

## Documentation ##

Please see [Wiki section](http://code.google.com/p/staff/wiki/Welcome?tm=6).

## Support ##

Feel free to ask any questions related to WSF Staff:
  * [WSF Staff on Google groups](https://groups.google.com/forum/?fromgroups#!forum/wsf-staff)
  * [Support Page](SupportPage.md)

## Help this project ##

If you want to help this project but not sure what you can do, see how to [help this project](HowToHelpThisProject.md).


If you find a bug in the program or documentation, please let us know: [open a new issue](http://code.google.com/p/staff/issues/entry)

You can also [request a new feature](http://code.google.com/p/staff/issues/entry?template=Feature%20request%20from%20user)

<p />




<a href='Hidden comment: 
<table border="0">
<tbody>
<tr>
<td valign="middle" width="20px">
<p/><g:plusone size="big">

Unknown end tag for &lt;/plusone&gt;




Unknown end tag for &lt;/td&gt;


<td valign="middle">
<wiki:gadget url="http://www.ohloh.net/p/488207/widgets/project_users_logo.xml" height="43" width="140" border="0"/>


Unknown end tag for &lt;/td&gt;




Unknown end tag for &lt;/tr&gt;




Unknown end tag for &lt;/tbody&gt;




Unknown end tag for &lt;/table&gt;




<wiki:gadget url="http://www.ohloh.net/p/488207/widgets/project_languages.xml" width="360" height="200" border="0"/>
'></a>